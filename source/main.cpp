/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"

DigitalOut alivenessLED(LED1, 1);

uint8_t peer0 = 0;
uint8_t peer1 = 0;
minar::callback_handle_t hrHandle;

bool isHeartrate(const uint8_t *advertisingData, uint8_t advertisingDataLen){
    bool found = false;

    uint8_t len = advertisingDataLen;
    uint8_t pos = 0;
    while(len > pos) {
        uint8_t adlen = *(advertisingData+pos);
        uint8_t type  = *(advertisingData+pos+1);

        // printf("type: %02x\r\n", type);

        if (type == GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS) {
            uint8_t loweruuid = *(advertisingData+pos+2);
            uint8_t upperuuid = *(advertisingData+pos+3);

            // printf("S UUID: %02x%02x\r\n", upperuuid, loweruuid);

            if(loweruuid == 0x0d && upperuuid == 0x18){
                found = true;
                break;
            }
        }
        pos = pos + adlen + 1;
    }
    return found;
}

uint8_t getHeartRate(const uint8_t *advertisingData, uint8_t advertisingDataLen){
    uint8_t hr = 0;

    uint8_t len = advertisingDataLen;
    uint8_t pos = 0;
    while(len > pos) {
        uint8_t adlen = *(advertisingData+pos);
        uint8_t type  = *(advertisingData+pos+1);
        if (type == GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA) {

            // printf("MF Data: %02x\r\n", *(advertisingData+pos+2));

            //heart rate like 6b0003004c
            hr = *(advertisingData+pos+2+4);
            break;
        }
        pos = pos + adlen + 1;
    }
    return hr;
}

// void periodicCallback(void) {
//     alivenessLED = !alivenessLED; /* Do blinky on LED1 while we're waiting for BLE events */
// }

void periodicCallback(void) {
    alivenessLED = !alivenessLED; /* Do blinky on LED1 while we're waiting for BLE events */
}

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params) {

    //no device saved, find one
    if (peer0 == 0 && peer1 == 0){

        //note, Polar only advertises these right after power on
        if(isHeartrate(params->advertisingData, params->advertisingDataLen)){
            peer0 = params->peerAddr[0];
            peer1 = params->peerAddr[1];
        }
        return;
    }

    //if this is our device, get its heart rate from advertising packet
    if (params->peerAddr[0] == peer0 && params->peerAddr[1] == peer1) {
        uint8_t heartRate = getHeartRate(params->advertisingData, params->advertisingDataLen);

        if(heartRate){
            printf("%d\r\n", heartRate);
        }
    }
}

void onBleInitError(BLE &ble, ble_error_t error)
{
   /* Initialization error handling should go here */
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().setScanParams(500, 450, 0, true);
    ble.gap().startScan(advertisementCallback);
}
    
void app_start(int, char**) {

    /* Tell standard C library to not allocate large buffers for these streams */
    // setbuf(stdout, NULL);
    // setbuf(stderr, NULL);
    // setbuf(stdin, NULL);

    minar::Scheduler::postCallback(periodicCallback).period(minar::milliseconds(500));

    BLE::Instance().init(bleInitComplete);
}