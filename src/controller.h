#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Map>
#include <Wire.h>
#include <ArduinoLog.h>

#include "constants.h"
#include "things.h"


class Controller {

public:
    Controller ();
    virtual ~Controller ();

    void update();

    void handleSerialData();

    void xioPinMode(int pin, int mode);
    void xioDigitalWrite(int pin, int value);

private:
    void sendComponentValue(ComponentID component);
    void sendAllComponentValues();

    unsigned long mTimer;

    XIO mXioBoard;
    bool mXIORefreshRequested;

    std::map<ComponentID, Thing*> mComponents;
};

#endif
