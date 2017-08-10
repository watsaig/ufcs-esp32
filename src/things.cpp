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

        /*
        int toWrite = value;
        // Rescale the value if necessary
        if (mSetPointMaxValue != UINT8_MAX) 
            int toWrite = double(value)/double(UINT8_MAX) * mSetPointMaxValue;
            */

        double x = double(value)/double(UINT8_MAX);

        if (mSetPointPin == DAC0 || mSetPointPin == DAC1) {
            double y = 0.3306*pow(x, 3) - 0.428*pow(x, 2) + 1.0961*x - 0.0258;
            int toWrite = std::min(mSetPointMaxValue, 
                                   std::max(0, int(round(y*mSetPointMaxValue))));

            dacWrite(mSetPointPin, toWrite);
        }
        else {

            double y = 0.3689*pow(x, 3) - 0.4627*pow(x, 2) + 1.0525*x - 0.004;

            int toWrite = std::min(mSetPointMaxValue, 
                                   std::max(0, int(round(y*mSetPointMaxValue))));
            ledcWrite(1, toWrite);
        }
    }
}

uint8_t PressureController::getValue()
{
    int val = analogRead(mMeasurementPin);

    /* The result of analogRead doesn't correspond linearly to the voltage applied;
       see: https://github.com/espressif/esp-idf/issues/164
       So a calibration curve was established by applying voltages between 0 and 5v
       to the pressure controller pins, recording the result of analogRead, and fitting
       a polynomial to the data.
       If and when Espressif fix the analogRead() function to take into account the
       non-linearity, the following fit can be replaced by a simple rescaling
    */

    double x = val;
    double y = -3e-12*pow(x, 3) - 7e-10*pow(x, 2) + 0.0003*x + 0.0169;

    mMeasuredValue = std::min(255, std::max(0, int(round(y*UINT8_MAX))));

    // Rescale value to match range of uint8_t (0 to 255)
    //mMeasuredValue = double(val)/double(mMeasurementMaxValue) * UINT8_MAX;

    return mMeasuredValue;
}
