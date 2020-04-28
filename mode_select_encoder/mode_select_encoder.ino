#include <Encoder.h>
#include "button_multipress.h"

//const int buttonPin = 5; // pushbutton is on pin 5  // defining rotary button pin in button_multipress.h
Encoder knob(3, 4);     // initialize encoder on pins 0 and 1
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
bool asCaps = true;
bool recordLoop = false;
bool doBroadCast = false;

bool fwd = false;


void setup() {
  Serial.begin(9600);
  pinMode (buttonPin, INPUT_PULLUP);

}

void loop() {

  if (isPot < 1) readEnc(0);
  else readEnc(isPot);

  // handle button press types
  b = checkButton();
  encoderButton(b);

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
      // setMode(0);
    }
  }

  //broadcast

  if (doBroadCast == true && b == 1 ) {
    fwd = true;
    Serial.println(fwd);
    channel = rtCounter;
    //OLED display.write("Channel" + channel);
    Serial.print("broadcast channel = ");
    Serial.println(channel);
    doBroadCast = false;
    setMode(0);
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

void setMode(int myMode) {
  /*
    MODES
    0)DEFAULT ENCODER MODE - DONE
    1)CHANNEL SELECT - DONE
    2)ADSR - DONE  **need to integrate with cap_can**
    3)ACT AS POT note shifter - DONE  **need to integrate with cap_can**
    4)SLIDES as slides or notes - DONE **need to integrate with cap_can**
    5)All cap inputs as scale buttons? - DONE **need to integrate with cap_can**
    6)Record / loop  - no MIDI cmds I can find for this - tried Logix key assignment but doesn't seem to be working.
    7)Broadcast n CHANNEL - DONE **need to integrate with cap_can**
  */

  Serial.print("set mode = ");
  Serial.println(myMode);

  switch (myMode) {
    case 0: // set default encoderMode
       //OLED display.write("select mode");
      isPot = 0;
      selectCh = false;
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
      //recordLoop();
      break;
    case 7: // Broadcast X CHANNEL
      Serial.println("Broadcast X CHANNEL");
      isPot = 3;
      //OLED display.write("broadcast channel" + channel);
      broadCast();
      break;
    default:
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
        if (rtCounter < 1) rtCounter = 7;
        if (rtCounter > 7) rtCounter = 1;
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
      default:
        break;
    }
    Serial.println(rtCounter);
    //OLED display.write("mode" + rtCounter);
  }
}

void constrainEnc(int minE, int maxE) {
  rtCounter = constrain(rtCounter, minE, maxE);
}

void selectChannel() {
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  selectCh = true;   // set mode bool to true for handling in loop
}

void ADSR() {
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  selectADSR = true; // set mode bool to true for handling in loop
}

void slidesOrNotes() {
  asSlides = !asSlides;
  Serial.println(asSlides);
}

void capsAsScales() {
  asCaps = !asCaps;
  Serial.println(asCaps);
}


//void recordLoop(){}

void broadCast() {
  //forward midi cmd per channel to receiver.
  //need to set channel to pass somehow.
  if (b == 1) b = 0; //set b = 0 to prevent click entering mode
  doBroadCast = true; // set mode bool to true for handling in loop
  
}
