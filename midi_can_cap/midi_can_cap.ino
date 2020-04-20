/*
  creating buffer: https://www.arduino.cc/en/Tutorial/UdpNTPClient
*/

#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <MIDIUSB.h>
#include <CapacitiveSensor.h>

#include "secrets.h"

// WIFI

char ssid[] = _SSID;
char pass[] = _PASSWORD;

// UDP

WiFiUDP Udp;

IPAddress localRpi(192, 168, 86, 234);

const int MIDI_PACKET_SIZE = 3;

byte packetBufferIn[MIDI_PACKET_SIZE];
byte packetBufferOut[MIDI_PACKET_SIZE];

unsigned int localPort = 5000;
unsigned long timestamp;

// MIDI

const int pot = A0;

int keys[] = {2, 3, 4, 5};
int lastKeyState[] = {0, 0, 0, 0};

int keyCount = 4;
int baseNote = 0;
int noteValue = baseNote;

//CAPACITIVE
// add >= 300kΩ resistor between send pin (11) & receive pin (12)
// https://youtu.be/jco-uU5ZgEU?t=225  (more about capacitive send and receive)
CapacitiveSensor slideSensor1 = CapacitiveSensor(11, 12);  
CapacitiveSensor slideSensor2 = CapacitiveSensor(11, 10);
CapacitiveSensor btnSensor1 = CapacitiveSensor(8, 9);
CapacitiveSensor btnSensor2 = CapacitiveSensor(6, 7);

//CAP btn vars
int capBtnState[] = {LOW, LOW};
const int N_CAPBTNS = 2;
int capBtns [N_CAPBTNS];
int lastCapBtnState[] = {LOW, LOW};

//CAP slide vars
int capSlideState[] = {LOW, LOW};
const int N_CAPSLIDES = 2;
int capSlides [N_CAPSLIDES];
int lastCapSlideState[] = {LOW, LOW};
int slideNoteVal[N_CAPSLIDES];
int slideNote[N_CAPSLIDES];

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
  handleCapBtns();
  handleCapSlides();

  handleReceiveUdp();

//  delay(200);



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
  baseNote = map(analogRead(pot), 0, 1023, 0, 110); // these values should be adjusted - need calibration fn
  //Serial.println(baseNote);

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


void handleCapBtns() {

  capBtns[0] =  btnSensor1.capacitiveSensor(30);
  capBtns[1] =  btnSensor2.capacitiveSensor(30);

  for (int b = 0; b < N_CAPBTNS; b++) {
    checkCapBtnVals(b);
  }

  for (int c = 0; c <  N_CAPBTNS; c++) {

    int capBtnStateVal = capBtnState[c];

    if (capBtnStateVal != lastCapBtnState[c])
    {
      if (capBtnStateVal == HIGH)
      {
        Serial.println("turn midi signal on");
        midiOn(3 - c, baseNote);
      }
      else
      {
        midiOff(3 - c, baseNote);

        Serial.println("turn midi signal off");
        capBtnState[c] = LOW;
      }

      lastCapBtnState[c] = capBtnStateVal;
      delay(10);
    }
  }
}

void checkCapBtnVals(int myVar) {
  if (capBtns[myVar] > 1100) { // this value should be adjusted - need calibration fn
    capBtnState[myVar] = HIGH;
  } else {
    capBtnState[myVar] = LOW;
  }
}


void handleCapSlides() {

  capSlides[0] = slideSensor1.capacitiveSensor(30);
  capSlides[1] = slideSensor2.capacitiveSensor(30);

  for (int b = 0; b < N_CAPBTNS; b++) {
    checkCapSlideVals(b);
  }

  for (int c = 0; c <  N_CAPBTNS; c++) {

  //  slideNoteVal[c] = map ( capSlides[c], 100, 2800, 0, 110);
 
//    Serial.print("slideNoteVal[");
//    Serial.print(c);
//    Serial.print("] = ");
//    Serial.print(slideNoteVal[c]);
//    Serial.print("\t");
//    Serial.print("slideNote[");
//    Serial.print(c);
//    Serial.print("] = ");
//    Serial.print(slideNote[c]);    
//    Serial.print("\t");
//    Serial.print("capSlides[");
//    Serial.print(c);
//    Serial.print("] = ");
//    Serial.println(capSlides[c]); 
    
    slideNote[c] = slideNoteVal[c];
    slideNote[c] = constrain(slideNote[c], 10, 110);

    int capSlideStateVal = capSlideState[c];

      if (capSlideStateVal == HIGH)
      {
        Serial.println("turn midi slide signal on");
        midiOn(6 - c, slideNote[c]);
      }
      else
      {
      //  midiOff(6 - c, slideNote[c]);
      //  Serial.println("turn midi slide signal off");
        capSlideState[c] = LOW;
      }

      delay(10);
  }
}

void checkCapSlideVals(int myVar) {
  if (capSlides[myVar] > 1100) {  // this value should be adjusted - need calibration fn
    capSlideState[myVar] = HIGH;
    slideNoteVal[myVar] = map ( capSlides[myVar], 100, 2800, 0, 110); // these values should be adjusted - need calibration fn
  } else {
    capSlideState[myVar] = LOW;
  }
}





void midiOn(int chnl, int noteValue)
{
  // Serial.println(chnl);
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
    case 5:
      midiCommand(0x94, noteValue, 0x7F, true);
      break;
    case 6:
      midiCommand(0x95, noteValue, 0x7F, true);
      break;
    case 7:
      midiCommand(0x96, noteValue, 0x7F, true);
      break;
    case 8:
      midiCommand(0x97, noteValue, 0x7F, true);
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
    case 5:
      midiCommand(0x84, noteValue, 0x7F, true);
      break;
    case 6:
      midiCommand(0x85, noteValue, 0x7F, true);
      break;
    case 7:
      midiCommand(0x86, noteValue, 0x7F, true);;
      break;
    case 8:
      midiCommand(0x87, noteValue, 0x7F, true);
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
