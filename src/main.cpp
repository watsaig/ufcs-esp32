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

// Log level should be "LOG_LEVEL_VERBOSE" for debugging, and "LOG_LEVEL_ERROR" for production code.
#define LOG_LEVEL LOG_LEVEL_ERROR
void setup()
{
    Serial.begin(38400);
    delay(1000); // give peripherals time to start up

    controller.init();

    #ifdef BLUETOOTH_SERIAL
        SerialBT.begin("Microfluidics control system");
        // Sending long messages over bluetooth seems to be buggy; in the meantime, logging is enabled only for USB.
        //Log.begin(LOG_LEVEL, &SerialBT);
    #else
        Log.begin(LOG_LEVEL, &Serial);
    #endif

    #ifdef NEOPIXELS
        controller.initNeoPixelStrip();
    #endif
}

void loop()
{
    controller.update();
}
