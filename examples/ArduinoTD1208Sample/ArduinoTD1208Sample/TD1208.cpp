/* TD1208.cpp
 *
 * Copyright (C) 2016 IoTHerd
 * Author : A. Limonet
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * TD1208 is library for Sigfox TD1208 use with Arduino platform. 
 * The library is designed to use SoftwareSerial library for serial communication between TD1208 module and Arduino.
 * Current library coverrage is:
 *	 - AT command 
 *	 - Sigfox payload transfer
 *	 - TD1208 temperature read
 *
 */
#include "TD1208.h"

#define ATOK "OK"
#define ATCOMMAND "AT"
#define ATSIGFOXTX "AT$SS="
#define ATTEMPERATURE "ATI26"

#define ATCOMMAND_TIMEOUT (3000)
#define ATSIGFOXTX_TIMEOUT (30000)

TD1208::TD1208(SoftwareSerial* serialPort)
{
	TD1208::serialPort = serialPort;
}

bool TD1208::sendAT()
{
	return sendATCommand(ATCOMMAND, ATCOMMAND_TIMEOUT, nullptr);
}

bool TD1208::sendPayload(const String payload)
{
	String message = (String) ATSIGFOXTX + payload;
	String data = "";
	if (sendATCommand(message, ATSIGFOXTX_TIMEOUT, &data))
	{
		Serial.println(data);
		return true;
	}
	else
	{
		return false;
	}
}

bool TD1208::getTemperature(int *temperature)
{
	String data = "";
	if (sendATCommand(ATTEMPERATURE, ATCOMMAND_TIMEOUT, &data))
	{
		*temperature = data.toInt();
		return true;
	}
	else
	{
		return false;
	}
}

bool TD1208::sendATCommand(const String command, const int timeout, String *dataOut)
{
	// Start serial interface
	serialPort->begin(9600);
	delay(200);	
	serialPort->flush();
	serialPort->listen();

	// Add CRLF to the command
	String ATCommand = "";
	ATCommand.concat(command);
	ATCommand.concat("\r\n");

	// Print echo
	Serial.println((String)">>" + ATCommand);

	// Send the command : need to write/read char by char because of echo
	for (int i = 0; i < ATCommand.length(); ++i)
	{
		serialPort->print(ATCommand.c_str()[i]);
		serialPort->read();
	}
	Serial.print("<<");

	// Read response 
	String response = "";
	
	unsigned int startTime = millis();
	volatile unsigned int currentTime = millis();
	volatile char rxChar = '\0';

	// Manage rx : two ways for loop break:
	// - Timeout
	// - Receive OK
	do
	{
		if (serialPort->available() > 0)
		{
			rxChar = (char)serialPort->read();
			response.concat(rxChar);
		}

		currentTime = millis();
	}while(((currentTime - startTime) < timeout) && response.endsWith(ATOK) == false);

	serialPort->end();
	Serial.println(response);

	// Split the response
	int index = 0;
	String firstData = "";
	String secondData = "";

	if (response.length() != 0)
	{
		// Split CRLF
		do
		{
			// Save previous index
			int previous = index;

			// Get next index
			index = response.indexOf("\r\n", index);

			// Check that index change
			if (previous != index)
			{
				// Get the chunk
				String chunk = response.substring(previous+1, index);
				if (chunk.length() > 0)
				{
					// Remove \r\n 
					chunk.replace("\r\n", "");
					
					if (firstData != "")
					{
						secondData = chunk;
					}
					else if (firstData == "" && secondData == "")
					{
						firstData = chunk;
					}
					else
					{
						Serial.println("ERROR on rx frame");
						return false;
					}
				}
			}

			// Increment index
			if (index >= 0)
			{
				index++;
			}
		
		} while (index < response.length() && index >= 0);
	}
	else
	{
		return false;
	}

	//Serial.println((String) "firstData : " + firstData);
	//Serial.println((String) "secondData : " + secondData);

	// Check if we have data on the first string and OK on second = data + OK
	if (firstData != "" && secondData == ATOK)
	{
		*dataOut = firstData;
		return true;
	}
	// Check if we have only an OK
	else if (firstData == ATOK && secondData == "")
	{
		return true;
	}
	else
	{
		Serial.println("Wrong AT response");
		return false;
	}
}