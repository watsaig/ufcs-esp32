#include "controller.h"

#ifdef NEOPIXELS
int colorSaturation = 32;
int pixelCount = 37;

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation/3);
RgbColor black(0);

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> mStrip(pixelCount, NEOPIXELS_DATA_PIN);

void Controller::initNeoPixelStrip()
{
    mStrip.Begin();
    delay(1);
    mStrip.Show();

    for (size_t i = VALVE1; i <= VALVE32; i++) {
        if (mComponents[static_cast<ComponentID>(i)]->getValue() == OPEN)
            mStrip.SetPixelColor(i, green);
        else
            mStrip.SetPixelColor(i, red);
    }

    delay(1);
    mStrip.Show();
}

void Controller::setNeoPixel(int index, const RgbColor& color)
{
    mStrip.SetPixelColor(index, color);
    delay(1);
    mStrip.Show();
}

#endif

#ifdef BLUETOOTH_SERIAL
#include <BluetoothSerial.h>
extern BluetoothSerial SerialBT;
#endif

Controller::Controller()
    : mXIORefreshRequested(false)
{
}

void Controller::init()
{
    Wire.begin();
    mXioBoard.begin(LOW, LOW, LOW, XIO_RESET_PIN , XIO_OE_PIN);

    mComponents[VALVE1] = new Valve(VALVE1_PIN);
    mComponents[VALVE2] = new Valve(VALVE2_PIN);
    mComponents[VALVE3] = new Valve(VALVE3_PIN);
    mComponents[VALVE4] = new Valve(VALVE4_PIN);
    mComponents[VALVE5] = new Valve(VALVE5_PIN);
    mComponents[VALVE6] = new Valve(VALVE6_PIN);
    mComponents[VALVE7] = new Valve(VALVE7_PIN);
    mComponents[VALVE8] = new Valve(VALVE8_PIN);

    mComponents[VALVE9] = new Valve(VALVE9_PIN);
    mComponents[VALVE10] = new Valve(VALVE10_PIN);
    mComponents[VALVE11] = new Valve(VALVE11_PIN);
    mComponents[VALVE12] = new Valve(VALVE12_PIN);
    mComponents[VALVE13] = new Valve(VALVE13_PIN);
    mComponents[VALVE14] = new Valve(VALVE14_PIN);
    mComponents[VALVE15] = new Valve(VALVE15_PIN);
    mComponents[VALVE16] = new Valve(VALVE16_PIN);

    mComponents[VALVE17] = new Valve(VALVE17_PIN);
    mComponents[VALVE18] = new Valve(VALVE18_PIN);
    mComponents[VALVE19] = new Valve(VALVE19_PIN);
    mComponents[VALVE20] = new Valve(VALVE20_PIN);
    mComponents[VALVE21] = new Valve(VALVE21_PIN);
    mComponents[VALVE22] = new Valve(VALVE22_PIN);
    mComponents[VALVE23] = new Valve(VALVE23_PIN);
    mComponents[VALVE24] = new Valve(VALVE24_PIN);

    mComponents[VALVE25] = new Valve(VALVE25_PIN, false);
    mComponents[VALVE26] = new Valve(VALVE26_PIN, false);
    mComponents[VALVE27] = new Valve(VALVE27_PIN, false);
    mComponents[VALVE28] = new Valve(VALVE28_PIN, false);
    mComponents[VALVE29] = new Valve(VALVE29_PIN, false);
    mComponents[VALVE30] = new Valve(VALVE30_PIN, false);
    mComponents[VALVE31] = new Valve(VALVE31_PIN, false);
    mComponents[VALVE32] = new Valve(VALVE32_PIN, false);

    mComponents[PUMP1] = new Pump(PUMP1_PIN);
    mComponents[PUMP2] = new Pump(PUMP2_PIN);

    /*
    mComponents[PR1] = new PressureController(PR1_SETPOINT_PIN,
                                              PR1_MEASUREMENT_PIN,
                                              DAC_MAX_VALUE,
                                              ADC_MAX_VALUE);
      */
    mComponents[PR1] = new PressureController(PR1_I2C_ADDRESS);
    mComponents[PR2] = new PressureController(PR2_I2C_ADDRESS);

    /*
    mComponents[PR2] = new PressureController(PR2_SETPOINT_PIN,
                                              PR2_MEASUREMENT_PIN,
                                              DAC_MAX_VALUE,
                                              ADC_MAX_VALUE);
                                              */


    // Set all pin modes, and default values (LOW; see Valve::Valve())
    mXioBoard.refreshPinModes();
    mXioBoard.refreshIO();

    mTimer = millis();
    mPressureControlTimer = millis();
    mPumpLastSwitchOnTime = millis();

}

Controller::~Controller()
{}

/**
  * This function should be called by loop().
  */
void Controller::update()
{
    // Send current pressures every 1 second (or so)

    if ((millis() - mTimer) > 500) {
        sendComponentValue(PR1);
        sendComponentValue(PR2);

        mTimer = millis();
    }

    if ((millis() - mPressureControlTimer) > 500) {
        pressureControl();
        mPressureControlTimer = millis();
    }

#ifdef BLUETOOTH_SERIAL
    if (SerialBT.available())
#else
    if (Serial.available())
#endif
        handleSerialData();

    if (mXIORefreshRequested) {
        mXioBoard.refreshIO();
        mXIORefreshRequested = false;
    }
}

