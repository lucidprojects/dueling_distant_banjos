/*

  NETWORK - using nodejs to send signals across the interwebs via UDP to affect participants MIDI environment
  creating buffer: https://www.arduino.cc/en/Tutorial/UdpNTPClient

  MIDI - using MIDIUSB.h to send MIDI cmds to DAW

  CALIBRATION - iterate through an array of sensors and calibrate each sensor.  Storing values in an array of mins/maxes.
  • Can calibration will run first - canCalibrationTime  eg first 10 seconds
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
#include "encoder_mode_select.h"

// SCALE
#include <pitchToNote.h>
// the intervals in a major and natural minor scale:
int major[] = {2, 2, 1, 2, 2, 2, 1};
int naturalMinor[] = {2, 1, 2, 2, 1, 2, 2};
// an array to hold the final notes of the scale:
int scale[8];

// start with middle C:
int tonic = pitchC4;
// note to play:
int scaleValue = tonic;

// WIFI

char ssid[] = _SSID;
char pass[] = _PASSWORD;

// UDP

WiFiUDP Udp;

IPAddress localRpi(192, 168, 86, 234);

//NETWORK
const int MIDI_PACKET_SIZE = 3;

byte packetBufferIn[MIDI_PACKET_SIZE];
byte packetBufferOut[MIDI_PACKET_SIZE];

unsigned int localPort = 5000;
unsigned long timestamp;

int fwd[] = {0, 0, 0, 0, 0, 0, 0, 0};

// MIDI

int baseNote = 35;
int noteValue = baseNote;
int myScaleNote;
int pitchBend;
byte msb;      // get the high bits
byte lsb;  // get the low 8 bits


byte channelsOn[] = {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98};
byte channelsOff[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88};


//LED
int red = A3; //this sets the red led pin
int green = 0 ; //this sets the green led pin
int blue = 1 ; //this sets the green led pin

int ledVal = 0;


//CAPACITIVE

// can calibration will run first - canCalibrationTime  eg first 10 seconds
// purple LED and set capacitance of can
// store capacitance of can just touching / holding it
int canCap; // sensor object
int canCalibrationTime = 10000;  // set calibration time for can
int canCapMax;
int canCapMin;


//initialize sensors
// add >= 300kΩ resistor between send pin (11) & receive pin (12)
// sensor wire connected to receive pin
// higher resistor more sensitve 1mΩ or > will make it almost too sensitve - capacitve readings just by hovering
// https://youtu.be/jco-uU5ZgEU?t=225  (more about capacitive send and receive)
CapacitiveSensor slideSensor1 = CapacitiveSensor(11, 12);
CapacitiveSensor slideSensor2 = CapacitiveSensor(11, 10);
CapacitiveSensor slideSensor3 = CapacitiveSensor(8, 9);
CapacitiveSensor slideSensor4 = CapacitiveSensor(6, 7);

CapacitiveSensor btnSensor1 = CapacitiveSensor(6, 4);
CapacitiveSensor btnSensor2 = CapacitiveSensor(A7, A6);
CapacitiveSensor btnSensor3 = CapacitiveSensor(6, 5);
CapacitiveSensor btnSensor4 = CapacitiveSensor(2, 3);

//CAP btn vars
int capBtnState[] = {LOW, LOW, LOW, LOW};
const int N_CAPBTNS = 4;
int capBtns [N_CAPBTNS];
int lastCapBtnState[] = {LOW, LOW, LOW, LOW};

//CAP slide vars
int capSlideState[] = {LOW, LOW, LOW, LOW};
const int N_CAPSLIDES = 4;
int capSlides [N_CAPSLIDES];
int lastCapSlideState[] = {LOW, LOW, LOW, LOW};
int slideNoteVal[N_CAPSLIDES];
int slideNote[N_CAPSLIDES];

//CAP calibration
int calibrationTime = 4000;  // set calibration time per sensor
bool calComplete = false; // boolean to exit calibration while loop on completion

// combined array for all sensor calibration - not exactly sure what to do here.  starting combined
// take note of order index assignment in readSensors() so iteration in separate arrays works - e.g. capSlides need be set first
const int N_ALL_CAPSENS = 8;  // set this to total # of sensors (butons & slides)
int allCapSens[N_ALL_CAPSENS];   // sensor vars in an array so we can iterate through in for loop

int sensorValue[N_ALL_CAPSENS]; // the sensor value
int sensorMin[N_ALL_CAPSENS];   // minimum sensor value
int sensorMax[N_ALL_CAPSENS];   // maximum sensor value
int sensorValueRaw[N_ALL_CAPSENS]; // raw maximum sensor value - for debuging in loop

int sensorMin_length;
int sensorMax_length;

int sensorMinMIN, sensorMaxMAX;  // min max values after calibration
int sensorMinBuffer = 300; // buffer to prevent sensors from reading too low or 0


void setup()
{
  // init serial
  Serial.begin(9600);
  pinMode (buttonPin, INPUT_PULLUP);

  while (!Serial); // Wait for serial port to connect so we can see what is going on.

  Serial.println("Serial started");

  //start with can Calibration
  canCalibration();

  // Starts LED Yellow
  analogWrite(green, 255);
  digitalWrite(red, LOW);
  digitalWrite(blue, HIGH);


  // RGB LED pinModes
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);


  // take initial sensor readings - more consistent calibration readings
  readSensors();

  delay(2500); // small delay before sensor calibration starts

  // run callibration
  runSensorCalibration();

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

  Serial.print("canCapMax = ");
  Serial.println(canCapMax);


  //SCALE STUFF
  // fill the scale array with the scale you want:
  // start with the initial note:
  scale[0] = tonic;
  int note = scale[0];
  // iterate over the intervals, adding each to the next note
  // in the scale. You can change major to naturalMinor
  // if you want that kind of scale instead:
  for (int n = 0; n < 7; n++) {
    //    note = note + naturalMinor[n];
    note = note + major[n];
    scale[n + 1] = note;
  }

}

void loop()
{

  capCalibration_Debug();

  handleCapBtns();
  handleCapSlides();

  handleReceiveUdp();

  //  delay(200);

  if (isPot < 1) readEnc(0);
  else readEnc(isPot);

  // handle button press types
  b = checkButton();
  encoderButton(b);


  //set baseNote from Pot/encoder
  if (isPot == 1) baseNote = rtCounter;

  //channelSelect
  if (selectCh == true && b == 1 ) {
    channel = rtCounter;
    //OLED display.write("Channel" + channel);
    Serial.print("channel = ");
    Serial.println(channel);
    selectCh = false;
    setMode(0);
  }

  //adjust ADSR
  if (selectADSR) {
    if (adsrTypeSet == false && b == 3 ) { // long press sets adsrType then changes encoder to ADSRval contrain range
      adsrType = adsrTypeArray[rtCounter - 1];
      Serial.print("adsrType = ");
      Serial.println(adsrType);
      //OLED display.write("adsrType" + adsrType);
      isPot = 2;
      adsrTypeSet = true;
    }
    if (adsrTypeSet) {
      adsrVal = rtCounter;
      if (adsrVal != lastAdsrVal) {
        Serial.print("midi cmd = ");
        Serial.print(channel);
        Serial.print(", ");
        Serial.print(adsrType);
        Serial.print(", ");
        Serial.println(adsrVal);
        //send midi cmd
        // midiCommand(channel, controller, controllerVal)
        //this is the actaull cmd
        // midiCommand(channel, adsrType, adsrVal);

        //OLED display.write("adsrVal" + adsrVal);

      }
      lastAdsrVal = adsrVal;
    }
    if (selectADSR == true && b == 1 ) { // single click goes back to set adsrType
      adsrTypeSet = false;
      isPot = 4;
      Serial.println("in ASDR select mode");
    }
    //double click to exit
    if (selectADSR == true && b == 2 ) {
      selectADSR = false;
      adsrTypeSet = false;
      doScaleMod = false;
      // setMode(0);
    }
  }

  //broadcast
  if (doBroadCast == true && b == 1 ) {
    //fwd = true
    //    Serial.println(fwd);
    channel = channelsOn[rtCounter - 1];

    if ( fwd[rtCounter - 1] == 1 ) fwd[rtCounter - 1] = 0;
    else fwd[rtCounter - 1] = 1;

    //OLED display.write("Channel" + channel);
    Serial.print("broadcast channel = ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.println(fwd[rtCounter - 1]);
    doBroadCast = false;
    setMode(0);
  }


  //record and/or loop - can we do it with Logic X functions - currently not sending MIDI cmd for some reason ??  js 20200428 11:01am
  if (doRecord == true && b == 1) {
    midiCommand(channel, 49, 100, false);  // learned cmd in Logic doesn't seem to be working
    Serial.print("sent recordloop cmd");
  }


  if (doScaleMod == true) {
    // could add mod variations here
  }


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

void handleCapBtns() {

  capBtns[0] =  btnSensor1.capacitiveSensor(30);
  capBtns[1] =  btnSensor2.capacitiveSensor(30);
  capBtns[2] =  btnSensor3.capacitiveSensor(30);
  capBtns[3] =  btnSensor4.capacitiveSensor(30);

  for (int b = 0; b < N_CAPBTNS; b++) {
    checkCapBtnVals(b);
  }

  for (int c = 0; c <  N_CAPBTNS; c++) {

    int capBtnStateVal = capBtnState[c];

    if (capBtnStateVal != lastCapBtnState[c])
    {
      if (capBtnStateVal == HIGH)
      {

        if (doScale == true || doScaleMod == true) {  // all cap inputs play scale
          //midiOn(4 - c, scale[c]);
          midiOn(9, scale[c]);  // send scale notes all on same channel
          Serial.print("Sending scale note scale[");
          Serial.print(c);
          Serial.println("]");
          myScaleNote = scale[c];
        } else midiOn(4 - c, baseNote); // the first value sent in the fn will need to be adjusted for final channel assignment
      }
      else
      {
        if (doScale == true || doScaleMod == true) midiOff(9, baseNote);
        else midiOff(4 - c, baseNote); // the first value sent in the fn will need to be adjusted for final channel assignment
        Serial.println("turn midi signal off");
        capBtnState[c] = LOW;
      }

      lastCapBtnState[c] = capBtnStateVal;
      delay(10);
    }
  }
}

void checkCapBtnVals(int myVar) {
  if (capBtns[myVar] > canCapMax + sensorMinBuffer) { // threshold value is set by canCapMax (set by canCalibration() fn) + sensorMinBuffer. can be adjusted to dial in sensitivity
    capBtnState[myVar] = HIGH;
  } else {
    capBtnState[myVar] = LOW;
  }
}


void handleCapSlides() {

  capSlides[0] = slideSensor1.capacitiveSensor(30);
  capSlides[1] = slideSensor2.capacitiveSensor(30);
  capSlides[2] = slideSensor3.capacitiveSensor(30);
  capSlides[3] = slideSensor4.capacitiveSensor(30);

  for (int b = 0; b < N_CAPSLIDES; b++) {
    checkCapSlideVals(b);
  }

  for (int c = 0; c <  N_CAPSLIDES; c++) {

    if (asSlides) {
      slideNote[c] = slideNoteVal[c];
      slideNote[c] = constrain(slideNote[c], 10, 90);
    } else slideNote[c] = baseNote;


    int capSlideStateVal = capSlideState[c];

    if (capSlideStateVal == HIGH)
    {
      Serial.println("turn midi slide signal on");
      if (doScale == true) { // all cap inputs play scale
        //           midiOn(8 - c, scale[4+c]);
        midiOn(9, scale[4 + c]); // send scale notes all on same channel
        Serial.print("Sending scale note scale[");
        Serial.print(c);
        Serial.println("]");
      } if (doScaleMod == true) { // all cap inputs play scale
        pitchBend = map(slideNoteVal[c], 10, 90, 0, 16383); //constrain values could be adjusted here
        pitchBend = pitchBend << 1;          // shift low byte's msb to high byte
        msb = highByte(pitchBend);      // get the high bits
        lsb = lowByte(pitchBend) >> 1;  // get the low 8 bits

        // print the values, for reference:

        Serial.print("msb = ");
        Serial.print(msb);
        Serial.print(" lsb = ");
        Serial.println(lsb);

        // send the pitch bend message:
        midiCommand(0xE8, lsb, msb, false);  // pitchbend weirdness - first value is import here


      } else midiOn(8 - c, slideNote[c]); // the first value sent in the fn will need to be adjusted for final channel assignment
    }
    else
    {
      //  midiOff(8 - c, slideNote[c]);
      //  Serial.println("turn midi slide signal off");  // never turning it off helps slide effect
      capSlideState[c] = LOW;
    }

    delay(10);
  }
}

void checkCapSlideVals(int myVar) {
  if (capSlides[myVar] > canCapMax + sensorMinBuffer) { // threshold value is set by canCapMax (set by canCalibration() fn) + sensorMinBuffer. can be adjusted to dial in sensitivity
    capSlideState[myVar] = HIGH;
    // slide sensors need to always be first in the shared N_ALL_CAPSENS array
    // so we can iterate properly in the N_CAPSLIDES array
    // and properlly set min / max vals
    slideNoteVal[myVar] = map ( capSlides[myVar], sensorMin[myVar], sensorMax[myVar], 0, 90); // these values return from calibration fn
  } else {
    capSlideState[myVar] = LOW;
  }
}


// ###### CALIBRATION FUNCTIONS ######

// read sensor values
void readSensors() {
  allCapSens[0] =  slideSensor1.capacitiveSensor(30);
  allCapSens[1] =  slideSensor2.capacitiveSensor(30);
  allCapSens[2] =  slideSensor3.capacitiveSensor(30);
  allCapSens[3] =  slideSensor4.capacitiveSensor(30);
  allCapSens[4] =  btnSensor1.capacitiveSensor(30);
  allCapSens[5] =  btnSensor2.capacitiveSensor(30);
  allCapSens[6] =  btnSensor3.capacitiveSensor(30);
  allCapSens[7] =  btnSensor4.capacitiveSensor(30);
}


// used to set can capacitance. used in vaious thresholds throughout program
void canCalibration() {

  // purple LED to denote can calibration mode
  analogWrite(red, 80);
  analogWrite(green, 0);
  analogWrite(blue, 80);

  int canCapMillisNow  = millis();

  while (millis() < (canCapMillisNow + canCalibrationTime)) {
    Serial.println("check can capacitance");

    canCap = slideSensor1.capacitiveSensor(30);

    for (int c = 0; c < 5; c++) {

      canCap = slideSensor1.capacitiveSensor(30);
      // record the maximum sensor value
      if (canCap > canCapMax) {
        canCapMax = canCap;
      }
      // record the minimum sensor value
      if (canCap < canCapMin) {
        canCapMin = canCap;
      }
      if (canCap > 20) {
        Serial.print("canCap = ");
        Serial.println(canCap);
      }
    }
  }
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


    int capDebugVals;

    if ( canCapMax > 100) capDebugVals = canCapMax + sensorMinBuffer; else capDebugVals = 400;

    //debugging - show values when sensor > 400
    if (allCapSens[c] > capDebugVals) {

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
// ###### END CALIBRATION FUNCTIONS ######


void midiOn(int chnl, int noteValue)
{
  // Serial.println(chnl);
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
    case 9: // added channel 9 for scale notes to all play on same channel
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



void midiCommand(byte cmd, byte data1, byte data2, int fwd)
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
