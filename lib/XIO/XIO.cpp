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

#include <stdlib.h>
#include <string.h>
#include "Arduino.h"
#include "Wire.h"
#include "XIO.h"

#ifndef _BV
#define _BV(a) (1<<(a))
#endif

XIO::XIO()
{
	this->addr = 0;
	this->dioReset = -1; // Unknown digital I/O for the reset line
	this->dioOutputEnable = -1; // Unknown digital I/O for the reset line
}

void XIO::begin(boolean a0, boolean a1, boolean a2, char dioResetPin, char dioOutputEnablePin)
{
	uint8_t addrBitmap = (a0?0x01:0x00) | (a1?0x02:0x00) | (a2?0x04:0x00);
	this->addr = 0x20 | (addrBitmap);
	
	this->dioReset = dioResetPin;
	this->dioOutputEnable = dioOutputEnablePin;
  		
	// If there's a DIO pin assigned to reset, use it to do a hardware reset on initialization
	if (-1 != this->dioOutputEnable)
		pinMode(this->dioOutputEnable, OUTPUT);

	if (-1 != this->dioReset)
		pinMode(this->dioReset, OUTPUT);

	if (-1 != this->dioReset)
	{
		digitalWrite(this->dioReset, LOW);
		delayMicroseconds(100);
		digitalWrite(this->dioReset, HIGH);
	}
	
	if (-1 != this->dioOutputEnable)
		digitalWrite(this->dioOutputEnable, LOW);
		
	memset(this->pinDirections, 0xFF, sizeof(this->pinDirections));
	memset(this->pinOutputStates, 0x00, sizeof(this->pinOutputStates));
	memset(this->pinOutputStates, 0x00, sizeof(this->pinInputStates));
}

void XIO::xioPinModeCached(byte pin, byte mode)
{
	uint8_t bit = pin & 0x0F;
	uint8_t bank = (pin>>4) & 0x0F;
	// If things are out of range, do nothing		
	if ((bit > 7) || (bank > 4))
		return;

	switch(mode)
	{
		case OUTPUT:
			pinDirections[bank] &= ~(_BV(bit));
			break;

		case INPUT:
			pinDirections[bank] |= _BV(bit);
			break;
			
		default:
			return; // Don't understand this pin mode, so do nothing	
	}
}

void XIO::xioPinMode(byte pin, byte mode)
{
	uint8_t bit = pin & 0x0F;
	uint8_t bank = (pin>>4) & 0x0F;
	// If things are out of range, do nothing		
	if ((bit > 7) || (bank > 4))
		return;

	xioPinModeCached(pin, mode);
	
	sendOutputConfiguration(bank);
}

void XIO::xioDigitalWriteCached(byte pin, boolean value)
{
	uint8_t bit = pin & 0x0F;
	uint8_t bank = (pin>>4) & 0x0F;
	// If things are out of range, do nothing		
	if ((bit > 7) || (bank > 4))
		return;

	if (value)
		pinOutputStates[bank] |= _BV(bit);
	else
		pinOutputStates[bank] &= ~_BV(bit);

}

void XIO::xioDigitalWrite(byte pin, boolean value)
{
	uint8_t bit = pin & 0x0F;
	uint8_t bank = (pin>>4) & 0x0F;
	// If things are out of range, do nothing		
	if ((bit > 7) || (bank > 4))
		return;

	xioDigitalWriteCached(pin, value);
	
	sendOutput(bank);
}

boolean XIO::xioDigitalReadCached(byte pin)
{
	uint8_t bit = pin & 0x0F;
	uint8_t bank = (pin>>4) & 0x0F;
	// If things are out of range, do nothing		
	if ((bit > 7) || (bank > 4))
		return false;

	return( (this->pinInputStates[bank] & _BV(bit)) ? HIGH:LOW );
}

boolean XIO::xioDigitalRead(byte pin)
{
	uint8_t bit = pin & 0x0F;
	uint8_t bank = (pin>>4) & 0x0F;
	// If things are out of range, do nothing		
	if ((bit > 7) || (bank > 4))
		return false;

	this->pinInputStates[bank] = getInput(bank);

	return xioDigitalReadCached(pin);
}



void XIO::sendOutputConfiguration(byte bank)
{
	uint8_t numWrites = 1, i;
	if (0xFF == bank)
	{
		bank = 0;
		numWrites = 5;
	}
	
   Wire.beginTransmission(this->addr);
   Wire.write((uint8_t)(0x80 | (0x18 + bank))); // 0x18 is the direction register base, 0x80 is auto-increment
	for(i=0; i<numWrites; i++)
		Wire.write(this->pinDirections[bank+i]);
   Wire.endTransmission();
}

void XIO::sendOutput(byte bank)
{
	uint8_t numWrites = 1, i;
	if (0xFF == bank)
	{
		bank = 0;
		numWrites = 5;
	}
	
   Wire.beginTransmission(this->addr);
   Wire.write(0x80 | (0x08 + bank)); // 0x08 is the output register base, 0x80 is auto-increment
	for(i=0; i<numWrites; i++)
		Wire.write(this->pinOutputStates[bank+i]);
   Wire.endTransmission();
}

byte XIO::getInput(byte bank)
{
	uint8_t numReads = 1, i;
	if (0xFF == bank)
	{
		bank = 0;
		numReads = 5;
	}

   Wire.beginTransmission(this->addr);
   Wire.write(0x80 | (0x00 + bank)); // 0x00 is the input register base, 0x80 is auto-increment
	i = Wire.endTransmission(false);

	if (0 != i)
	{
		Wire.endTransmission(true); // Make sure we send a stop bit
		return 0; // I2C error, don't continue
	}

	Wire.requestFrom(this->addr, numReads);
	if (Wire.available() < numReads)
	{
		Wire.endTransmission(true); // Make sure we send a stop bit
		return 0; // I2C error, don't continue
	}

	for(i=0; i<numReads; i++)
		this->pinInputStates[bank+i] = Wire.read();

	return(this->pinInputStates[bank]);

}

void XIO::refreshPinModes()
{
	sendOutputConfiguration(0xFF);
}

void XIO::refreshIO()
{
	sendOutput(0xFF);
	getInput(0xFF);
}



