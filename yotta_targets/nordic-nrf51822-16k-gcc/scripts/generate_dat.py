#!/usr/bin/python

# genereate .dat and .zip for an application binary
# todo, take softdevice and bootloaders and do whatever alterations to the hex necesary 
# https://github.com/NordicSemiconductor/nRF-Master-Control-Panel/blob/master/init%20packet%20handling/How%20to%20generate%20the%20INIT%20file%20for%20DFU.pdf
# code shamelessly stolen from https://devzone.nordicsemi.com/question/22586/anyone-do-successfully-dfu-ota-for-sdk701-softdevice-s110-v710/


import argparse, sys, zipfile, glob, os

def calc_crc16(binfile):
  crc = 0xFFFF

  for b in binfile:
    crc = (crc >> 8 & 0x00FF) | (crc << 8 & 0xFF00)
    crc = crc ^ b
    crc = crc ^ ( ( crc & 0x00FF) >> 4 )
    crc = crc ^ ( ( crc << 8) << 4)
    crc = crc ^ ( ( crc & 0x00FF) << 4) << 1
  return crc & 0xFFFF

def convert_uint16_to_array(value):
    """ Convert a number into an array of 2 bytes (LSB). """
    return [(value >> 0 & 0xFF), (value >> 8 & 0xFF)]

def _create_init_packet(init_packet):
    hardware_version = [0xFF, 0xFF]
    hardware_revision = [0xFF, 0xFF]
    application_version = [0xFF, 0xFF, 0xFF, 0xFF]
    softdevice_len = [0x01, 0x00]
    softdevice_array = [0xFE, 0xFF]

    init_packet.extend(hardware_version)
    init_packet.extend(hardware_revision)
    init_packet.extend(application_version)
    init_packet.extend(softdevice_len)
    init_packet.extend(softdevice_array)

def _create_manifest(bin_file, dat_file, application_version, device_revision, device_type, firmware_crc16, softdevice_req):
  manifest_text = """
  {
    "manifest": {
        "application": {
            "bin_file": "%s",
            "dat_file": "%s",
            "init_packet_data": {
                "application_version": %d,
                "device_revision": %d,
                "device_type": %d,
                "firmware_crc16": %d,
                "softdevice_req": [
                    %d
                ]
            }
        },
        "dfu_version": 0.5
    }
}"""
  return manifest_text % (bin_file, dat_file, application_version, device_revision, device_type, firmware_crc16, softdevice_req)

def main():

  parser = argparse.ArgumentParser(description='generate a .bin.dat file from an input bin file.')
  parser.add_argument ('file', metavar = 'file', type = str, help = 'The binary to crc')
  args = parser.parse_args()

  name = args.file.split('.')[0]
  dat_out_name =  name + '.dat'
  manifest_name = 'manifest.json'
  archive_name = name + '.zip'

  try:
    bin = open(args.file, 'rb')

  except:
    sys.exit('Could not open %s' % args.file)

  try:
    dat = open(dat_out_name, 'w')

  except:
    sys.exit('Could not open/create %s' % dat_out_name)

  try:
    manifest = open(manifest_name, 'w')

  except:
    sys.exit('Could not open/create %s' % manifest_name)

  try:
    archive = zipfile.ZipFile(archive_name, 'w')

  except:
    sys.exit('Could not open/create %s' % archive_name)

  init_packet = []
  _create_init_packet(init_packet)

  ba = bytearray(bin.read())
  bin.close();

  crc = calc_crc16(ba)
  init_packet.extend(convert_uint16_to_array(crc))

  dat.write(bytearray(init_packet))
  dat.close()

  application_version = 4294967295
  device_revision = 65535
  device_type = 65535
  softdevice_req = 65534

  manifest_text = _create_manifest(args.file, dat_out_name, application_version, device_revision, device_type, crc, softdevice_req)
  manifest.write(manifest_text);
  manifest.close()

  archive.write(dat_out_name, os.path.basename(dat_out_name), zipfile.ZIP_DEFLATED)
  archive.write(manifest_name, os.path.basename(manifest_name), zipfile.ZIP_DEFLATED)
  archive.write(args.file, os.path.basename(args.file), zipfile.ZIP_DEFLATED)
  archive.close()

if __name__ == "__main__":
  main()