/**
  * Replacement of pinMode() for components connected to the PCA9896
  */
void Controller::xioPinMode(int pin, int mode)
{
    mXioBoard.xioPinModeCached(pin, mode);
}

/**
  * Replacement of digitalWrite() for components connected to the PCA9896
  *
  * The value is not set immediately, but on the next run of update(). This is to
  * avoid sending too many requests via the i2c bus, which would be too slow.
  * (see XIO library reference for more details)
  */
void Controller::xioDigitalWrite(int pin, int value)
{
    mXioBoard.xioDigitalWriteCached(pin, value);
    mXIORefreshRequested = true;
}

/**
  * @brief Read data from the serial buffer, and execute any instructions received.
  *
  * Data is exchanged between the microcontroller and the PC in two-byte packets.
  * The PC can either request the status of one or all components, or request a new
  * setpoint for a given component.
  * In the first case, the first byte will be STATUS_REQUEST, and the second byte
  * will be a component ID (see constants.h).
  * In the second case, the first byte will be a component ID, and the second byte, the
  * desired setpoint.
  */
void Controller::handleSerialData()
{

#ifdef BLUETOOTH_SERIAL
    while (SerialBT.available() >= 2) {
        int length = SerialBT.available();
        uint8_t firstByte, secondByte;
        firstByte = SerialBT.read();
        secondByte = SerialBT.read();
#else
    while (Serial.available() >= 2) {
        int length = Serial.available();
        uint8_t firstByte, secondByte;
        firstByte = Serial.read();
        secondByte = Serial.read();

#endif
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
                // Set requested value and communicate the new state
                mComponents[static_cast<ComponentID>(firstByte)]->setValue(secondByte);
                sendComponentValue(static_cast<ComponentID>(firstByte));

#ifdef NEOPIXELS
                setNeoPixel(firstByte - VALVE1, (secondByte == OPEN ? green : red));
#endif
            }

        }
    }
}

/**
  * @brief Write the value of the given component to serial.
  */
void Controller::sendComponentValue(ComponentID component)
{
    if (mComponents.count(component)) {
        uint8_t toSend[4] = {START_BYTE, (uint8_t)component, (uint8_t)mComponents[component]->getValue(), END_BYTE};
        #ifdef BLUETOOTH_SERIAL
            SerialBT.write(toSend, 4);
        #else
            Serial.write(toSend, 4);
        #endif
    }
}

/**
  * @brief Write the value of all components to serial.
  */
void Controller::sendAllComponentValues()
{
    for (auto const& i : mComponents) {
        sendComponentValue(i.first);
    }

    sendUptime();
}

/**
  * @brief Send an error code over serial.
  */
void Controller::sendErrorCode(Errors code)
{
    uint8_t toSend[4] = {START_BYTE, ERROR, code, END_BYTE};
    #ifdef BLUETOOTH_SERIAL
        SerialBT.write(toSend, 4);
    #else
        Serial.write(toSend, 4);
    #endif
}

/**
  * @brief Send the current uptime (in seconds) over serial.
  */
void Controller::sendUptime()
{
    unsigned long uptime = millis() / 1000;
    uint8_t value0, value1, value2, value3;

    value0 = (uptime & 0xFF000000) >> 24;
    value1 = (uptime & 0x00FF0000) >> 16;
    value2 = (uptime & 0x0000FF00) >> 8;
    value3 = (uptime & 0x000000FF);

    uint8_t toSend[7] = {START_BYTE, UPTIME, value0, value1, value2, value3, END_BYTE};
    #ifdef BLUETOOTH_SERIAL
        SerialBT.write(toSend, 7);
    #else
        Serial.write(toSend, 7);
    #endif
}

void Controller::pressureControl()
{
    // Pressure control: if the current pressure is above a certain threshold,
    // we turn the pump off. If it is below a certain threshold (all relative to
    // the setpoint), we turn the pump on.
    // Pressure controllers 1 and 2 are connected to pump 1 (positive pressure);


    // For now: just one PR connected to 1 pump.

    PressureController * pc1 = static_cast<PressureController*>(mComponents[PR1]);
    PressureController * pc2 = static_cast<PressureController*>(mComponents[PR2]);
    Pump * pump = static_cast<Pump*>(mComponents[PUMP1]);

    // If the supply pressure is not indicated as too low, and it has been that way
    // for at least 2 seconds, we switch the pump off.

    if (pc1->setPointValue() == 0 && pc2->setPointValue() == 0) {
        pump->setValue(OFF);
    }

    else if (pump->getValue() == ON &&
        !pc1->isInputPressureTooLow() &&
        !pc2->isInputPressureTooLow() &&
        millis() - mPumpLastSwitchOnTime > 5000)
    {
            pump->setValue(OFF);
    }
    else if (pump->getValue() == OFF &&
             (pc1->isInputPressureTooLow() || pc2-> isInputPressureTooLow()))
    {
        pump->setValue(ON);
        mPumpLastSwitchOnTime = millis();
    }
}
