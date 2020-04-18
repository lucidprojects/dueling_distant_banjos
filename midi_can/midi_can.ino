/*
	creating buffer: https://www.arduino.cc/en/Tutorial/UdpNTPClient
*/

#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <MIDIUSB.h>

#include "secrets.h"

// WIFI

char ssid[] = _SSID;
char pass[] = _PASSWORD;

// UDP

WiFiUDP Udp;

IPAddress localRpi(192, 168, 0, 168);

const int MIDI_PACKET_SIZE = 3;

byte packetBufferIn[MIDI_PACKET_SIZE];
byte packetBufferOut[MIDI_PACKET_SIZE];

unsigned int localPort = 5000;
unsigned long timestamp;

// MIDI

const int pot = A3;

int keys[] = {2, 3, 4, 5};
int lastKeyState[] = {0, 0, 0, 0};

int keyCount = 4;
int baseNote = 0;
int noteValue = baseNote;

void setup()
{
	// init serial
	Serial.begin(9600);
	while (!Serial);

	// init input pins
	for (int k = 0; k < keyCount; k++)
	{
		pinMode(keys[k], INPUT_PULLUP);
	}

	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("Communication with WiFi module failed!");
		while (true);
	}

	String fv = WiFi.firmwareVersion();
	if (fv < WIFI_FIRMWARE_LATEST_VERSION)
	{
		Serial.println("Please upgrade the firmware");
	}

	// attempt to connect to Wifi network:
	int status = WL_IDLE_STATUS;
	while (status != WL_CONNECTED)
	{
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pass);

		// wait 10 seconds for connection:
		delay(10000);
	}

	Serial.println("Connected to WiFi");
	printWifiStatus();

	Serial.println("\nStarting connection to sender server...");
	Udp.begin(localPort);
}

void loop()
{
	handleInputs();
	handleReceiveUdp();
}

void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

void handleInputs()
{
	baseNote = map(analogRead(pot), 0, 1023, 0, 110);

	for (int k = 0; k < keyCount; k++)
	{
		int keyState = digitalRead(keys[k]);

		if (keyState != lastKeyState[k])
		{
			if (keyState == LOW)
			{
				midiOn(k + 1, baseNote);
			}
			else
			{
				midiOff(k + 1, baseNote);
			}

			lastKeyState[k] = keyState;
			delay(10);
		}
	}
}

void midiOn(int chnl, int noteValue)
{
	switch (chnl)
	{
	case 1:
		midiCommand(0x90, noteValue, 0x7F, true);
		break;
	case 2:
		midiCommand(0x91, noteValue, 0x7F, true);
		break;
	case 3:
		midiCommand(0x92, noteValue, 0x7F, true);
		break;
	case 4:
		midiCommand(0x93, noteValue, 0x7F, true);
		break;
	default:
		break;
	}
}

void midiOff(int chnl, int noteValue)
{
	switch (chnl)
	{
	case 1:
		midiCommand(0x80, noteValue, 0x7F, true);
		break;
	case 2:
		midiCommand(0x81, noteValue, 0x7F, true);
		break;
	case 3:
		midiCommand(0x82, noteValue, 0x7F, true);
		break;
	case 4:
		midiCommand(0x83, noteValue, 0x7F, true);
		break;
	default:
		break;
	}
}

void midiCommand(byte cmd, byte data1, byte data2, bool fwd)
{
	midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
	MidiUSB.sendMIDI(midiMsg);

	if (fwd)
	{
		handleSendUpd(cmd, data1, data2);
	}
}

void handleSendUpd(byte cmd, byte data1, byte data2)
{
	memset(packetBufferOut, 0, MIDI_PACKET_SIZE);

	packetBufferOut[0] = cmd;
	packetBufferOut[1] = data1;
	packetBufferOut[2] = data2;

	Udp.beginPacket(localRpi, 5000);
	Udp.write(packetBufferOut, MIDI_PACKET_SIZE);
	Udp.endPacket();
}

void handleReceiveUdp() 
{
	// if there's data available, read a packet
	int packetSize = Udp.parsePacket();

	if (packetSize)
	{
		Serial.print("Received packet of size ");
		Serial.println(packetSize);
		Serial.print("From ");
		IPAddress remoteIp = Udp.remoteIP();
		Serial.print(remoteIp);
		Serial.print(", port ");
		Serial.println(Udp.remotePort());

		memset(packetBufferIn, 0, MIDI_PACKET_SIZE);
		Udp.read(packetBufferIn, MIDI_PACKET_SIZE);
		
		Serial.println("Contents:");
		Serial.println(packetBufferIn[0]);
		Serial.println(packetBufferIn[1]);
		Serial.println(packetBufferIn[2]);

		midiCommand(packetBufferIn[0], packetBufferIn[1], packetBufferIn[2], false);
	}
}
