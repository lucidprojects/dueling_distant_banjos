#include <Encoder.h>
#include "button_multipress.h"

//const int buttonPin = 5; // pushbutton is on pin 5  // defining rotary button pin in button_multipress.h
Encoder knob(16, 15);     // initialize encoder on pins 0 and 1
int lastKnobState = 0;   // set last knob state
int rtCounter = 0; //incremet rtCounter for accurate counts instead of encoder variance.
int lastRtCounter = 0;
int isPot = 0;
int b;
int channel;  //store channel value
int adsrVal = 0; //ADSR value
int lastAdsrVal; // check if ADSR val changes
/*ADSR type
  1 ~ 49 (73 in hex) = Attack
  2 ~ 4B (75 in hex) = Decay
  3 ~ 40 (64 in hex)? = Sustain  (Hold Pedal)
  4 ~ 48 (72 in hex) = Release
*/
int adsrTypeArray[4] = {73, 75, 64, 72};
int adsrType;


bool adsrTypeSet = false;
bool selectCh = false;
bool selectADSR = false;
bool asSlides = true;
bool doScale = false;
bool doRecord = false;
bool doBroadCast = false;
bool doScaleMod = false;

//bool fwd = false;

// constrain encoder values
void constrainEnc(int minE, int maxE) {
  rtCounter = constrain(rtCounter, minE, maxE);
}

// eneable channel select in the loop
void selectChannel() {
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  selectCh = true;   // set mode bool to true for handling in loop
}

// eneable ADSR select in the loop
void ADSR() {
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  selectADSR = true; // set mode bool to true for handling in loop
}

// use slides as slides or notes
void slidesOrNotes() {
  asSlides = !asSlides;
  Serial.println(asSlides);
}

// use cap buttons a scales
void capsAsScales() {
  doScale = !doScale;
  Serial.println(doScale);
}

void doRecordLoop() {
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  doRecord = true; // set mode bool to true for handling in loop
  Serial.println(doRecord);
}



void broadCast() {
  //forward midi cmd per channel to receiver.
  //need to set channel to pass somehow.
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  doBroadCast = true; // set mode bool to true for handling in loop
  
}


void modScale(){
  //  Mod scale - 4btns as scales sliders modify btns
  doScaleMod = !doScaleMod; // toggle scale mod on and off
  Serial.println(doScaleMod);
}



void setMode(int myMode) {
  /*
    MODES
    0)DEFAULT ENCODER MODE - DONE
    1)CHANNEL SELECT - DONE
    2)ADSR - DONE  **need to integrate with cap_can**
    3)ACT AS POT note shifter - DONE  integrated
    4)SLIDES as slides or notes - DONE integrated
    5)All cap inputs as scale buttons - DONE integrated
    6)Record / loop  - no MIDI cmds I can find for this - tried Logix key assignment but doesn't seem to be working.
    7)Broadcast n CHANNEL - DONE integrated
  */

  Serial.print("set mode = ");
  Serial.println(myMode);

  switch (myMode) {
    case 0: // set default encoderMode
       //OLED display.write("select mode");
      isPot = 0;
      selectCh = false;
      doScaleMod = false;
      asSlides = true;
      doScale = false;
      break;
    case 1: // channel select mode;
      isPot = 3;
      Serial.println("channel select mode");
      //OLED display.write("select channel");
      selectChannel();
      break;
    case 2: // ADSR mode
      Serial.println("set to ADSR mode chose ADSR type");
      isPot = 4;
      //OLED display.write("select ADSR type");
      ADSR();
      break;
    case 3: // Encoder as Pot mode
      Serial.println("Encoder as \"Pot\" mode - audible MIDI notes");
      //OLED display.write("in Pot mode");
      isPot = 1;
      break;
    case 4: // Slides or notes mode
      Serial.println("Slides or notes mode");
      //OLED display.write("asSlides" + asSlides);
      slidesOrNotes();
      break;
    case 5: // All cap inputs as scale buttons mode
      Serial.println("All cap inputs as scale buttons mode");
       //OLED display.write("asCaps" + asCaps);
      capsAsScales();
      break;
    case 6: // Record / Loop mode
      Serial.println("Record / Loop mode");
      doRecordLoop();
      break;
    case 7: // Broadcast X CHANNEL
      Serial.println("Broadcast X CHANNEL");
      isPot = 3;
      //OLED display.write("broadcast channel" + channel);
      broadCast();
      break;
    case 8: // Mod scale - 4btns as scales sliders modify btns
      Serial.println("Mod Scale mode");
      isPot = 5;
      //OLED display.write(""Mod Scale mode);
      modScale();
      break;  
    default:
      break;
  }
}


void  encoderButton(int b) {

  switch (b) {
    case 0:
      break;
    case 1:
      Serial.println("single click");
      if (isPot == 0)  setMode(rtCounter);
      break;
    case 2:
      Serial.println("double click");
      setMode(0);
      break;
    case 3:
      Serial.println("press and hold");
      break;
    case 4:
      Serial.println("long press and hold");
      break;
    default:
      setMode(rtCounter);
      break;
  }

}

void readEnc(int encMode) {
  //Tom's encoder code
  //checks for absolute value to check for forward or back movement
  //incrementing rtCounter for better accuracy from the encoder
  int knobState = knob.read();
  int knobChange = knobState - lastKnobState;
  // if it's changed by 4 or more pulses:
  if (abs(knobChange) >= 4) {
    //want direction of change, not value, so
    // divide value by abs. value to get 1 or -1:
    knobChange = knobChange / abs(knobChange);

    // if knobChange == 1, turned up/right rtCounter++, else rtCounter--
    if (knobChange == 1) rtCounter++;
    else rtCounter --;

    // save the current state for comparison next time:
    lastKnobState = knobState;

    // handle whether encoder acts as encoder or "potentiometer" and the range
    switch (encMode) {
      case 0: //default - mode select encoder
        if (rtCounter < 1) rtCounter = 8;
        if (rtCounter > 8) rtCounter = 1;
        break;
      case 1: //encoder as "pot" for note exploration
        constrainEnc(10, 110);  //MIDI audible note values
        break;
      case 2: //encoder for ADSR
        constrainEnc(1, 127);  //MIDI ADSR values
        break;
      case 3: //encoder for channel select in ADSR & broadcast funcs
        constrainEnc(1, 8);  //# of MIDI channels
        break;
      case 4: //encoder for ADSR type select
        constrainEnc(1, 4);  //(1 = attack, 2 = decay, 3 = sustain, 4 = release)
        break;
      case 5: //encoder for pitchbend
        constrainEnc(0, 1);  // on /off for pitchbend
        break;
      default:
        break;
    }
    Serial.println(rtCounter);
    //OLED display.write("mode" + rtCounter);
  }
}
