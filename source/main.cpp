#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"
#include "us_ticker_api.h"

// generally not enough memory for buttons, leds, and printing
#define BUTTON
#define LED
#define printf(...)

#ifdef BUTTON
InterruptIn button(BUTTON1);
#endif

#ifdef LED
DigitalOut led(LED1, 1);
minar::callback_handle_t ledCallback;
static const uint16_t onTime = 200;
uint16_t offTime = 200;
#endif

unsigned long lastAdvertisement = 0;
static const int ADVERTISEMENT_TIMEOUT_SECONDS = 10;
minar::callback_handle_t scanCallback;

uint8_t peer0 = 0;
uint8_t peer1 = 0;

#ifdef LED
void toggle()
{
  if(led)
  {
    ledCallback = minar::Scheduler::postCallback(toggle).delay(minar::milliseconds(onTime)).getHandle();
  }else
  {
    ledCallback = minar::Scheduler::postCallback(toggle).delay(minar::milliseconds(offTime)).getHandle();
  }
  led = !led;
}
#endif

bool isHeartrate(const uint8_t *advertisingData, uint8_t advertisingDataLen)
{
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

uint8_t getHeartRate(const uint8_t *advertisingData, uint8_t advertisingDataLen)
{
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

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{
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

        lastAdvertisement = us_ticker_read();
        uint8_t heartRate = getHeartRate(params->advertisingData, params->advertisingDataLen);

#ifdef LED
        if(heartRate){
            // printf("hr: %d\r\n", heartRate);
            // printf("offtime: %d\r\n", offTime);
            offTime = (-8.33f * (float)heartRate) + (1500.0f - (float)onTime );
        }
#endif
    }
}

void onBleInitError(BLE &ble, ble_error_t error)
{
   /* Initialization error handling should go here */
}

void stopScan()
{
    // printf("stop scan");
    // unsigned long difference = us_ticker_read() - lastAdvertisement;
    // printf("%lu", difference);

    // turn off led, our leds are inverted
#ifdef LED
    led = 1;
    minar::Scheduler::cancelCallback(ledCallback);
#endif

    minar::Scheduler::cancelCallback(scanCallback);

    // lose the old 'pairing'
    peer0 = 0;
    peer1 = 0;

    BLE::Instance().gap().stopScan();

    lastAdvertisement = 0;
}

void timeout()
{
    if( (us_ticker_read() - lastAdvertisement) > (ADVERTISEMENT_TIMEOUT_SECONDS * 1000000) ){
        minar::Scheduler::postCallback(stopScan);
    }
}

void startScan()
{
    // printf("start scan");
    lastAdvertisement = us_ticker_read();
#ifdef LED
    ledCallback = minar::Scheduler::postCallback(toggle).getHandle();
#endif
    scanCallback = minar::Scheduler::postCallback(timeout).period(minar::milliseconds(ADVERTISEMENT_TIMEOUT_SECONDS * 1000)).getHandle();
    BLE::Instance().gap().startScan(advertisementCallback);
}

#ifdef BUTTON
void buttonCallback()
{
    if(!lastAdvertisement){
        minar::Scheduler::postCallback(startScan);
    }else{
        minar::Scheduler::postCallback(stopScan);
    }
}
#endif

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
}
    
void app_start(int, char**)
{
    BLE::Instance().init(bleInitComplete);

#ifdef BUTTON
    button.rise(buttonCallback);
#else
    startScan();
#endif
}
