/* TD1208.h
*
* Copyright (C) 2016 IoTHerd
* Author : A. Limonet
*
* This software may be modified and distributed under the terms
* of the MIT license.  See the LICENSE file for details.
*
* This sample code is demo project for Arduino TD1208 Library:
*	- Check if TD1208 is available on setup 
*	- Read TD1208 temperature
*	- Send temperature on sigfox
*/
#include <SoftwareSerial.h>
#include "TD1208.h"

// TD1208 Sigfox module IO definition
#define SIGFOXMOD_TX (2)
#define SIGFOXMOD_RX (3)

// Sigfox module serial PIN configuration
SoftwareSerial SigFoxSerial(SIGFOXMOD_RX, SIGFOXMOD_TX); // RX, TX

// TD1208 management instance
TD1208 td1208(&SigFoxSerial);

void setup()
{
	// Init debug port
	Serial.begin(9600);

	// Check TD1208 communication
	if (td1208.sendAT())
	{
		Serial.println("TD1208 OK");
	}
	else
	{
		// Infinite loop
		Serial.println("TD1208 KO");
		while (1);
	}
}

void loop()
{
	// Read module temperature
	int temperature = 0;
	if (td1208.getTemperature(&temperature))
	{
		// Send temperature on Sigfox module
		if (td1208.sendPayload((String)temperature))
		{
			Serial.println("Send OK");
		}
		else
		{
			Serial.println("Send KO");
		}
	}
	else
	{
		Serial.println("Temperature KO");
	}

	// Infinite loop
	while (1);
}
