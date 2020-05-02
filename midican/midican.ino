/*

	NETWORK - using nodejs to send signals across the interwebs via UDP to affect participants MIDI environment
	creating buffer: https://www.arduino.cc/en/Tutorial/UdpNTPClient

	MIDI - using MIDIUSB.h to send MIDI cmds to DAW

	CALIBRATION - iterate through an array of sensors and calibrate each sensor.  Storing values in an array of mins/maxes.
	• Can calibration will run first - canCalibrationTime  eg first 10 seconds
	• Set calibration time per sensor in calibrationTime var, used in runSensorCalibration() fn
	• During calibration tap and release sensor to set min & max values
	• Uses RGB LED to denote different calibration modes:
	Starts in yellow. blue calibrating, toggle red switching to next sensor back to blue. solid red when calibration complete
	Short green when completing setup()
	Yellow to start loop()
	• Assigns min sensorMin[] in sensorMinMIN & max sensorMax[] in sensorMaxMAX
	use these vars for various thresholds
*/

#include "udp_server.h"
#include "cap_sen.h"
#include "tcp_server.h"

void setup()
{
	// init serial
	Serial.begin(9600);
	while (!Serial);

	Serial.println("Serial started");

	//start with can Calibration
	canCalibration();

	pinMode(buttonPin, INPUT_PULLUP);

	// RGB LED pinModes
	pinMode(red, OUTPUT);
	pinMode(green, OUTPUT);
	pinMode(blue, OUTPUT);

	// Starts LED Yellow
	analogWrite(green, 255);
	digitalWrite(red, LOW);
	digitalWrite(blue, HIGH);

	// take initial sensor readings - more consistent calibration readings
	readSensors();

	// small delay before sensor calibration starts
	delay(2500);

	initCapacitive();
	initWifi();
	initUdp();

	Serial.print("canCapMax = ");
	Serial.println(canCapMax);
}

void loop()
{
	capCalibration_Debug();
	handleCapBtns();
	handleCapSlides();
	handleReceiveUdp();

	if (isPot < 1)
	{
		readEnc(0);
	}
	else
	{
		readEnc(isPot);
	}

	// handle button press types
	b = checkButton();

	encoderButton(b);

	// set baseNote from Pot/encoder
	if (isPot == 1)
	{
		baseNote = rtCounter;
	}

	// channelSelect
	if (selectCh == true && b == 1)
	{
		channel = rtCounter;
		// OLED display.write("Channel" + channel);
		Serial.print("channel = ");
		Serial.println(channel);
		selectCh = false;
		setMode(0);
	}

	// adjust ADSR
	if (selectADSR)
	{
		// long press sets adsrType then changes encoder to ADSRval contrain range
		if (adsrTypeSet == false && b == 3)
		{
			adsrType = adsrTypeArray[rtCounter - 1];
			Serial.print("adsrType = ");
			Serial.println(adsrType);
			//OLED display.write("adsrType" + adsrType);
			isPot = 2;
			adsrTypeSet = true;
		}

		if (adsrTypeSet)
		{
			adsrVal = rtCounter;
			if (adsrVal != lastAdsrVal)
			{
				Serial.print("midi cmd = ");
				Serial.print(channel);
				Serial.print(", ");
				Serial.print(adsrType);
				Serial.print(", ");
				Serial.println(adsrVal);
				// send midi cmd
				// midiCommand(channel, controller, controllerVal)
				// this is the actaull cmd
				// midiCommand(channel, adsrType, adsrVal);
				// OLED display.write("adsrVal" + adsrVal);
			}

			lastAdsrVal = adsrVal;
		}

		// single click goes back to set adsrType
		if (selectADSR == true && b == 1)
		{
			adsrTypeSet = false;
			isPot = 4;
			Serial.println("in ASDR select mode");
		}

		// double click to exit
		if (selectADSR == true && b == 2)
		{
			selectADSR = false;
			adsrTypeSet = false;
			doScaleMod = false;
			// setMode(0);
		}
	}

	// broadcast
	if (doBroadCast == true && b == 1)
	{
		// fwd = true
		// Serial.println(fwd);
		channel = channelsOn[rtCounter - 1];

		if (fwd[rtCounter - 1] == 1)
		{
			fwd[rtCounter - 1] = 0;
		}
		else
		{
			fwd[rtCounter - 1] = 1;
		}

		// OLED display.write("Channel" + channel);
		Serial.print("broadcast channel = ");
		Serial.print(channel);
		Serial.print(" ");
		Serial.println(fwd[rtCounter - 1]);
		doBroadCast = false;
		setMode(0);
	}

	// record and/or loop - can we do it with Logic X functions - currently not sending MIDI cmd for some reason ??  js 20200428 11:01am
	if (doRecord == true && b == 1)
	{
		midiCommand(channel, 49, 100, false); // learned cmd in Logic doesn't seem to be working
		Serial.print("sent recordloop cmd");
	}

	if (doScaleMod == true)
	{
		// could add mod variations here
	}
}
