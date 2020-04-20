
/* this is just testing 2 capactive buttons turning a LED on and off */

#include <CapacitiveSensor.h>

//int led = 2;      //use for single LED

int red = A3; //this sets the red led pin
int green = A4 ; //this sets the green led pin
int blue = A5 ; //this sets the green led pin

long doLED = 0;
int state = HIGH;

int capState[] = {LOW, LOW};
int newVal;
int capStateBtn;

boolean yes;
boolean previous = false;


int debounce = 200;

CapacitiveSensor   cbtns1 = CapacitiveSensor(8, 9);
CapacitiveSensor   cbtns2 = CapacitiveSensor(6, 7);


const int N_CAPPINS = 2;
int capBtns [N_CAPPINS];

int lastcapState[] = {LOW, LOW};

void setup() {

  Serial.begin(9600);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  //pinMode(led, OUTPUT);

  cbtns1.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... pinMode(led, OUTPUT);
  cbtns2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... pinMode(led, OUTPUT);

}

void loop()
{


  handleCapBtns();


}



void handleCapBtns() {

  capBtns[0] =  cbtns1.capacitiveSensor(30);
  capBtns[1] =  cbtns2.capacitiveSensor(30);

  for (int d = 0; d < N_CAPPINS; d++) {
    checkCapVals(d);

  }



  for (int c = 0; c <  N_CAPPINS; c++) {
    //    Serial.println(c);
    //    Serial.print("starting capState[");
    //    Serial.print(c);
    //    Serial.print("] =");
    //    Serial.print(capState[c]);
    //    Serial.print("\t");
    //    Serial.print("yes");
    //    Serial.print("\t");
    //    Serial.print(yes);
    //
    //    Serial.print("\t");
    //    Serial.print("newVal");
    //    Serial.print("\t");
    //    Serial.print(newVal);
    //
    //    Serial.print("\t");
    //    Serial.print("previous");
    //    Serial.print("\t");
    //    Serial.println(previous);



    int capStateVal = capState[c];

    //
    //Serial.println(capStateVal);


    if (capStateVal != lastcapState[c])
    {
      if (capStateVal == HIGH)
      {
        Serial.println("turn midi signal on");
        //midiOn(k + 1, baseNote);
        toggleLED(HIGH);
      }
      else
      {
        // midiOff(k + 1, baseNote);

        Serial.println("turn midi signal off");
        capState[c] = LOW;
        // toggleLED(HIGH);
      }

      lastcapState[c] = capStateVal;
      delay(10);
    }
  }

}


void checkCapVals(int myVar) {
  if (capBtns[myVar] > 1100) {
    capState[myVar] = HIGH;
  } else {
    capState[myVar] = LOW;
  }


}


void toggleLED(int x) {

  int capLedStateVal = x;

  if (capLedStateVal == HIGH) {
    doLED == true;
  } else {
    doLED == false;
  }


//  Serial.print("capLedStateVal");
//  Serial.print("\t");
//  Serial.print(capLedStateVal);
//
//  Serial.print("\t");
//  Serial.print("previous");
//  Serial.print("\t");
//  Serial.println(previous);


  if (state == HIGH)
  {
    state = LOW;
//    Serial.println("state = LOW");
  }
  else
  {
    state = HIGH;
//    Serial.println("state = HIGH");
  }

  previous = doLED;
  delay(10);

  digitalWrite(green, state);
  digitalWrite(red, HIGH);
  digitalWrite(blue, HIGH);



}
