#ifndef CONSTANTS_H
#define CONSTANTS_H

// Uncomment following line to use Bluetooth serial interface instead of USB
//#define BLUETOOTH_SERIAL

#include <XIO.h>

// Testing stuff
// ---------------------------------------

// if the following line is uncommented, the neopixel-related code in controller.[h/cpp] will be compiled 
// (switching LEDs on/off when valves are toggled)
//#define NEOPIXELS
#define NEOPIXELS_DATA_PIN 14


// I/O pins
// ---------------------------------------
#define XIO_RESET_PIN 13
#define XIO_OE_PIN 12

#define PUMP1_PIN 32
#define PUMP2_PIN 33

#define DAC0 25
#define DAC1 26

#define PR1_SETPOINT_PIN DAC0
#define PR2_SETPOINT_PIN DAC1
#define PR1_MEASUREMENT_PIN 36
#define PR2_MEASUREMENT_PIN 39

#define DAC_MAX_VALUE 255
#define ADC_MAX_VALUE 4095
#define PWM_MAX_VALUE 255 // depends on resolution (8 bit => max value of 255)
#define PWM_RESOLUTION 8
#define PWM_FREQ 12000

#define VALVE1_PIN XIO_PIN_A7
#define VALVE2_PIN XIO_PIN_A6
#define VALVE3_PIN XIO_PIN_A5
#define VALVE4_PIN XIO_PIN_A4
#define VALVE5_PIN XIO_PIN_A3
#define VALVE6_PIN XIO_PIN_A2
#define VALVE7_PIN XIO_PIN_A1
#define VALVE8_PIN XIO_PIN_A0

#define VALVE9_PIN XIO_PIN_B7
#define VALVE10_PIN XIO_PIN_B6
#define VALVE11_PIN XIO_PIN_B5
#define VALVE12_PIN XIO_PIN_B4
#define VALVE13_PIN XIO_PIN_B3
#define VALVE14_PIN XIO_PIN_B2
#define VALVE15_PIN XIO_PIN_B1
#define VALVE16_PIN XIO_PIN_B0

#define VALVE17_PIN XIO_PIN_C7
#define VALVE18_PIN XIO_PIN_C6
#define VALVE19_PIN XIO_PIN_C5
#define VALVE20_PIN XIO_PIN_C4
#define VALVE21_PIN XIO_PIN_C3
#define VALVE22_PIN XIO_PIN_C2
#define VALVE23_PIN XIO_PIN_C1
#define VALVE24_PIN XIO_PIN_C0

#define VALVE25_PIN XIO_PIN_D7
#define VALVE26_PIN XIO_PIN_D6
#define VALVE27_PIN XIO_PIN_D5
#define VALVE28_PIN XIO_PIN_D4
#define VALVE29_PIN XIO_PIN_D3
#define VALVE30_PIN XIO_PIN_D2
#define VALVE31_PIN XIO_PIN_D1
#define VALVE32_PIN XIO_PIN_D0

// Pressure control loop constants
// -------------------------------
#define PRESSURE_LOW_THRESHOLD_RATIO 0.8
#define PRESSURE_HIGH_THRESHOLD_RATIO 1.0

// Constants for serial communication -- don't edit (or also edit them on the PC side)
// ---------------------------------------
enum Command : uint8_t {
    VALVE,
    PRESSURE,
    PUMP,
    STATUS_REQUEST,
    UPTIME,
    ERROR,
    LOG,
    NUM_COMMANDS
};

enum LogLevel : uint8_t {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

const uint8_t START_BYTE = 250;
const uint8_t STOP_BYTE = 251;
const uint8_t ESCAPE_BYTE = 252;


#endif
