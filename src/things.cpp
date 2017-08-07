#include "things.h"
#include "controller.h"

extern Controller controller;

Thing::Thing()
{}

Thing::~Thing()
{}


Valve::Valve(int pin, bool normallyOpen)
    : Thing()
    , mPin(pin)
    , mNormallyOpen(normallyOpen)
    , mValue(normallyOpen ? OPEN : CLOSED)
{
    // Valves are intially powered off, i.e their initial value depends on whether they are
    // normally open or normally closed.

    controller.xioPinMode(mPin, OUTPUT);
    controller.xioDigitalWrite(mPin, LOW);
}

Valve::~Valve()
{}

void Valve::setValue(uint8_t value)
{
    /* 
       setValue calls xioDigitalWrite, since all valves are connected to 
       the IO expander. If this weren't the case, then it would be necessary
       to handle calling either digitalWrite or xioDigitalWrite based on the
       component's pin.
    */

    if (value != mValue) {
        mValue = value;

        int pinLevel = LOW;
        if ((value == OPEN && !mNormallyOpen)
                || (value == CLOSED && mNormallyOpen))
            pinLevel = HIGH;

        Log.notice("Switching valve %s \n", value == OPEN ? "open" : "closed");

        controller.xioDigitalWrite(mPin, pinLevel); 
    }
}

uint8_t Valve::getValue()
{
    return mValue;
}


Pump::Pump(int pin)
    : Thing()
    , mPin(pin)
    , mValue(OFF)
{
    pinMode(mPin, OUTPUT);
    digitalWrite(mPin, LOW);
}

Pump::~Pump()
{}

void Pump::setValue(uint8_t value)
{
    if (value != mValue) {
        mValue = value;
        int pinLevel = (value == ON ? HIGH : LOW);

        Log.notice("Switching pump %s \n", value == ON ? "on" : "off");

        digitalWrite(mPin, pinLevel);
    }
}

uint8_t Pump::getValue()
{
    return mValue;
}


PressureController::PressureController(int setPointPin,
                                       int measurementPin,
                                       int setPointMaxValue,
                                       int measurementMaxValue)
    : Thing()
    , mSetPointPin(setPointPin)
    , mMeasurementPin(measurementPin)
    , mSetPointValue(0)
    , mMeasuredValue(0)
    , mSetPointMaxValue(setPointMaxValue)
    , mMeasurementMaxValue(measurementMaxValue)
{
    pinMode(setPointPin, OUTPUT);
    pinMode(measurementPin, INPUT);

    if (mSetPointPin == PR3_SETPOINT_PIN) {
        ledcAttachPin(PR3_SETPOINT_PIN, 1);
        ledcSetup(1, PWM_FREQ, PWM_RESOLUTION);
    }

    setValue(0);
}

PressureController::~PressureController()
{}

void PressureController::setValue(uint8_t value)
{

    // check that value is between zero and the max value allowed
    // rescale to match DAC resolution, if necessary
    if (value != mSetPointValue) {

        mSetPointValue = value;
        Log.notice("Setting pressure to %d \n", value);

        int toWrite = value;
        // Rescale the value if necessary
        if (mSetPointMaxValue != UINT8_MAX) 
            int toWrite = double(value)/double(UINT8_MAX) * mSetPointMaxValue;

        if (mSetPointPin == DAC0 || mSetPointPin == DAC1) 
            dacWrite(mSetPointPin, toWrite);
        else 
            ledcWrite(1, toWrite);

    }
}

uint8_t PressureController::getValue()
{
    int val = analogRead(mMeasurementPin);

    // Rescale value to match range of uint8_t (0 to 255)
    mMeasuredValue = double(val)/double(mMeasurementMaxValue) * UINT8_MAX;

    return mMeasuredValue;
}
