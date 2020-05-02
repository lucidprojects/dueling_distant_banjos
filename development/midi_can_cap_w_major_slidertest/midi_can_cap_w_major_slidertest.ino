
/* testing playing "major" scale with capacitve slide - below config is playing the scale on channel 5 0x94 */

#include <MIDIUSB.h>
#include <CapacitiveSensor.h>

// SCALE
#include <pitchToNote.h>
const int buttonPin = 4;

// the intervals in a major and natural minor scale:
int major[] = {2, 2, 1, 2, 2, 2, 1};
int naturalMinor[] = {2, 1, 2, 2, 1, 2, 2};
// an array to hold the final notes of the scale:
int scale[8];

// start with middle C:
int tonic = pitchC4;
// note to play:
int scaleValue = tonic;


// MIDI

const int pot = A0;

int keys[] = {2, 3, 4, 5};
int lastKeyState[] = {0, 0, 0, 0};

int keyCount = 4;
int baseNote = 0;
int noteValue = baseNote;
int slideNote, slideNoteVal, slideNote2, slideNoteVal2;

//CAPACITIVE


// add >= 300kΩ resistor between send pin (11) & receive pin (12)  
// https://youtu.be/jco-uU5ZgEU?t=225  (more about capacitive send and receive)

CapacitiveSensor Sensor1 = CapacitiveSensor(11, 12);  
CapacitiveSensor Sensor2 = CapacitiveSensor(11, 10);
CapacitiveSensor btnSensor1 = CapacitiveSensor(8, 9);
CapacitiveSensor btnSensor2 = CapacitiveSensor(6, 7);


long val1 , val2, val4, val3, btn1, btn2, btn3, btn4;




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
  handleInputs();
//  handleReceiveUdp();

  //SLIDERS

  val1 = Sensor1.capacitiveSensor(30);
  val2 = Sensor2.capacitiveSensor(30);

    Serial.print("vensor1 val = ");
    Serial.print(val1);
    Serial.print("\t");
    Serial.print("venso2 val = ");
    Serial.println(val2);


  //  int slideVal = map ( val1, 10, 4000, 0, 255);
  slideNoteVal = map ( val1, 500, 4000, 0, 70);
  slideNote = slideNoteVal;
  slideNote = constrain(slideNote, 10, 110);

  //  int slideVal2 = map ( val2, 10, 4000, 0, 255);
  slideNoteVal2 = map ( val2, 500, 4000, 0, 70);
  slideNote2 = slideNoteVal2;
  slideNote2 = constrain(slideNote2, 10, 110);


  if (val1 >= 600)
  {
     scaleValue = scale[random(8)];
      // play it:
      midiCommand(0x94, scaleValue, 0x7F, true);
 //   midiOn(5, slideNote);
    //    pos = 1;
    delay(10);
  }

  else if (val1 >= 600)
  {
    //    pos = 0;
     //midiCommand(0x94, scaleValue, 0, true);
    delay(10);
  }

  if (val2 >= 400)
  {
    midiOn(6,  slideNote2);
    delay(100);
  }

  else if (val2 >= 100)
  {
    delay(100);
  }


  // END SLIDERS


  // CAP BTNS


  btn1 =  btnSensor1.capacitiveSensor(30);
  btn2 =  btnSensor2.capacitiveSensor(30);


//  Serial.print("btn1");
//  Serial.print("\t");
//  Serial.print(btn1);
//  Serial.print("\t");
//  Serial.print("btn2");
//  Serial.print("\t");
//  Serial.println(btn2);



  baseNote = map(analogRead(pot), 0, 1023, 0, 110);



  if (btn1 > 1100) {
    midiOn(2, baseNote);
  } else if (btn1 < 1100) {
    //midiOff(2, baseNote);
  }

  if (btn2 > 1100) {
    midiOn(3, baseNote);
  } else if (btn1 < 1100) {
   // midiOff(3, baseNote);
  }



  // END CAP BTNS



  delay(10);



}


void handleInputs()
{
  baseNote = map(analogRead(pot), 0, 1023, 0, 110);
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

//  if (fwd)
//  {
//    handleSendUpd(cmd, data1, data2);
////  }

}
