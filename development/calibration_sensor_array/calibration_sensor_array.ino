/*
  Modified Sensor Calibration
  created 29 Oct 2008
  By David A Mellis
  modified 30 Aug 2011
  By Tom Igoe
  modified 20 April 2020
  By Jake Sherwood

  http://arduino.cc/en/Tutorial/Calibration

  Modified to iterate through an array of sensors and calibrate each sensor.  Storing values in an array of mins/maxes.
  • Can calibration runs first setting can capacitance.
  • Set calibration time per sensor in calibrationTime var, used in runSensorCalibration() fn
  • During calirbration tap and release sensor to set min & max values
  • Uses RGB LED to denote different calibration modes:
    Starts in yellow. blue calibrating, toggle red switching to next sensor back to blue. solid red when calibration complete
    Short green when completing setup()
    Yellow to start loop()
*/

#include <CapacitiveSensor.h>

int red = A3; //this sets the red led pin
int green = A4 ; //this sets the green led pin
int blue = A5 ; //this sets the green led pin

int ledVal = 0;

//CAPACITIVE

// can calibration will run first - canCalibrationTime  eg first 10 seconds
// purple LED and set capacitance of can
// store capacitance of can just touching / holding it 
int canCap; // sensor object
int canCalibrationTime = 10000;  // set calibration time for can
int canCapMax; 
int canCapMin;


const int N_CAPBTNS = 4;  // set this to total # of sensors
int capBtns[N_CAPBTNS];   // sensor vars in an array so we can iterate through in for loop

int sensorValue[N_CAPBTNS]; // the sensor value
int sensorMin[N_CAPBTNS];   // minimum sensor value
int sensorMax[N_CAPBTNS];   // maximum sensor value
int sensorValueRaw[N_CAPBTNS]; // raw maximum sensor value - for debuging in loop

int calibrationTime = 3000;  // set calibration time per sensor
bool calComplete = false; // boolean to exit calibration while loop on completion

//initialize senors
// add >= 300kΩ resistor between send pin (11) & receive pin (12)
// sensor wire connected to receive pin
// high resistor more sensitve 1mΩ or < will make it capacitive by hovering
// https://youtu.be/jco-uU5ZgEU?t=225  (more about capacitive send and receive)

CapacitiveSensor btnSensor0 = CapacitiveSensor(11, 12);
CapacitiveSensor btnSensor1 = CapacitiveSensor(11, 10);
CapacitiveSensor btnSensor2 = CapacitiveSensor(8, 9);
CapacitiveSensor btnSensor3 = CapacitiveSensor(6, 7);


void setup() {
  Serial.begin(9600);

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

  Serial.print("canCapMax = ");
  Serial.println(canCapMax);
}

void loop() {
  
  handleCapSensors();


}

// read sensor values
void readSensors() {
  capBtns[0] =  btnSensor0.capacitiveSensor(30);
  capBtns[1] =  btnSensor1.capacitiveSensor(30);
  capBtns[2] =  btnSensor2.capacitiveSensor(30);
  capBtns[3] =  btnSensor3.capacitiveSensor(30);
}




// handle cap values in loop()
void handleCapSensors() {
  // read the sensors:
  readSensors();

  for (int c = 0; c < N_CAPBTNS; c++) {

    sensorValue[c] = capBtns[c];
    sensorValueRaw[c] = capBtns[c];

    // apply the calibration to the sensor reading
    sensorValue[c] = map(sensorValue[c], sensorMin[c], sensorMax[c], 0, 255);

    // in case the sensor value is outside the range seen during calibration // this is for LED
    sensorValue[c] = constrain(sensorValue[c], 0, 255);

    // set ledVals fade from yellow to green based on calibrated reading
    setLedVal(c);

    //debugging - show values when sensor > 400
    if (capBtns[c] > 400) {

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



// used to set can capacitance. used in vaious thresholds throughout program
void canCalibration() {

  // purple LED to denote can calibration mode
  analogWrite(red, 80);
  analogWrite(green, 0);
  analogWrite(blue, 80);

  int canCapMillisNow  = millis();

  while (millis() < (canCapMillisNow + canCalibrationTime)) {
    Serial.println("check can capacitance");

    canCap = btnSensor0.capacitiveSensor(30);

    for (int c = 0; c < 5; c++) {

      canCap = btnSensor0.capacitiveSensor(30);
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




// main calibration fn used in setup()
void runSensorCalibration() {

  while (calComplete == false) {
    // take sensor readings again - more consistent calibration readings
    readSensors();

    for (int c = 0; c < N_CAPBTNS; c++) { // iterate through sensors

      sensorMin[c] = 1023;        // initial minimum sensor value
      sensorMax[c] = 0;           // initial maximum sensor value

      if (c < N_CAPBTNS) {  // do this to only go through the loop once per number of sensors

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
    calComplete = true;  // after iterating though # sensors

  }

  if (calComplete == true) {
    Serial.print("end of the array with length = ");
    Serial.println(N_CAPBTNS);
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

  sensorValue[myVar] = capBtns[myVar];  // take sensor X reading

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