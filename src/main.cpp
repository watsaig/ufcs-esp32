#include <ArduinoLog.h>

#include "constants.h"
#include "controller.h"

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

    // Wait for serial to attach
    while (!Serial);
}

void loop()
{
    controller.update();
}
