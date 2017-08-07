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
  // Alternately, the pin configuration can be done much faster (because 
  //  of using a single I2C bus transaction) by doing the following:
  xioBoard.xioPinModeCached(XIO_PIN_A0, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A1, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A2, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_A3, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A4, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A5, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_A6, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_A7, OUTPUT);  

  xioBoard.xioPinModeCached(XIO_PIN_B0, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_B1, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_B2, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_B3, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_B4, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_B5, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_B6, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_B7, OUTPUT);

  xioBoard.xioPinModeCached(XIO_PIN_C0, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_C1, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_C2, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_C3, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_C4, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_C5, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_C6, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_C7, OUTPUT);

  xioBoard.xioPinModeCached(XIO_PIN_D0, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_D1, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_D2, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_D3, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_D4, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_D5, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_D6, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_D7, OUTPUT);

  xioBoard.xioPinModeCached(XIO_PIN_E0, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_E1, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_E2, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_E3, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_E4, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_E5, OUTPUT);  
  xioBoard.xioPinModeCached(XIO_PIN_E6, OUTPUT);
  xioBoard.xioPinModeCached(XIO_PIN_E7, OUTPUT);


  xioBoard.refreshPinModes();
  
  do
  {
    // These commands blink the output on pin A0 - try it by attaching an LED through a resistor!
    xioBoard.xioDigitalWriteCached(XIO_PIN_A0, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A1, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A2, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_A3, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A4, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A5, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_A6, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A7, HIGH);  
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_B0, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B1, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B2, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_B3, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B4, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B5, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_B6, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B7, HIGH);
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_C0, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C1, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C2, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_C3, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C4, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C5, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_C6, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C7, HIGH);
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_D0, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D1, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D2, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_D3, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D4, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D5, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_D6, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D7, HIGH);
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_E0, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E1, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E2, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_E3, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E4, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E5, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_E6, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E7, HIGH);
    xioBoard.refreshIO();
    Serial.print("Cycle A\n");
    delay(1000);
    
    xioBoard.xioDigitalWriteCached(XIO_PIN_A0, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A1, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A2, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_A3, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A4, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A5, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_A6, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_A7, LOW);  
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_B0, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B1, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B2, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_B3, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B4, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B5, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_B6, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_B7, LOW);
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_C0, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C1, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C2, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_C3, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C4, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C5, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_C6, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_C7, LOW);
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_D0, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D1, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D2, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_D3, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D4, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D5, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_D6, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_D7, LOW);
  
    xioBoard.xioDigitalWriteCached(XIO_PIN_E0, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E1, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E2, HIGH);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_E3, LOW);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E4, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E5, LOW);  
    xioBoard.xioDigitalWriteCached(XIO_PIN_E6, HIGH);
    xioBoard.xioDigitalWriteCached(XIO_PIN_E7, LOW);
    xioBoard.refreshIO();
    Serial.print("Cycle B\n");
    delay(1000);
  } while(1);
}
