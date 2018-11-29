# Microfluidics control system // ESP32 code

Code for the microcontroller-side of the microfluidics control system. Please see the following page for more information: https://hackaday.io/project/27511-microfluidics-control-system

## Install

If you have PlatformIO already, all you need to do is import the project, plug in your ESP32 and hit Upload. If not, follow these instructions to set up everything.


## Customizing the code to your hardware setup

The hardware is modular, and different valves, pumps and pressure regulators can be used. The main things you may need to specify is which valves are normally-open and which are normally-closed, as well as what types of pressure regulators are connected and where.

The list of components is initialized in the `init()` method of the `Controller` class, located at `src/controller.cpp:51`. This class has a list of components (implemented as a `Map`), where each component can be either a `Valve`, `PressureController`, or `Pump`. `Valve` objects are constructed with two arguments: a pin number and a `normallyOpen` parameter which specifies whether the valve is normally open or not. If for example your setup uses normally-closed valves for positions 1-8, simply change:

    mComponents[VALVE1] = new Valve(VALVE1_PIN);

to

    mComponents[VALVE1] = new Valve(VALVE1_PIN, false);

If you wish to customize the pin assignments of each component, it is best to re-define the constants defined in  `src/constants.h`. However, this should not be necessary in most cases.

Next, you may wish to customize the number, types and/or addresses of pressure regulators (aka pressure controllers). These are defined lower down in the `Controller` constructor. Examples are provided for both i2c and analog pressure regulators. If you have analog regulators (Parker OEM/VSO series for example), you will have to specify which pins they are connected to:

    mComponents[PR1] = new PressureController(PR1_SETPOINT_PIN,
                                              PR1_MEASUREMENT_PIN,
                                              DAC_MAX_VALUE,
                                              ADC_MAX_VALUE);

This indicates that pressure regulator 1 (`PR1`) is connected to the first port on the PCB (which uses pins `PR1_SETPOINT_PIN` and `PR1_MEASUREMENT_PIN` to communicate with the regulator). Change these two parameters to `PR2_SETPOINT_PIN` and `PR2_MEASUREMENT_PIN` if the regulator is connected to the second port on the PCB. Again, these constants are defined in `src/constants.h` and do not need to be edited.

Digital pressure regulators communicate via i2c, and so the address of the regulators must be specified when initializing them. For example,

    mComponents[PR2] = new PressureController(PR2_I2C_ADDRESS);

indicates that the second pressure regulator is at address `PR2_I2C_ADDRESS` (defined as 44). This address should be edited if need be, based on the address of each pressure regulator (which must be unique; see the code and schematics for the pressure regulators for more details on changing this).
