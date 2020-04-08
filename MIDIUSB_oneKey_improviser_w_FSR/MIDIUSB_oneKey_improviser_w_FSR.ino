/*
  One-key piano MIDI player for MIDUSB

  Generates a  MIDI notes when you push the button.
  Randomizes the note within a scale each time

  Uses MIDIUSB for MIDI, so will work on any
  32U4-based board (e.g. Uno, Leonardo, Micro, Yún)

  Circuit:
      pushbutton attached to +Vcc from pin 5.
      10-kilohm resistor to ground from pin 5

  created 14 Jan 2019
  by Tom Igoe
*/
#include <MIDIUSB.h>
#include <pitchToNote.h>  // include pitch chart from MIDIUSB library
#include <PushButton.h>  // from https://github.com/tigoe/PushButton

PushButton playerButton1(2);
PushButton playerButton2(3);
PushButton playerButton3(4);
PushButton playerButton4(5);


// the intervals in a major and natural minor scale:
int major[] = {2, 2, 1, 2, 2, 2, 1};
int naturalMinor[] = {2, 1, 2, 2, 1, 2, 2};
// an array to hold the final notes of the scale:
int scale[8];

// start with middle C:
int tonic = pitchC4;
// note to play:
int noteValue = tonic;

// previous state of the button:
int lastButtonState = LOW;

// Define FSR pin:
#define fsrpin A1
#define fsr2pin A2


//Define variable to store sensor readings:
int fsrreading; //Variable to store FSR value
int fsr2reading; //Variable to store FSR value

#define perPot A0
int purPot;
int purPotreading;

void setup() {
   while (!Serial);
  playerButton1.begin(INPUT_PULLUP);
  playerButton2.begin(INPUT_PULLUP);
  playerButton3.begin(INPUT_PULLUP);
  playerButton3.begin(INPUT_PULLUP);
  
  // fill the scale array with the scale you want:
  // start with the initial note:
  scale[0] = tonic;
  int note = scale[0];
  // iterate over the intervals, adding each to the next note
  // in the scale. You can change major to naturalMinor
  // if you want that kind of scale instead:
  for (int n = 0; n < 7; n++) {
    note = note + naturalMinor[n];
    scale[n + 1] = note;
  }
}

void loop() {
  // read the pushbutton:
//  int buttonState = digitalRead(buttonPin);
//  // compare its state to the previous state:
//  if (buttonState != lastButtonState) {
//    // debounce delay:
//    delay(5);
//    // if the button's changed and it's pressed:
//    if (buttonState == LOW) {
//      // pick a random note in the scale:
//      noteValue = scale[random(8)];
//      // play it:
//      midiCommand(0x91, noteValue, 0x7F);
//    } else  {
//      // turn the note off:
//      midiCommand(0x81, noteValue, 0);
//    }
//    // save the button state for comparison next time through:
//    lastButtonState = buttonState;
//  }



  if (playerButton1.read() == LOW) {
    noteValue = scale[random(8)];
      // play it:
      midiCommand(0x91, noteValue, 0x7F);
  } else {
    midiCommand(0x81, noteValue, 0);
  }

   if (playerButton2.read() == LOW) {
     noteValue = scale[random(8)];
      
      // play it:
      midiCommand(0x92, noteValue, 0x7F);
    } else {
    midiCommand(0x82, noteValue, 0);
  }


 if (playerButton3.read() == LOW) {

    noteValue = scale[random(8)];
      // play it:
      midiCommand(0x93, noteValue, 0x7F);
  } else {
    midiCommand(0x83, noteValue, 0);
  }



 fsrreading = analogRead(fsrpin);
 fsr2reading = analogRead(fsr2pin);

  // Print the fsrreading in the serial monitor:
  // Print the string "Analog reading = ".
  Serial.print("Analog reading = ");
  // Print the fsrreading:
  Serial.print(fsrreading);

  // We can set some threshholds to display how much pressure is roughly applied:
  if (fsrreading < 15) {
    Serial.println(" - No pressure");
       noteValue = scale[random(82)];
      // play it:
      midiCommand(0x80, noteValue, 0);
  } else if (fsrreading < 200) {
    Serial.println(" - Light touch");
     noteValue = scale[random(0,2)];
      // play it:
      midiCommand(0x90, noteValue, 0x7F);
  } else if (fsrreading < 500) {
    Serial.println(" - Light squeeze");
     noteValue = scale[random(3,5)];
      // play it:
      midiCommand(0x90, noteValue, 0x7F);
  } else if (fsrreading < 800) {
        
    Serial.println(" - Medium squeeze");
     noteValue = scale[random(6,7)];
     Serial.print("noteValue =");
    Serial.println(noteValue);
      // play it:
      midiCommand(0x90, noteValue, 0x7F);
  } else {
    Serial.println(" - Big squeeze"); 
     noteValue = scale[random(8)];
      // play it:
      midiCommand(0x90, noteValue, 0x7F);
  }


  if (fsr2reading < 15) {
    Serial.println(" - No pressure fsr2");
       noteValue = scale[random(82)];
      // play it:
      midiCommand(0x84, noteValue, 0);
//  } else if (fsrreading < 200) {
//    Serial.println(" - Light touch");
//     noteValue = scale[random(0,2)];
//      // play it:
//      midiCommand(0x90, noteValue, 0x7F);
//  } else if (fsrreading < 500) {
//    Serial.println(" - Light squeeze");
//     noteValue = scale[random(3,5)];
//      // play it:
//      midiCommand(0x90, noteValue, 0x7F);
//  }
  }else if (fsr2reading < 800) {
    Serial.println(" - Medium squeeze fsr2");
    Serial.print("noteValue =");
    Serial.print(noteValue);
     noteValue = scale[random(6,7)];
      // play it:
      midiCommand(0x94, noteValue, 0x7F);
  } else {
    Serial.println(" - Big squeeze fsr2"); 
     noteValue = scale[random(8)];
     Serial.print("noteValue =");
    Serial.print(noteValue);
      // play it:
      midiCommand(0x94, noteValue, 0x6F);
  }


purPotreading = analogRead(purPot);
purPotreading = map(purPotreading, 0,1024, 38, 100);
   


//  midiCommand(0x95, purPotreading, 0x60);
  
  
   delay(200); //Delay 200 ms.
  
}

// send a 3-byte midi message
void midiCommand(byte cmd, byte data1, byte  data2) {
  // First parameter is the event type (top 4 bits of the command byte).
  // Second parameter is command byte combined with the channel.
  // Third parameter is the first data byte
  // Fourth parameter second data byte

  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}
