# Microfluidics control system // ESP32 code

Code for the microcontroller-side of the microfluidics control system. Please see the following page for more information: https://hackaday.io/project/27511-microfluidics-control-system

## Install

If you have PlatformIO already, all you need to do is import the project, plug in your ESP32 and hit Upload. If not, you can obtain it [here](https://platformio.org/platformio-ide).


## Customizing the code to your hardware setup

The hardware is modular, and different valves, pumps and pressure regulators can be used. The main things you may need to specify is which valves are normally-open and which are normally-closed, as well as what types of pressure regulators are connected and where.

The list of components is initialized in the `init()` method of the `Controller` class, in `src/controller.cpp`. This class has lists of valves, pressure controllers and pumps, which are defined similarly.
`Valve` objects are constructed with two arguments: a pin number and a `normallyOpen` parameter which specifies whether the valve is normally open or not. If for example your setup uses normally-closed valves for positions 1-8, simply change:

    mValves.push_back(new Valve(VALVE1_PIN))

to

    mValves.push_back(new Valve(VALVE1_PIN, false))

If you wish to customize the pin assignments of each component, it is best to re-define the constants defined in  `src/constants.h`. However, this should not be necessary in most cases.

Next, you may wish to customize the number, types and/or addresses of pressure regulators (aka pressure controllers). These are initialized lower down in the same `init()` method. If you have digital pressure regulators (as described in the HardwareX article), you only need to specify their i2c address:

    mPCs.push_back(new PressureController(43));

This address should be edited if need be, based on the address of each pressure regulator (which must be unique; see the code and schematics for the pressure regulators for more details on changing this).

If you have analog regulators (Parker OEM/VSO series for example), you will have to specify which pins they are connected to:

    mPCs.push_back(new PressureController(PR1_SETPOINT_PIN,
                                          PR1_MEASUREMENT_PIN,
                                          DAC_MAX_VALUE,
                                          ADC_MAX_VALUE));

This indicates that the pressure regulator is connected to the first port on the PCB (which uses pins `PR1_SETPOINT_PIN` and `PR1_MEASUREMENT_PIN` to communicate with the regulator). Change these two parameters to `PR2_SETPOINT_PIN` and `PR2_MEASUREMENT_PIN` if the regulator is connected to the second port on the PCB. Again, these constants are defined in `src/constants.h` and do not need to be edited.

For all the components defined in `init()`, the order defines what number each component is assigned. For example, when the user toggles valve 12 in the PC software, the 12th valve in the list will be toggled, regardless of what pin it is connected to.
