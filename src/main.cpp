
#include "constants.h"
#include "controller.h"

#ifdef BLUETOOTH_SERIAL
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
#endif

// ----------------------------------------
// Global Controller instance. To access it, add `extern Controller controller` to the top of your cpp file.
// Alternatively, a singleton could be used, but the result is the same.
Controller controller;
// ----------------------------------------

void setup()
{
    #ifdef BLUETOOTH_SERIAL
        SerialBT.begin("Microfluidics control system");
    #else
        Serial.begin(115200);
    #endif

    delay(1000); // give peripherals time to start up
    controller.init();

    #ifdef NEOPIXELS
        controller.initNeoPixelStrip();
    #endif
}

void loop()
{
    controller.update();
}
