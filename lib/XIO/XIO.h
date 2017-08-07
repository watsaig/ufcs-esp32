/*************************************************************************
Title:    Iowa Scaled Engineering I2C-XIO (PCA9698) Driver Library
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

#ifndef XIO_H
#define XIO_H

#include "Arduino.h"


#define XIO_PIN_A0  0x00
#define XIO_PIN_A1  0x01
#define XIO_PIN_A2  0x02
#define XIO_PIN_A3  0x03
#define XIO_PIN_A4  0x04
#define XIO_PIN_A5  0x05
#define XIO_PIN_A6  0x06
#define XIO_PIN_A7  0x07

#define XIO_PIN_B0  0x10
#define XIO_PIN_B1  0x11
#define XIO_PIN_B2  0x12
#define XIO_PIN_B3  0x13
#define XIO_PIN_B4  0x14
#define XIO_PIN_B5  0x15
#define XIO_PIN_B6  0x16
#define XIO_PIN_B7  0x17

#define XIO_PIN_C0  0x20
#define XIO_PIN_C1  0x21
#define XIO_PIN_C2  0x22
#define XIO_PIN_C3  0x23
#define XIO_PIN_C4  0x24
#define XIO_PIN_C5  0x25
#define XIO_PIN_C6  0x26
#define XIO_PIN_C7  0x27

#define XIO_PIN_D0  0x30
#define XIO_PIN_D1  0x31
#define XIO_PIN_D2  0x32
#define XIO_PIN_D3  0x33
#define XIO_PIN_D4  0x34
#define XIO_PIN_D5  0x35
#define XIO_PIN_D6  0x36
#define XIO_PIN_D7  0x37

#define XIO_PIN_E0  0x40
#define XIO_PIN_E1  0x41
#define XIO_PIN_E2  0x42
#define XIO_PIN_E3  0x43
#define XIO_PIN_E4  0x44
#define XIO_PIN_E5  0x45
#define XIO_PIN_E6  0x46
#define XIO_PIN_E7  0x47


class XIO
{
	public:
		XIO();
		void begin(boolean a0, boolean a1, boolean a2, char dioReset, char dioOutputEnable);
		void xioPinMode(byte pin, byte mode);
		void xioPinModeCached(byte pin, byte mode);
		boolean xioDigitalRead(byte pin);
		boolean xioDigitalReadCached(byte pin);
		void xioDigitalWrite(byte pin, boolean value);
		void xioDigitalWriteCached(byte pin, boolean value);		
		void refreshIO();
		void refreshPinModes();		

	private:
		uint8_t addr;
		uint8_t pinDirections[5];
		uint8_t pinOutputStates[5];
		uint8_t pinInputStates[5];		
		int8_t dioReset;
		int8_t dioOutputEnable;

		void sendOutputConfiguration(byte bank);
		byte getInput(byte bank);
		void sendOutput(byte bank);
};

#endif

