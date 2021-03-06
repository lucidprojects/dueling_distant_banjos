
/* 4-Way Button:  Click, Double-Click, Press+Hold, and Press+Long-Hold Test Sketch

  By Jeff Saltzman
  Oct. 13, 2009

  http://forum.arduino.cc/index.php?topic=14479.0

  To keep a physical interface as simple as possible, this sketch demonstrates generating four output events from a single push-button.
  1) Click:  rapid press and release
  2) Double-Click:  two clicks in quick succession
  3) Press and Hold:  holding the button down
  4) Long Press and Hold:  holding the button for a long time
*/


/* modified 20200707 to work for capacitive buttons by jake sherwood */

//#define buttonPin 7        // digital input pin

// ALL CAP sensor vars - moved from capacitve_touch.h - 20200708 js
const int N_ALL_CAPSENS = 8;   // set this to total # of sensors (buttons & slides)
int allCapSens[N_ALL_CAPSENS]; // sensor vars in an array so we can iterate through in for loop

int allCapsState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
int lastAllCapsState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};


// Button timing variables
int capDebounce = 50;          // ms debounce period to prevent flickering when pressing or releasing the button
int capDCgap = 500;            // max ms between clicks for a double click event
int capHoldTime = 2000;        // ms hold period: how long to wait for press+hold event
int capLongHoldTime = 5000;    // ms long hold period: how long to wait for press+hold event

// Button variables
//boolean capButtonVal = LOW;   // value read from button
//boolean capButtonLast = LOW;  // buffered value of the button's previous state
//boolean capDCwaiting = false;  // whether we're waiting for a double click (down)
//boolean capDConUp = false;     // whether to register a double click on next release, or whether to wait and click
//boolean capSingleOK = true;    // whether it's OK to do a single click
//long capDownTime = -1;         // time the button was pressed down
//long capUpTime = -1;           // time the button was released
//boolean capIgnoreUp = false;   // whether to ignore the button release because the click+hold was triggered
//boolean capWaitForUp = false;        // when held, whether to wait for the up event
//boolean capHoldEventPast = false;    // whether or not the hold event happened already
//boolean capLongHoldEventPast = false;// whether or not the long hold event happened already



boolean capButtonVal[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
boolean capButtonLast[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
//boolean capButtonVal = LOW;   // value read from button
//boolean capButtonLast = LOW;  // buffered value of the button's previous state
boolean capDCwaiting[] = {false, false, false, false, false, false, false, false};  // whether we're waiting for a double click (down)
boolean capDConUp[] = {false, false, false, false, false, false, false, false};    // whether to register a double click on next release, or whether to wait and click
boolean capSingleOK[] = {true, true, true, true, true, true, true, true};    // whether it's OK to do a single click
long capDownTime = -1;         // time the button was pressed down
long capUpTime = -1;           // time the button was released
boolean capIgnoreUp[] = {false, false, false, false, false, false, false, false};    // whether to ignore the button release because the click+hold was triggered
boolean capWaitForUp[] = {false, false, false, false, false, false, false, false};        // when held, whether to wait for the up event
boolean capHoldEventPast[] = {false, false, false, false, false, false, false, false};     // whether or not the hold event happened already
boolean capLongHoldEventPast[] = {false, false, false, false, false, false, false, false};// whether or not the long hold event happened already




int checkCapButton(int sensorVar) {
  int event = 0;

//  for (int g = 0; g < N_ALL_CAPSENS; g++)
//  {
    capButtonVal[sensorVar] = allCapsState[sensorVar];
//    Serial.print(allCapsState[g]);
//    Serial.print(", ");
//  }
//  Serial.println("i");

//  for (int i = 0; i < N_ALL_CAPSENS; i++)
//  {
    
//    Serial.println(i);
    // capButtonVal = capState;
    // Button pressed down
    if (capButtonVal[sensorVar] == HIGH && capButtonLast[sensorVar] == LOW && (millis() - capUpTime) > capDebounce)
    {
      capDownTime = millis();
      capIgnoreUp[sensorVar] = false;
      capWaitForUp[sensorVar] = false;
      capSingleOK[sensorVar] = true;
      capHoldEventPast[sensorVar] = false;
      capLongHoldEventPast[sensorVar] = false;
      if ((millis() - capUpTime) < capDCgap && capDConUp[sensorVar] == false && capDCwaiting[sensorVar] == true)  capDConUp[sensorVar] = true;
      else  capDConUp[sensorVar] = false;
      capDCwaiting[sensorVar] = false;
    }
    // Button released
    //   else if (capButtonVal[sensorVar] == HIGH && capButtonLast[sensorVar] == LOW && (millis() - capDownTime) > capDebounce)
    else if (capButtonVal[sensorVar] == LOW && capButtonLast[sensorVar] == HIGH && (millis() - capDownTime) > capDebounce)
    {
      if (not capIgnoreUp[sensorVar])
      {
        capUpTime = millis();
        if (capDConUp[sensorVar] == false) capDCwaiting[sensorVar] = true;
        else
        {
          event = 2;
          capDConUp[sensorVar] = false;
          capDCwaiting[sensorVar] = false;
          capSingleOK[sensorVar] = false;
        }
      }
    }
    // Test for normal click event: DCgap expired
    //   if ( capButtonVal[sensorVar] == HIGH && (millis()-capUpTime) >= capDCgap && capDCwaiting == true && capDConUp == false && capSingleOK == true && event != 2)
    if ( capButtonVal[sensorVar] == LOW && (millis() - capUpTime) >= capDCgap && capDCwaiting[sensorVar] == true && capDConUp[sensorVar] == false && capSingleOK[sensorVar] == true && event != 2)
    {
      event = 1;
      capDCwaiting[sensorVar] = false;
    }
    // Test for hold
    // if (capButtonVal[sensorVar] == LOW && (millis() - capDownTime) >= capHoldTime) {
    if (capButtonVal[sensorVar] == HIGH && (millis() - capDownTime) >= capHoldTime) {
      // Trigger "normal" hold
      if (not capHoldEventPast[sensorVar])
      {
        event = 3;
        capWaitForUp[sensorVar] = true;
        capIgnoreUp[sensorVar] = true;
        capDConUp[sensorVar] = false;
        capDCwaiting[sensorVar] = false;
        //downTime = millis();
        capHoldEventPast[sensorVar] = true;
      }
      // Trigger "long" hold
      if ((millis() - capDownTime) >= capLongHoldTime)
      {
        if (not capLongHoldEventPast[sensorVar])
        {
          event = 4;
          capLongHoldEventPast[sensorVar] = true;
        }
      }
    }
    capButtonLast[sensorVar] = capButtonVal[sensorVar];
    
 // }
  return event;
}
