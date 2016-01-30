* pwm appears broken on the nrf51822 targets right now https://github.com/ARMmbed/mbed-hal-nrf51822-mcu/issues/42 *

Install [yotta](http://yottadocs.mbed.com/):
* `brew tap ARMmbed/homebrew-formulae`
* `brew install python cmake ninja arm-none-eabi-gcc srecord`
* `pip install yotta awscli`

To Build
* `yt build`

To upload with a wire
* `cp build/tinyble-gcc/source/ble-eddystoneservice-combined-fota.hex /Volumes/MBED/`

To debug
* `yt build -d` - skip and debug optimized until https://github.com/ARMmbed/nrf51-sdk/issues/20
* `yt debug mbed-hr-observer`
* `load`
* `mon reset halt`
*  set `breakpoint` or continue `c`