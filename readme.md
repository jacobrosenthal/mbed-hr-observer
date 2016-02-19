# mbed-hr-observer

Watch for [Polar H7](http://www.polar.com/us-en/products/accessories/H7_heart_rate_sensor) heartrate ble advertisement packets and blink out heartrate. By default uses the [Tinyble from Seeed Studio](http://www.seeedstudio.com/depot/Seeed-Tiny-BLE-BLE-6DOF-Mbed-Platform-p-2268.html). Power on your H7 and then hit the tinyble button to start a search for your device. 

Install [yotta](http://yottadocs.mbed.com/):
* `brew tap ARMmbed/homebrew-formulae`
* `brew install python cmake ninja arm-none-eabi-gcc`
* `pip install yotta`

Build
* `yt build`

Upload with a wire
* `cp build/tinyble-gcc/source/mbed-hr-observer-combined-fota.hex /Volumes/MBED/`

Debug
* `yt build -d`
* `yt debug mbed-hr-observer`
* `load`
* `mon reset halt`
*  set `breakpoint` or continue `c`
