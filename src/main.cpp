#include <ArduinoLog.h>

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
    Serial.begin(115200);

    // Log level should be "LOG_LEVEL_VERBOSE" for debugging, and "LOG_LEVEL_ERROR" for production code.
    Log.begin(LOG_LEVEL_ERROR, &Serial);

    #ifdef BLUETOOTH_SERIAL
    SerialBT.begin("Microfluidics control system");
    #endif

    // Wait for serial to attach
    while (!Serial);

    #ifdef NEOPIXELS
    controller.initNeoPixelStrip();
    #endif
}

void loop()
{
    controller.update();
}
