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

    for (int i(0); i < mValves.size(); ++i) {
        RgbColor color = mValves[i]->getValue() ? green : red;
        mStrip.SetPixelColor(i, color);
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

    // 32 valves, including 8 that are normally-closed (17 through 24)
    mValves.push_back(new Valve(VALVE1_PIN));
    mValves.push_back(new Valve(VALVE2_PIN));
    mValves.push_back(new Valve(VALVE3_PIN));
    mValves.push_back(new Valve(VALVE4_PIN));
    mValves.push_back(new Valve(VALVE5_PIN));
    mValves.push_back(new Valve(VALVE6_PIN));
    mValves.push_back(new Valve(VALVE7_PIN));
    mValves.push_back(new Valve(VALVE8_PIN));
    mValves.push_back(new Valve(VALVE9_PIN));
    mValves.push_back(new Valve(VALVE10_PIN));
    mValves.push_back(new Valve(VALVE11_PIN));
    mValves.push_back(new Valve(VALVE12_PIN));
    mValves.push_back(new Valve(VALVE13_PIN));
    mValves.push_back(new Valve(VALVE14_PIN));
    mValves.push_back(new Valve(VALVE15_PIN));
    mValves.push_back(new Valve(VALVE16_PIN));
    mValves.push_back(new Valve(VALVE17_PIN, false));
    mValves.push_back(new Valve(VALVE18_PIN, false));
    mValves.push_back(new Valve(VALVE19_PIN, false));
    mValves.push_back(new Valve(VALVE20_PIN, false));
    mValves.push_back(new Valve(VALVE21_PIN, false));
    mValves.push_back(new Valve(VALVE22_PIN, false));
    mValves.push_back(new Valve(VALVE23_PIN, false));
    mValves.push_back(new Valve(VALVE24_PIN, false));
    mValves.push_back(new Valve(VALVE25_PIN));
    mValves.push_back(new Valve(VALVE26_PIN));
    mValves.push_back(new Valve(VALVE27_PIN));
    mValves.push_back(new Valve(VALVE28_PIN));
    mValves.push_back(new Valve(VALVE29_PIN));
    mValves.push_back(new Valve(VALVE30_PIN));
    mValves.push_back(new Valve(VALVE31_PIN));
    mValves.push_back(new Valve(VALVE32_PIN));

    // 2 pumps
    mPumps.push_back(new Pump(PUMP1_PIN));
    mPumps.push_back(new Pump(PUMP2_PIN));

    // 2 digital pressure controllers. I2C addresses are 43 and 44
    mPCs.push_back(new PressureController(43));
    mPCs.push_back(new PressureController(44));

    // Set all pin modes, and default values (LOW; see Valve::Valve())
    mXioBoard.refreshPinModes();
    mXioBoard.refreshIO();

    mPressureUpdateTimer = millis();
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

    if ((millis() - mPressureUpdateTimer) > 500) {
        sendComponentValue(PRESSURE, 1);
        sendComponentValue(PRESSURE, 2);

        mPressureUpdateTimer = millis();
    }

    if ((millis() - mPressureControlTimer) > 500) {
        pressureControl();
        mPressureControlTimer = millis();
    }

    // Read data from serial port
#ifdef BLUETOOTH_SERIAL
    while (SerialBT.available()) 
        mBuffer.push_back(SerialBT.read());
#else
    while (Serial.available()) 
        mBuffer.push_back(Serial.read());
#endif

    // Decode and handle the commands 
    while (mBuffer.size() > 0) {
        std::deque<uint8_t> b = decodeBuffer();
        if (b.size() > 0) 
            parseDecodedBuffer(b);
    }

    if (mXIORefreshRequested) {
        mXioBoard.refreshIO();
        mXIORefreshRequested = false;
    }
}

/**
  * @brief Replacement of pinMode() for components connected to the PCA9896
  */
void Controller::xioPinMode(int pin, int mode)
{
    mXioBoard.xioPinModeCached(pin, mode);
}

/**
  * @brief Replacement of digitalWrite() for components connected to the PCA9896
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
 * @brief Go through mBuffer and find the first complete message in it
 * @return The first complete message found, minus start, stop and escape sequences, or an empty deque
 * 
 * Like its counterpart in ufcs-pc, this goes through mBuffer until it finds a start
 * byte; it appends everything following it (except for any escape bytes) until it reaches
 * the stop byte. It then returns the message found.
 
 * If no message is found, an empty deque is returned.
 * 
 * If a start byte is found but no stop byte, the partial message and state of the decoder is
 * kept; it will continue adding to that message the next time it is called. 
 */
