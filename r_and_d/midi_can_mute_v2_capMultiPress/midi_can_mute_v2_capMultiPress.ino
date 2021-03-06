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
#include "display.h"
#include "tcp_server.h"
#include "udp_server.h"
#include "capacitive_touch.h"


void setup()
{
  // init serial
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Serial started");

  //modeText = "calibrate";
  disCh = 0;
  //display.clearDisplay();
  draw();
  drawMode(10, rtCounter);
  //  drawSensorCal(00);
  //drawCh(disCh);

  //start with can Calibration
  canCalibration();

  // encoder button
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

  // start capacitive sensors
  initCapacitive();

  // connect to wifi and start TCP server
  initWifi();

  // Start UPD server
  initUdp();

  Serial.print("canCapMax = ");
  Serial.println(canCapMax);
  setMode(0);
  drawModeSelect(true);
  drawOctave(3);
  drawSlidesONotes(asSlides);
}

void loop()
{
  webApi();

  handleCapBtns();
  handleCapSlides();
  handleReceiveUdp();
  handleEncoder();
  handleChannel();
  handleBroadcast();
  handleChMute();
  handleChVolAdjust();
  handleRecord();
  handleScale();
  handleChords();
  //capCalibration_Debug(); // uncomment out to show sensor values in  serial monitor

}

void handleEncoder()
{
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
}

void handleChannel()
{
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
}

void handleBroadcast()
{
  // broadcast
  if (doBroadCast == true) drawBroadcast(fwd[rtCounter - 1]);

  if (doBroadCast == true && b == 1)
  {
    // fwd = true
    // Serial.println(fwd);
    channel = channelsOn[rtCounter - 1];

    if (fwd[rtCounter - 1] == 1)
    {
      fwd[rtCounter - 1] = 0;
      drawBroadcast(false);
    }
    else
    {
      fwd[rtCounter - 1] = 1;
      drawBroadcast(true);
    }

    // OLED display.write("Channel" + channel);
    Serial.print("broadcast channel = ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.println(fwd[rtCounter - 1]);
    doBroadCast = false;
    setMode(8);
  }
}

void handleChMute()
{
  // ChVolume
  if (chMute == true) {
    //Serial.println("channel vol on");
    //    capChMute = !capChMute;  // set cap buttons to be channel vol on
  } else {
    // Serial.println("channel vol off");
    capChMute = false; // set cap buttons to be channel vol off
  }

}

void handleChVolAdjust() {
  int volIncrement;

  String myHex;
  byte bHex;
  if (volAdjust == true) {
    if (lastRtCounter != rtCounter) {
      if (lastRtCounter < rtCounter) rtCounter += 10;
      else rtCounter -= 10;

      rtCounter = constrain(rtCounter, 1, 127);
      myHex = decToHex(rtCounter, 4);
      bHex = myHex.toInt();
      Serial.println(myHex);
//      midiCommand(channel, noteValue, rtCounter, 0);
      //midiOn(channel, noteValue);
      midiVolCtrl(channel, noteValue, rtCounter);
    }
    lastRtCounter = rtCounter;
    lastVolInc = volIncrement;
  }
}

void handleRecord()
{
  // record and/or loop - can we do it with Logic X functions
  // WARN: currently not sending MIDI cmd for some reason ?? js 20200428 11:01am
  if (doRecord == true && b == 1)
  {
    midiCommand(channel, 49, 100, false); // learned cmd in Logic doesn't seem to be working
    Serial.print("sent recordloop cmd");
  }
}

void handleScale()
{
  if (doScale == true || doScaleMod == true)
  {
    // could add mod variations here
    channel = rtCounter;


  }
}

void handleChords()
{
  if (asChords == true)
  {
    if (b == 3)
    {
      chordsOctave = true;
      Serial.println("entering change octave mode");
    }
    else if (b == 1)
    {
      chordsOctave = false;
      drawOctaveSelect(false);
      setMode(1);
    }
  }

  octaveChange(); // change octave

  currentPitchMillis = millis();
}

String decToHex(byte decValue, byte desiredStringLength) {

  String hexString = String(decValue, HEX);
  if (decValue > 15) while (hexString.length() < desiredStringLength) hexString = "0x" + hexString;
  else hexString = "0x0" + hexString;
  return hexString;

}
