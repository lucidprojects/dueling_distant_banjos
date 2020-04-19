
/* this is just testing 2 capactive buttons turning a LED on and off */

#include <CapacitiveSensor.h>

//int led = 2;      //use for single LED

int red = A3; //this sets the red led pin
int green = A4 ; //this sets the green led pin
int blue = A5 ; //this sets the green led pin

long time = 0;
int state = LOW;

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

int lastcapState[N_CAPPINS];

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


  //  log cap values
  //    Serial.print("yes");
  //    Serial.print("\t");
  //    Serial.print(yes);
  //    Serial.print("\t");
  //    Serial.print("capBtns[0]");
  //    Serial.print("\t");
  //    Serial.print(capBtns[0]);
  //    Serial.print("\t");
  //    Serial.print("capBtns[1]");
  //    Serial.print("\t");
  //    Serial.println(capBtns[1]);

  capBtns[0] =  cbtns1.capacitiveSensor(30);
  capBtns[1] =  cbtns2.capacitiveSensor(30);

  for (int c = 0; c <  N_CAPPINS; c++) {
    //    Serial.println(c);
    Serial.print("starting capState[");
    Serial.print(c);
    Serial.print("] =");
    Serial.print(capState[c]);
    Serial.print("\t");
    Serial.print("yes");
    Serial.print("\t");
    Serial.print(yes);

    Serial.print("\t");
    Serial.print("newVal");
    Serial.print("\t");
    Serial.print(newVal);

    Serial.print("\t");
    Serial.print("previous");
    Serial.print("\t");
    Serial.println(previous);

    for (int d = 0; d < N_CAPPINS; d++) {
      checkCapVals(d);

    }




//        if (capState[c] != lastcapState[c])
    if (capState[c] != newVal)
// if (capState[c]= !capState[c])


      
    {

  

      if (capState[c] == HIGH)
      {
        Serial.println( capBtns[c]);
        Serial.print("midiOn(");
        Serial.print(c);
        Serial.println("+ 4, baseNote)");
        yes = true;
      }
      else
      {
        Serial.print("midiOff(");
        Serial.print(c);
        Serial.println("+ 4, baseNote)");
        yes = false;

      }

      
      capState[c] = newVal;
      //      lastcapState[c] = capState;
      Serial.print("after loop capState[");
      Serial.print(c);
      Serial.print("] =");
      Serial.println(capState[c]);

//      lastcapState[c] = capState[c];
      delay(100);
    }

  }

    //
    //  if (capBtns[0] > 1100) {
    //    yes = true;
    //  } else if (capBtns[1] > 1100) {
    //    yes = true;
    //  } else {
    //    yes = false;
    //  }


    // to toggle the state of state
    if (yes == true && previous  == false && millis() - time > debounce) {

      if (state == HIGH) {
        state = LOW;
      }
      else {
        state = HIGH;
      time = millis();

    }

    digitalWrite(green, state);
    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);

    //digitalWrite(led, state);

    previous = yes;

    //  Serial.println(millis() - time);
    delay(10);




  }


}


  void checkCapVals(int myVar) {
    if (capBtns[myVar] > 1100) {
      capState[myVar] = HIGH;
    } else {
      capState[myVar] = LOW;
    }


  }
