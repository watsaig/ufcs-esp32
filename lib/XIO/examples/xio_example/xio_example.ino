/*************************************************************************
Title:    Iowa Scaled Engineering I2C-XIO (PCA9698) Arduino Example
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2014 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    For more information about the Iowa Scaled Engineering I2C-XIO, see:
    http://www.iascaled.com/store/I2C-XIO

*************************************************************************/

#include <XIO.h>
#include <Wire.h>

XIO xioBoard;

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Wire.begin();
  //  Both of the following initialization examples assume that address jumpers A0, A1, and A2
  //   on the I2C-XIO are set to high (open, no jumper).  If any are set low (jumpered), please
  //   replace the appropriate configuration value with "LOW" in the "begin()" call

  //  If you're using a standard Iowa Scaled shield to connect the I2C lines to the Arduino,
  //   the /IORST line is likely tied to Digital IO line 4 and /OEN is tied to Digital IO line 3.
  //   Use the initializer below.
  xioBoard.begin(HIGH, HIGH, HIGH, 4, 3);

  //  If the /IORST and /OEN lines on the I2C cable aren't connected anywhere, use this initializer
  //   instead of the one above.
  //  xioBoard.begin(HIGH, HIGH, HIGH, -1, -1);
}


void loop() 
{
  xioBoard.xioPinMode(XIO_PIN_A0, OUTPUT);
  xioBoard.xioPinMode(XIO_PIN_A1, OUTPUT);
  xioBoard.xioPinMode(XIO_PIN_A2, OUTPUT);  
  xioBoard.xioPinMode(XIO_PIN_A5, INPUT);
  xioBoard.xioPinMode(XIO_PIN_A6, INPUT);

  // Alternately, the pin configuration can be done much faster (because 
  //  of using a single I2C bus transaction) by doing the following:
  /*
  xioBoard.xioPinModeCached(XIO_PIN_A0, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A1, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A2, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_A5, INPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A6, INPUT);
  xioBoard.refreshPinModes();
  */
  
  do
  {
    // These commands blink the output on pin A0 - try it by attaching an LED through a resistor!
    xioBoard.xioDigitalWrite(XIO_PIN_A0, HIGH);
    delay(250);
    xioBoard.xioDigitalWrite(XIO_PIN_A0, LOW);
    delay(250);
    
    // Since A5 and A6 are configured as inputs, let's read them and output the states
    // to the serial console.  Try connecting A5 and A6 to +5V and GND to see them change!
    // Be careful! Never short an output directly to a power rail.  It's safer to even connect
    // inputs through a small resistance (500-1000 ohms) just in case you accidentally configure
    // something as an output.

    Serial.print("Input A5 is: ");
    Serial.print(xioBoard.xioDigitalRead(XIO_PIN_A5)?"HIGH":"LOW");
    Serial.print("\n");
    Serial.print("Input A6 is: ");
    Serial.print(xioBoard.xioDigitalRead(XIO_PIN_A6)?"HIGH":"LOW");
    Serial.print("\n\n");

    // Alternately, again, these reads could be done as a single transaction over the bus to speed
    // things along.  Here's how that code might look:
    /*
    xioBoard.refreshIO();
    Serial.print("Input A5 is: ");
    Serial.print(xioBoard.xioDigitalReadCached(XIO_PIN_A5)?"HIGH":"LOW");
    Serial.print("\n");
    Serial.print("Input A6 is: ");
    Serial.print(xioBoard.xioDigitalReadCached(XIO_PIN_A6)?"HIGH":"LOW");
    Serial.print("\n\n");
    */
  } while(1);
}
