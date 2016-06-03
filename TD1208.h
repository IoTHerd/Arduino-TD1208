/* TD1208.h
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
#ifndef TD1208_H
#define TD1208_H
#include <Arduino.h>
#include <SoftwareSerial.h>

class TD1208
{
public:
	TD1208(SoftwareSerial* serialPort);
	bool sendPayload(const String payload);
	bool sendAT();
	bool getTemperature(int *temperature);

private:
	bool sendATCommand(const String command, const int timeout, String *dataOut);
	SoftwareSerial* serialPort;
};

#endif // TD1208_H
