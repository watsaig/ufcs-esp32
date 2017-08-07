#ifndef THINGS_H
#define THINGS_H

#include <Wire.h>
#include <ArduinoLog.h>

#include "constants.h"

/* Abstract class describing an electro/pneumatic component, i.e valve, pump, pressure controller. I can't think of a term that encompasses all of these, hence the current name */

class Thing {

public:
    Thing ();
    virtual ~Thing ();

    virtual void setValue(uint8_t) = 0;
    virtual uint8_t getValue() = 0;
protected:
    //int mID;
};


class Valve : public Thing {
public:
    Valve (int pin, bool normallyOpen);
    virtual ~Valve ();

    virtual void setValue(uint8_t); 
    virtual uint8_t getValue();

protected:
    int mPin;
    bool mNormallyOpen;
    uint8_t mValue; // CLOSED or OPEN
};

class Pump : public Thing {
public:
    Pump (int pin);
    virtual ~Pump ();

    virtual void setValue(uint8_t);
    virtual uint8_t getValue();

protected:
    int mPin;
    uint8_t mValue; // ON or OFF
};

class PressureController : public Thing {
public:
    PressureController (int setPointPin, int measurementPin, int setPointMaxValue, int measurementMaxValue);
    virtual ~PressureController ();

    virtual void setValue(uint8_t); // value should be between 0 and 255. Rescaled internally.
    virtual uint8_t getValue();

    uint8_t setPointValue() { return mSetPointValue; }

protected:
    int mSetPointPin;
    int mMeasurementPin;

    uint8_t mSetPointValue;
    uint8_t mMeasuredValue;

    int mSetPointMaxValue;
    int mMeasurementMaxValue;
};

#endif