std::deque<uint8_t> Controller::decodeBuffer()
{
    bool foundCompleteMessage(false);
    int decoderIndex(0);

    for (uint8_t c: mBuffer) {
        if (mDecoderRecording) {
            if (mDecoderEscaped) {
                mDecodedBuffer.push_back(c);
                mDecoderEscaped = false;
            }

            else if (c == ESCAPE_BYTE)
                mDecoderEscaped = true;

            else if (c == STOP_BYTE) {
                foundCompleteMessage = true;
                mDecoderRecording = false;
            }

            else
                mDecodedBuffer.push_back(c);
        }

        else if (c == START_BYTE) {
            mDecoderRecording = true;
        }

        decoderIndex++;
    }

    // Everything that was parsed already should be removed from mBuffer
    mBuffer.erase(mBuffer.begin(), mBuffer.begin()+decoderIndex);

    if (foundCompleteMessage) {
        std::deque<uint8_t> decodedBuffer = mDecodedBuffer;
        mDecodedBuffer.clear();
        return decodedBuffer;
    }

    return std::deque<uint8_t>();
}

/**
 * @brief Parse a decoded buffer and execute the command it contains
 * @param buffer A buffer containing a message (as returned by decodeBuffer) 
 */
void Controller::parseDecodedBuffer(std::deque<uint8_t> const& buffer)
{
    if (buffer.size() == 0) 
        return;

    uint8_t command = buffer[0];

    std::deque<std::deque<uint8_t> > parameters;

    int i(1);
    while (i < buffer.size()) {
        uint8_t paramSize = buffer[i];
        i++;
        if (i + paramSize <= buffer.size()) {
            parameters.push_back(std::deque<uint8_t>());
            for (int k(i); k < i+paramSize; ++k)
                parameters.back().push_back(buffer[k]);
        }
        i += paramSize;
    }

    int nParams = parameters.size();

    switch (command) {
        case VALVE:
            if (nParams != 2 || parameters[0].size() != 1 || parameters[1].size() != 1)
                break;
            setValve(parameters[0][0], parameters[1][0]);
            break;
            
        case PUMP:
            if (nParams != 2 || parameters[0].size() != 1 || parameters[1].size() != 1)
                break;
            setPump(parameters[0][0], parameters[1][0]);
            break;

        case PRESSURE:
            if (nParams != 2 || parameters[0].size() != 1 || parameters[1].size() != 1)
                break;
            setPressure(parameters[0][0], parameters[1][0]);
            break;
            
        case STATUS_REQUEST:
            if (nParams == 0)
                sendAllComponentValues();
            else if (nParams == 2 && parameters[0].size() == 1 && parameters[1].size() == 1)
                sendComponentValue((Command)parameters[0][0], parameters[1][0]); 
            break;

        default:
            Log.error("Unknown command received: %d", command);
            break;
    }

}

/**
 * @brief Send the value of the given component to the host
 * @param componentType VALVE, PUMP, PRESSURE,...
 * @param number The component number, 1-indexed
 */
void Controller::sendComponentValue(Command componentType, uint8_t number)
{
    std::vector<uint8_t> message;
    message.push_back(componentType);

    switch (componentType) {
        case VALVE: 
            if (number <= mValves.size()) {
                message.push_back(1);
                message.push_back(number);
                message.push_back(1);
                message.push_back(mValves[number-1]->getValue());
            }
            break;

        case PUMP:
            if (number <= mPumps.size()) {
                message.push_back(1);
                message.push_back(number);
                message.push_back(1);
                message.push_back(mPumps[number-1]->getValue());
            }
            break;

        case PRESSURE:
            if (number <= mPCs.size()) {
                message.push_back(1);
                message.push_back(number);
                message.push_back(1);
                message.push_back(mPCs[number-1]->setPointValue());
                message.push_back(1);
                message.push_back(mPCs[number-1]->getValue());
            }
            break;

        default:
            break;
    }

    frameAndSendMessage(message);
}

/**
 * @brief Send the value of all components to the host
 */

