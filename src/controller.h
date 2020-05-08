#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <list>
#include <deque>
#include <Wire.h>
#include <ArduinoLog.h>

#include "constants.h"
#include "things.h"

#ifdef NEOPIXELS
    #include <NeoPixelBus.h>
#endif


/**
  * The controller class handles all high-level functionality as well as
  * serial communication.
  * Its constructor can be seen as the equivalent of the Arduino setup() function,
  * and its update() function as the equivalent of the Arduino loop() function.
  *
  * For simplicity and ease of maintenance, the various components (valves, pumps and
  * pressure regulators) are represented by different classes that all inherit from
  * the Thing class. This way, the Controller simply has a list of pointers to Things,
  * and can call setValue and getValue to set and get the current state of each component.
  * This is implemented as a Map, to easily associate a ComponentID (which is communicated
  * via the serial link) to each instance.
  */
class Controller
{

public:
    Controller ();
    void init();
    virtual ~Controller ();

    void update();

    void xioPinMode(int pin, int mode);
    void xioDigitalWrite(int pin, int value);

private:
    // Serial communication 
    std::deque<uint8_t> decodeBuffer();
    void parseDecodedBuffer(std::deque<uint8_t> const& buffer);

    void sendComponentValue(Command componentType, uint8_t number);
    void sendAllComponentValues();
    void sendUptime();
    void frameAndSendMessage(std::vector<uint8_t> const& message);

    std::deque<uint8_t> mBuffer;
    std::deque<uint8_t> mDecodedBuffer;
    bool mDecoderRecording;
    bool mDecoderEscaped;

    // Hardware components 
    void setValve(uint8_t number, bool open);
    void setPump(uint8_t number, bool on);
    void setPressure(uint8_t number, uint8_t setpoint);
    void pressureControl();

    XIO mXioBoard;
    bool mXIORefreshRequested;

    std::deque<Valve*> mValves;
    std::deque<Pump*> mPumps;
    std::deque<PressureController*> mPCs;

    // Timers
    unsigned long mPressureUpdateTimer;
    unsigned long mPressureControlTimer;
    unsigned long mPumpLastSwitchOnTime;


#ifdef NEOPIXELS
public:
    void initNeoPixelStrip();
    void setNeoPixel(int index, const RgbColor& color);

#endif // def NEOPIXELS
};

#endif
