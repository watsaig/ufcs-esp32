#include "controller.h"

Controller::Controller()
    : mXIORefreshRequested(false)
{
    Wire.begin();
    mXioBoard.begin(LOW, LOW, LOW, XIO_RESET_PIN , XIO_OE_PIN);

    // Move the normally-open states to constants.h?
    mComponents[VALVE1] = new Valve(VALVE1_PIN, true);
    mComponents[VALVE2] = new Valve(VALVE2_PIN, true);
    mComponents[VALVE3] = new Valve(VALVE3_PIN, true);
    mComponents[VALVE4] = new Valve(VALVE4_PIN, true);
    mComponents[VALVE5] = new Valve(VALVE5_PIN, true);
    mComponents[VALVE6] = new Valve(VALVE6_PIN, true);
    mComponents[VALVE7] = new Valve(VALVE7_PIN, true);
    mComponents[VALVE8] = new Valve(VALVE8_PIN, true);

    mComponents[VALVE9] = new Valve(VALVE9_PIN, true);
    mComponents[VALVE10] = new Valve(VALVE10_PIN, true);
    mComponents[VALVE11] = new Valve(VALVE11_PIN, true);
    mComponents[VALVE12] = new Valve(VALVE12_PIN, true);
    mComponents[VALVE13] = new Valve(VALVE13_PIN, true);
    mComponents[VALVE14] = new Valve(VALVE14_PIN, true);
    mComponents[VALVE15] = new Valve(VALVE15_PIN, true);
    mComponents[VALVE16] = new Valve(VALVE16_PIN, true);

    mComponents[VALVE17] = new Valve(VALVE17_PIN, true);
    mComponents[VALVE18] = new Valve(VALVE18_PIN, true);
    mComponents[VALVE19] = new Valve(VALVE19_PIN, true);
    mComponents[VALVE20] = new Valve(VALVE20_PIN, true);
    mComponents[VALVE21] = new Valve(VALVE21_PIN, true);
    mComponents[VALVE22] = new Valve(VALVE22_PIN, true);
    mComponents[VALVE23] = new Valve(VALVE23_PIN, true);
    mComponents[VALVE24] = new Valve(VALVE24_PIN, true);

    mComponents[VALVE25] = new Valve(VALVE25_PIN, true);
    mComponents[VALVE26] = new Valve(VALVE26_PIN, true);
    mComponents[VALVE27] = new Valve(VALVE27_PIN, true);
    mComponents[VALVE28] = new Valve(VALVE28_PIN, true);
    mComponents[VALVE29] = new Valve(VALVE29_PIN, true);
    mComponents[VALVE30] = new Valve(VALVE30_PIN, true);
    mComponents[VALVE31] = new Valve(VALVE31_PIN, true);
    mComponents[VALVE32] = new Valve(VALVE32_PIN, true);

    mComponents[PUMP1] = new Pump(PUMP1_PIN);
    mComponents[PUMP2] = new Pump(PUMP2_PIN);

    mComponents[PR1] = new PressureController(PR1_SETPOINT_PIN,
                                              PR1_MEASUREMENT_PIN,
                                              DAC_MAX_VALUE,
                                              ADC_MAX_VALUE);

    mComponents[PR2] = new PressureController(PR2_SETPOINT_PIN,
                                              PR2_MEASUREMENT_PIN,
                                              DAC_MAX_VALUE,
                                              ADC_MAX_VALUE);

    mComponents[PR3] = new PressureController(PR3_SETPOINT_PIN,
                                              PR3_MEASUREMENT_PIN,
                                              PWM_MAX_VALUE,
                                              ADC_MAX_VALUE);



    // Set all pin modes, and default values (LOW; see Valve::Valve())
    mXioBoard.refreshPinModes();
    mXioBoard.refreshIO();

    mTimer = millis();
}

Controller::~Controller()
{}

// called by loop()
void Controller::update()
{
    // Send current pressures every 1 second (or so)

    /*
    if ((millis() - mTimer) > 1000) {
        sendComponentValue(PR1);
        sendComponentValue(PR2);
        sendComponentValue(PR3);

        mTimer = millis();
    }
    */

    // Pressure control: if the current pressure is above a certain threshold,
    // we turn the pump off. If it is below a certain threshold (all relative to
    // the setpoint), we turn the pump on.
    // Pressure controllers 1 and 2 are connected to pump 1 (positive pressure);
    // Pressure controller 3 is connected to pump 2 (vacuum).


    // For now: just one PR connected to 1 pump.

    /*
    PressureController * pc = dynamic_cast<PressureController*>(mComponents[PR1]);
    uint8_t setPoint = pc->setPointValue();
    uint8_t current = pc->getValue();

    if (current < PRESSURE_LOW_THRESHOLD_RATIO*setPoint) 
        mComponents[PUMP1]->setValue(ON);
    else if (current > PRESSURE_HIGH_THRESHOLD_RATIO*setpoint)
        mComponents[PUMP1]->setValue(OFF);

        */

    if (Serial.available())
        handleSerialData();

    if (mXIORefreshRequested) {
        mXioBoard.refreshIO();
        mXIORefreshRequested = false;
    }
}

void Controller::xioPinMode(int pin, int mode)
{
    mXioBoard.xioPinModeCached(pin, mode);
}

void Controller::xioDigitalWrite(int pin, int value)
{
    mXioBoard.xioDigitalWriteCached(pin, value);
    mXIORefreshRequested = true;
}

void Controller::handleSerialData()
{

    while (Serial.available() >= 2) {
        int length = Serial.available();
        uint8_t firstByte, secondByte;
        firstByte = Serial.read();
        secondByte = Serial.read();

        Log.notice("Received %d bytes: %d ; %d \n", length, firstByte, secondByte);

        if (firstByte == STATUS_REQUEST) {
            Log.notice("Status request for component %d \n", secondByte);
            if (secondByte == ALL_COMPONENTS) 
                sendAllComponentValues();
            else
                sendComponentValue(static_cast<ComponentID>(secondByte));
        }

        else if (firstByte >= VALVE1 && firstByte < ALL_COMPONENTS) {
            if (mComponents.count(static_cast<ComponentID>(firstByte))) {
                mComponents[static_cast<ComponentID>(firstByte)]->setValue(secondByte);
            }

        }
    }
}

void Controller::sendComponentValue(ComponentID component)
{
    if (mComponents.count(component)) {
        Serial.write(component);
        Serial.write(mComponents[component]->getValue());
    }
}

void Controller::sendAllComponentValues()
{
    for (auto const& i : mComponents) {
        sendComponentValue(i.first);
    }
}
