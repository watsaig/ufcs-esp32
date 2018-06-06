#ifndef THINGS_H
#define THINGS_H

#include <Wire.h>
#include <ArduinoLog.h>

#include "constants.h"

/** 
 * Abstract class describing an electro/pneumatic component, i.e valve, pump, 
 * pressure controller. I can't think of a term that encompasses all of these, 
 * hence the current name. 
 * The point of this class structure is to offer a very simple interface to any
 * component, through the setValue and getValue functions.
 *
 * All values passed to/by these functions are bytes, i.e of type uint8_t. The
 * enums defined in constants.h provide values for pumps and valves (ON / OFF
 * and OPEN / CLOSED, resp.). For the pressure regulators, the setpoint and measured
 * values are an integer between 0 and 255, with 255 being the maximum pressure
 * supported by that regulator.
 */

class Thing
{
public:
    Thing ();
    virtual ~Thing ();

    virtual void setValue(uint8_t) = 0;
    virtual uint8_t getValue() = 0;
};


class Valve : public Thing
{
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

class Pump : public Thing
{
public:
    Pump (int pin);
    virtual ~Pump ();

    virtual void setValue(uint8_t);
    virtual uint8_t getValue();

protected:
    int mPin;
    uint8_t mValue; // ON or OFF
};

class PressureController : public Thing
{
public:
    enum interfaceType {
        analog,
        i2c
    };

    // Use this constructor for analog (Parker-Hannifin) pressure controllers
    PressureController (int setPointPin, int measurementPin,
                        int setPointMaxValue, int measurementMaxValue);

    // Use this constructor for i2c pressure controllers
    PressureController (int i2cAddress);

    virtual ~PressureController ();

    virtual void setValue(uint8_t);
    virtual uint8_t getValue();

    uint8_t setPointValue() { return mSetPointValue; }

protected:
    int mSetPointPin;
    int mMeasurementPin;

    uint8_t mSetPointValue;
    uint8_t mMeasuredValue;

    int mSetPointMaxValue;
    int mMeasurementMaxValue;

    interfaceType mInterface; // a touch redundant. we could just check whether i2cAddress == -1
    int mI2cAddress;
};

#endif
