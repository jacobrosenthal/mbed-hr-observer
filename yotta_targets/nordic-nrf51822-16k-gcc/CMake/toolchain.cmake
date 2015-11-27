# Copyright (c) 2015 ARM Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(TARGET_NORDIC_NRF51822_16K_GCC_TOOLCHAIN_INCLUDED)
    return()
endif()
set(TARGET_NORDIC_NRF51822_16K_GCC_TOOLCHAIN_INCLUDED 1)

# Set S130 as the default SoftDevice if not defined through yotta config
if(NOT YOTTA_CFG_NORDIC_SOFTDEVICE)
    set(YOTTA_CFG_NORDIC_SOFTDEVICE "S130")
endif()

# legacy definitions for building mbed 2.0 modules with a retrofitted build
# system:
set(MBED_LEGACY_TARGET_DEFINITIONS "NORDIC" "NRF51822_MKIT" "MCU_NRF51822" "MCU_NORDIC_16K")
# provide compatibility definitions for compiling with this target: these are
# definitions that legacy code assumes will be defined.
add_definitions("-DNRF51 -DTARGET_NORDIC -DTARGET_M0 -D__MBED__=1 -DMCU_NORDIC_16K -DTARGET_NRF51822 -DTARGET_MCU_NORDIC_16K")

if(YOTTA_CFG_NORDIC_SOFTDEVICE STREQUAL "S110")
    add_definitions("-DTARGET_MCU_NRF51_16K -DTARGET_MCU_NRF51_16K_S110")
    set(MBED_LEGACY_TARGET_DEFINITIONS   ${MBED_LEGACY_TARGET_DEFINITIONS} "MCU_NRF51" "MCU_NRF51_16K" "MCU_NORDIC_16K_S110")
    set(NRF51822_LINKER_FLAGS_FILE_PATH  "${CMAKE_CURRENT_LIST_DIR}/../ld/NRF51822_S110.ld")
    set(NRF51822_SOFTDEVICE_FILE_PATH    "${CMAKE_CURRENT_LIST_DIR}/../softdevice/s110_nrf51822_8.0.0_softdevice.hex")
elseif(YOTTA_CFG_NORDIC_SOFTDEVICE STREQUAL "S130")
    set(NRF51822_LINKER_FLAGS_FILE_PATH  "${CMAKE_CURRENT_LIST_DIR}/../ld/NRF51822_S130.ld")
    set(NRF51822_SOFTDEVICE_FILE_PATH    "${CMAKE_CURRENT_LIST_DIR}/../softdevice/s130_nrf51_1.0.0_softdevice.hex")
else()
    message(FATAL_ERROR "SoftDevice version '${YOTTA_CFG_NORDIC_SOFTDEVICE}' is not recognized. Please check your yotta config file.")
endif()

# append non-generic flags, and set NRF51822-specific link script
set(_CPU_COMPILATION_OPTIONS "-mcpu=cortex-m0 -mthumb -D__thumb2__")

set(CMAKE_C_FLAGS_INIT             "${CMAKE_C_FLAGS_INIT} ${_CPU_COMPILATION_OPTIONS}")
set(CMAKE_ASM_FLAGS_INIT           "${CMAKE_ASM_FLAGS_INIT} ${_CPU_COMPILATION_OPTIONS}")
set(CMAKE_CXX_FLAGS_INIT           "${CMAKE_CXX_FLAGS_INIT} ${_CPU_COMPILATION_OPTIONS}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${CMAKE_MODULE_LINKER_FLAGS_INIT} -mcpu=cortex-m0 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "${CMAKE_EXE_LINKER_FLAGS_INIT} -mcpu=cortex-m0 -mthumb -T\"${NRF51822_LINKER_FLAGS_FILE_PATH}\"")

# used by the apply_target_rules function below:
set(NRF51822_SOFTDEVICE_HEX_FILE    "${NRF51822_SOFTDEVICE_FILE_PATH}")
set(NRF51822_MERGE_HEX_SCRIPT       "${CMAKE_CURRENT_LIST_DIR}/../scripts/merge_hex.py")
set(NRF51822_GEN_DAT_SCRIPT         "${CMAKE_CURRENT_LIST_DIR}/../scripts/generate_dat.py")
set(NRF51822_BOOTLOADER_HEX_FILE    "${CMAKE_CURRENT_LIST_DIR}/../../../yotta_modules/ble-nrf51822/bootloader/s130_nrf51_1.0.0_bootloader.hex")
set(NRF51822_MEMORY_INFO_SCRIPT     "${CMAKE_CURRENT_LIST_DIR}/../scripts/memory_info.py")
set(NRF51822_HEAP_WARNING_THRESHOLD 1024)

# define a function for yotta to apply target-specific rules to build products,
# in our case we need to convert the built elf file to .hex, and add the
# pre-built softdevice:
function(yotta_apply_target_rules target_type target_name)
    if(${target_type} STREQUAL "EXECUTABLE")
        add_custom_command(TARGET ${target_name}
            POST_BUILD
            # objcopy to hex
            COMMAND arm-none-eabi-objcopy -O ihex ${target_name} ${target_name}.hex
            # and append the softdevice hex file
            COMMAND python ${NRF51822_MERGE_HEX_SCRIPT} ${NRF51822_SOFTDEVICE_HEX_FILE} ${target_name}.hex ${target_name}-combined.hex
            COMMENT "hexifying and adding softdevice to ${target_name}"
            COMMAND "${ARM_NONE_EABI_OBJCOPY}" -O binary ${target_name} ${target_name}.bin
            COMMENT "converting to .bin"
            # generate dfu .dat from bin
            COMMAND python ${NRF51822_GEN_DAT_SCRIPT} ${target_name}.bin
            COMMENT "generating .dat"
            COMMAND srec_cat ${NRF51822_SOFTDEVICE_HEX_FILE} -intel ${target_name}.hex -intel ${NRF51822_BOOTLOADER_HEX_FILE} -intel -exclude 0x3FC00 0x3FC20 -generate 0x3FC00 0x3FC04 -l-e-constant 0x01 4 -generate 0x3FC04 0x3FC08 -l-e-constant 0x00 4 -generate 0x3FC08 0x3FC0C -l-e-constant 0xFE 4 -generate 0x3FC0C 0x3FC20 -constant 0x00 -o ${target_name}-combined-bootloader.hex -intel
            COMMENT "building fota ${target_name}-combined-bootloader.hex"
            VERBATIM
        )
        add_custom_command(TARGET ${target_name}
            POST_BUILD
            # printing memory usage information
            COMMAND python ${NRF51822_MEMORY_INFO_SCRIPT} ${target_name} ${NRF51822_HEAP_WARNING_THRESHOLD}
            COMMENT "printing memory usage information"
            VERBATIM
        )
    endif()
endfunction()
