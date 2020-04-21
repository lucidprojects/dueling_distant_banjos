/*
  
  NETWORK - using nodejs to send signals across the interwebs via UDP to affect participants MIDI environment
  creating buffer: https://www.arduino.cc/en/Tutorial/UdpNTPClient


  
  MIDI - using MIDIUSB.h to send MIDI cmds to DAW   

  CALIBRATION - iterate through an array of sensors and calibrate each sensor.  Storing values in an array of mins/maxes.
  • Set calibration time per sensor in calibrationTime var, used in runSensorCalibration() fn
  • During calirbration tap and release sensor to set min & max values
  • Uses RGB LED to denote different calibration modes:
    Starts in yellow. blue calibrating, toggle red switching to next sensor back to blue. solid red when calibration complete
    Short green when completing setup()
    Yellow to start loop()
  • Assigns min sensorMin[] in sensorMinMIN & max sensorMax[] in sensorMaxMAX
    use these vars for various thresholds
    
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

//LED
int red = A3; //this sets the red led pin
int green = A4 ; //this sets the green led pin
int blue = A5 ; //this sets the green led pin

int ledVal = 0;


//CAPACITIVE

//initialize senors
// add >= 300kΩ resistor between send pin (11) & receive pin (12)
// sensor wire connected to receive pin
// high resistor more sensitve 1mΩ or < will make it capacitive by hovering
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

//CAP calibration
int calibrationTime = 3000;  // set calibration time per sensor
bool calComplete = false; // boolean to exit calibration while loop on completion

// combined array for all sensor calibration - not exactly sure what to do here.  starting combined 
// take note of order index assignment in readSensors() so iteration in separate arrays works - e.g. capSlides need be set first 
const int N_ALL_CAPSENS = 4;  // set this to total # of sensors (butons & slides)
int allCapSens[N_ALL_CAPSENS];   // sensor vars in an array so we can iterate through in for loop

int sensorValue[N_ALL_CAPSENS]; // the sensor value
int sensorMin[N_ALL_CAPSENS];   // minimum sensor value
int sensorMax[N_ALL_CAPSENS];   // maximum sensor value
int sensorValueRaw[N_ALL_CAPSENS]; // raw maximum sensor value - for debuging in loop

int sensorMin_length;
int sensorMax_length;

int sensorMinMIN, sensorMaxMAX;  // min max values after calibration
int sensorMinBuffer = 200; // buffer to prevent sensors from reading too low or 0


void setup()
{
  // init serial
  Serial.begin(9600);

  // Starts LED Yellow
  analogWrite(green, 255);
  digitalWrite(red, LOW);
  digitalWrite(blue, HIGH);


  // RGB LED pinModes
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  while (!Serial); // Wait for serial port to connect so we can see what is going on.

  Serial.println("Serial started");

  // take initial sensor readings - more consistent calibration readings
  readSensors();

  // run callibration
  runSensorCalibration();

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

  capCalibration_Debug();

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
  if (capBtns[myVar] > sensorMinMIN + sensorMinBuffer) { // this value should be adjusted - need calibration fn
    capBtnState[myVar] = HIGH;
  } else {
    capBtnState[myVar] = LOW;
  }
}


void handleCapSlides() {

  capSlides[0] = slideSensor1.capacitiveSensor(30);
  capSlides[1] = slideSensor2.capacitiveSensor(30);

  for (int b = 0; b < N_CAPSLIDES; b++) {
    checkCapSlideVals(b);
  }

  for (int c = 0; c <  N_CAPSLIDES; c++) {

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
  if (capSlides[myVar] > sensorMinMIN + sensorMinBuffer) {  //check sensor values is > than this value return from calibration fn + buffer
    capSlideState[myVar] = HIGH;
    // slide sensors need to always be first in the shared N_ALL_CAPSENS array
    // so we can iterate properly in the N_CAPSLIDES array
    // and properlly set min / max vals
    slideNoteVal[myVar] = map ( capSlides[myVar], sensorMin[myVar], sensorMax[myVar], 0, 110); // these values return from calibration fn
  } else {
    capSlideState[myVar] = LOW;
  }
}


// ###### CALIBTATION FUNCTIONS ######

// read sensor values
void readSensors() {
  allCapSens[0] =  slideSensor1.capacitiveSensor(30);
  allCapSens[1] =  slideSensor2.capacitiveSensor(30);
  allCapSens[2] =  btnSensor1.capacitiveSensor(30);
  allCapSens[3] =  btnSensor2.capacitiveSensor(30);
}

// read/show cap values adjust led usd in loop() 
// this is mostly for debugging and can be removed / commented out in final code - js 20200421
void capCalibration_Debug() {
  // read the sensors:
  readSensors();

  for (int c = 0; c < N_ALL_CAPSENS; c++) {

    sensorValue[c] = allCapSens[c];
    sensorValueRaw[c] = allCapSens[c];

    // apply the calibration to the sensor reading
    sensorValue[c] = map(sensorValue[c], sensorMin[c], sensorMax[c], 0, 255);

    // in case the sensor value is outside the range seen during calibration // this is for LED
    sensorValue[c] = constrain(sensorValue[c], 0, 255);

    // set ledVals fade from yellow to green based on calibrated reading
    setLedVal(c);

    //debugging - show values when sensor > 400
    if (allCapSens[c] > 400) {

      Serial.print("sensorValue[");
      Serial.print(c);
      Serial.print("] = ");
      Serial.print(sensorValue[c]);
      Serial.print("\t");

      Serial.print("sensorValueRaw[");
      Serial.print(c);
      Serial.print("] = ");
      Serial.print(sensorValueRaw[c]);
      Serial.print("\t");

      Serial.print("sensorMin[");
      Serial.print(c);
      Serial.print("] = ");
      Serial.print(sensorMin[c]);
      Serial.print("\t");

      Serial.print("sensorMax[");
      Serial.print(c);
      Serial.print("] = ");
      Serial.print(sensorMax[c]);
      Serial.println("\t");

    }
  }
}


// get min and max values from sensorMin & sensorMax arrays
int getMinMax(int* array, int size) {
  int sensorMinMIN = array[0];
  for (int i = 0; i < size; i++)
  {
    if (array[i] < sensorMinMIN) sensorMinMIN = array[i];
  }

  int tempMinimum = sensorMinMIN;
  for (int j = 0; j < size; j++)
  {
    if (array[j] > tempMinimum) sensorMaxMAX = array[j];
  }

  // sets maximum in above for loop but returns minimum to prevent 0 value min
  return sensorMinMIN;
}


// main calibration fn used in setup()
void runSensorCalibration() {
  while (calComplete == false) {
    // take sensor readings again - more consistent calibration readings
    readSensors();

    for (int c = 0; c < N_ALL_CAPSENS; c++) { // iterate through sensors

      sensorMin[c] = 1023;        // initial minimum sensor value
      sensorMax[c] = 0;           // initial maximum sensor value

      if (c < N_ALL_CAPSENS) {  // do this to only go through the loop once per number of sensors

        digitalWrite(green, HIGH);
        digitalWrite(red, HIGH);
        digitalWrite(blue, LOW);

        int millisNow = millis();

        while (millis() < (millisNow + calibrationTime)) {  // 3000 = take calibration for 3 seconds - set in calibrationTime var
          setMinMaxVals(c);
        }

        // print which sensor just calibrated and toggle red LED to indicate next reading
        Serial.println(c);
        delay(1000);
        digitalWrite(green, HIGH);
        digitalWrite(red, LOW);
        digitalWrite(blue, HIGH);
        delay(300);

      }
    }

    // find min and max values from sensorMin & sensorMax arrays
    sensorMin_length = sizeof(sensorMin) / sizeof(int);
    sensorMax_length = sizeof(sensorMax) / sizeof(int);

    int sensorMinMIN = getMinMax(sensorMin, sensorMin_length); // pass the array and its size
    int sensorMinMAX = getMinMax(sensorMax, sensorMax_length); // pass the array and its size

    Serial.println(sensorMinMIN);
    Serial.println(sensorMaxMAX);


    calComplete = true;  // after iterating though # sensors

  }

  if (calComplete == true) {
    Serial.print("end of calibration array with length = ");
    Serial.println(N_ALL_CAPSENS);
    Serial.println("hello world");

    // signal the end of the calibration period  - LED will turn GREEN for a second
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);
    delay(1000);
  }

}



// used in setup() for calibration
void setMinMaxVals(int myVar) {

  readSensors();

  sensorValue[myVar] = allCapSens[myVar];  // take sensor X reading

  // record the maximum sensor value
  if (sensorValue[myVar] > sensorMax[myVar]) {
    sensorMax[myVar] = sensorValue[myVar];
  }

  // record the minimum sensor value
  if (sensorValue[myVar] < sensorMin[myVar]) {
    sensorMin[myVar] = sensorValue[myVar];
  }

  Serial.print("sensorValue[");
  Serial.print(myVar);
  Serial.print("] = ");
  Serial.print(sensorValue[myVar]);
  Serial.print("\t");

  Serial.print("sensorMin[");
  Serial.print(myVar);
  Serial.print("] = ");
  Serial.print(sensorMin[myVar]);
  Serial.print("\t");

  Serial.print("sensorMax[");
  Serial.print(myVar);
  Serial.print("] = ");
  Serial.print(sensorMax[myVar]);
  Serial.println("\t");

}


void setLedVal(int myLedVal) {

  if (sensorValueRaw[myLedVal] > 400)    {
    ledVal = sensorValue[myLedVal];
  }

  if (ledVal > 220 ) {
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);
  } else {
    analogWrite(green, ledVal);
    analogWrite(red, ledVal);
    digitalWrite(blue, HIGH);
  }


}
// ###### END CALIBTATION FUNCTIONS ######


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