void Controller::sendAllComponentValues()
{
    for (int i(1); i <= mValves.size(); ++i) 
        sendComponentValue(VALVE, i);

    for (int i(1); i <= mPumps.size(); ++i) 
        sendComponentValue(PUMP, i);

    for (int i(1); i <= mPCs.size(); ++i) 
        sendComponentValue(PRESSURE, i);
}

/**
  * @brief Send the current uptime (in seconds) to the host
  */
void Controller::sendUptime()
{
    unsigned long uptime = millis() / 1000;
    uint8_t value0, value1, value2, value3;

    value0 = (uptime & 0xFF000000) >> 24;
    value1 = (uptime & 0x00FF0000) >> 16;
    value2 = (uptime & 0x0000FF00) >> 8;
    value3 = (uptime & 0x000000FF);

    std::vector<uint8_t> message {value0, value1, value2, value3};
    frameAndSendMessage(message);
}

/**
 * @brief Send a message to the host, after framing it
 * @param message The valid, raw message to send
 * 
 * Messages start with START_BYTE, end with STOP_BYTE, and any special character
 * (STOP_BYTE or ESCAPE_BYTE) contained in the message is escaped with ESCAPE_BYTE
 */
void Controller::frameAndSendMessage(std::vector<uint8_t> const& message)
{
    std::vector<uint8_t> framed;
    framed.push_back(START_BYTE);

    for (auto const& c: message) {
        if (c == STOP_BYTE || c == ESCAPE_BYTE)
            framed.push_back(ESCAPE_BYTE);
        framed.push_back(c);
    }

    framed.push_back(STOP_BYTE);

    uint8_t * data = framed.data();

#ifdef BLUETOOTH_SERIAL
    SerialBT.write(data, framed.sized());
#else
    Serial.write(data, framed.size());
#endif
}

/**
 * @brief Set a valve to a given state (open or closed)
 * @param number The valve number, 1-indexed
 * @param open True if the valve should be opened; false if closed
 */
void Controller::setValve(uint8_t number, bool open)
{
    if (number <= mValves.size())  {
        mValves[number-1]->setValue(open);
        sendComponentValue(VALVE, number);

#ifdef NEOPIXELS
        setNeoPixel(number-1, (open ? green : red));
#endif
    }
}

/**
 * @brief Set a pump to a given state (on or off)
 * @param number The valve number, 1-indexed
 * @param on True if the pump should be turned on; false if off
 */
void Controller::setPump(uint8_t number, bool on)
{
    if (number <= mPumps.size()) {
        mPumps[number-1]->setValue(on);
        sendComponentValue(PUMP, number);
    }
}

/**
 * @brief Set the setpoint of a given pressure controller
 * @param number The pressure controller number, 1-indexed
 * @param setpoint The setpoint, between 0 and 255
 */
void Controller::setPressure(uint8_t number, uint8_t setpoint)
{
    if (number <= mPCs.size()) {
        mPCs[number-1]->setValue(setpoint);
        sendComponentValue(PRESSURE, number);
    }
}

/**
  * @brief Turn pump(s) on and off automatically
  *
  * If the current pressure is above a certain threshold, the pump is turned off.
  * This is based on the digital pressure regulators' feedback. They signal
  * whether input pressure is sufficient or not. If it is signaled as being too
  * low, then the pump is switched on for at least 3 seconds and until the
  * regulator stops signaling that pressure is too low.
  */
void Controller::pressureControl()
{
    // In this configuration, the first two pressure regulators are connected
    // to pump 1.

    PressureController * pc1 = mPCs[0];
    PressureController * pc2 = mPCs[1];
    Pump * pump = mPumps[0];

    // Switch the pump off if the setpoints are at 0
    if (pc1->setPointValue() == 0 && pc2->setPointValue() == 0) {
        pump->setValue(false);
    }

    // If the pressure is sufficient, and the pump has been on for at least
    // 3 seconds (to build up a little extra pressure), then turn it off.
    else if (pump->getValue() &&
             !pc1->isInputPressureTooLow() &&
             !pc2->isInputPressureTooLow() &&
             millis() - mPumpLastSwitchOnTime > 3000)
    {
        pump->setValue(false);
        sendComponentValue(PUMP, 0);
    }
    
    else if (pump->getValue() == false &&
             (pc1->isInputPressureTooLow() || pc2-> isInputPressureTooLow()))
    {
        pump->setValue(true);
        mPumpLastSwitchOnTime = millis();
        sendComponentValue(PUMP, 0);
    }
}