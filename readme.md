# mbed-hr-observer

Watch for [Polar H7](http://www.polar.com/us-en/products/accessories/H7_heart_rate_sensor) heartrate ble advertisement packets and blink out heartrate. By default uses the [Tinyble from Seeed Studio](http://www.seeedstudio.com/depot/Seeed-Tiny-BLE-BLE-6DOF-Mbed-Platform-p-2268.html). Power on your H7 and then hit the tinyble button to start a search for your device. 

#### [pwm appears broken on the nrf51822 targets right now](https://github.com/ARMmbed/mbed-hal-nrf51822-mcu/issues/42)

Install [yotta](http://yottadocs.mbed.com/):
* `brew tap ARMmbed/homebrew-formulae`
* `brew install python cmake ninja arm-none-eabi-gcc`
* `pip install yotta`

Build
* `yt build`

Upload with a wire
* `cp build/tinyble-gcc/source/mbed-hr-observer-combined-fota.hex /Volumes/MBED/`

Debug
* `yt build -d` - skip this step and debug optimized as [unoptimized building appears broken right now](https://github.com/ARMmbed/nrf51-sdk/issues/20)
* `yt debug mbed-hr-observer`
* `load`
* `mon reset halt`
*  set `breakpoint` or continue `c`