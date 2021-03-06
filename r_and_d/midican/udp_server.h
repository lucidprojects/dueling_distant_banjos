#include <WiFiUdp.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>

// UDP

WiFiUDP Udp;

IPAddress localRpi(192, 168, 86, 234);

const int MIDI_PACKET_SIZE = 3;

byte packetBufferIn[MIDI_PACKET_SIZE];
byte packetBufferOut[MIDI_PACKET_SIZE];

unsigned int localPort = 5000;
unsigned long timestamp;

// MIDI

int baseNote = 35;
int noteValue = baseNote;

int myScaleNote;
int pitchBend;

byte msb; // get the high bits
byte lsb; // get the low 8 bits

byte channelsOn[] = {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98};
byte channelsOff[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88};

int fwd[] = {0, 0, 0, 0, 0, 0, 0, 0};

void initUdp()
{
	Udp.begin(localPort);
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

void midiCommand(byte cmd, byte data1, byte data2, int fwd)
{
	midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
	MidiUSB.sendMIDI(midiMsg);

	if (fwd)
	{
		handleSendUpd(cmd, data1, data2);
	}
}

void midiOn(int chnl, int noteValue)
{
	switch (chnl)
	{
	case 1:
		midiCommand(channelsOn[0], noteValue, 0x7F, fwd[0]);
		break;
	case 2:
		midiCommand(channelsOn[1], noteValue, 0x7F, fwd[1]);
		break;
	case 3:
		midiCommand(channelsOn[2], noteValue, 0x7F, fwd[2]);
		break;
	case 4:
		midiCommand(channelsOn[3], noteValue, 0x7F, fwd[3]);
		break;
	case 5:
		midiCommand(channelsOn[4], noteValue, 0x7F, fwd[4]);
		break;
	case 6:
		midiCommand(channelsOn[5], noteValue, 0x7F, fwd[5]);
		break;
	case 7:
		midiCommand(channelsOn[6], noteValue, 0x7F, fwd[6]);
		break;
	case 8:
		midiCommand(channelsOn[7], noteValue, 0x7F, fwd[7]);
		break;
	case 9:
		// channel 9 for scale notes to all play on same channel
		midiCommand(channelsOn[8], noteValue, 0x7F, fwd[8]);
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
		midiCommand(channelsOff[0], noteValue, 0x7F, fwd[0]);
		break;
	case 2:
		midiCommand(channelsOff[1], noteValue, 0x7F, fwd[1]);
		break;
	case 3:
		midiCommand(channelsOff[2], noteValue, 0x7F, fwd[2]);
		break;
	case 4:
		midiCommand(channelsOff[3], noteValue, 0x7F, fwd[3]);
		break;
	case 5:
		midiCommand(channelsOff[4], noteValue, 0x7F, fwd[4]);
		break;
	case 6:
		midiCommand(channelsOff[5], noteValue, 0x7F, fwd[5]);
		break;
	case 7:
		midiCommand(channelsOff[6], noteValue, 0x7F, fwd[6]);
		break;
	case 8:
		midiCommand(channelsOff[7], noteValue, 0x7F, fwd[7]);
		break;
	case 9:
		midiCommand(channelsOff[8], noteValue, 0x7F, fwd[8]);
		break;
	default:
		break;
	}
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