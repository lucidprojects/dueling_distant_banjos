#include <CapacitiveSensor.h>
#include "encoder_mode_select.h"

// CAPACITIVE

// can calibration will run first - canCalibrationTime  eg first 10 seconds
// purple LED and set capacitance of can
// store capacitance of can just touching / holding it
int canCap;						// sensor object
int canCalibrationTime = 10000; // set calibration time for can
int canCapMax;
int canCapMin;

//initialize sensors
// add >= 300kΩ resistor between send pin (11) & receive pin (12)
// sensor wire connected to receive pin
// higher resistor more sensitve 1mΩ or > will make it almost too sensitve - capacitve readings just by hovering
// https://youtu.be/jco-uU5ZgEU?t=225  (more about capacitive send and receive)

// input to channel mapping
// b1: 1c  |  s1: 5c
// b2: 2c  |  s2: 6c   
// b3: 3c  |  s3: 7c   
// b4: 4c  |  s4: 8c

CapacitiveSensor slideSensor1 = CapacitiveSensor(12, 11);
CapacitiveSensor slideSensor2 = CapacitiveSensor(12, 10);
CapacitiveSensor slideSensor3 = CapacitiveSensor(9, 8);
CapacitiveSensor slideSensor4 = CapacitiveSensor(9, 7);

CapacitiveSensor btnSensor1 = CapacitiveSensor(6, 5);
CapacitiveSensor btnSensor2 = CapacitiveSensor(6, 4);
CapacitiveSensor btnSensor3 = CapacitiveSensor(14, 3);
CapacitiveSensor btnSensor4 = CapacitiveSensor(14, 2);

//CAP btn vars
int capBtnState[] = {LOW, LOW, LOW, LOW};
const int N_CAPBTNS = 4;
int capBtns[N_CAPBTNS];
int lastCapBtnState[] = {LOW, LOW, LOW, LOW};

//CAP slide vars
int capSlideState[] = {LOW, LOW, LOW, LOW};
const int N_CAPSLIDES = 4;
int capSlides[N_CAPSLIDES];
int lastCapSlideState[] = {LOW, LOW, LOW, LOW};
int slideNoteVal[N_CAPSLIDES];
int slideNote[N_CAPSLIDES];

//CAP calibration
int calibrationTime = 4000; // set calibration time per sensor
bool calComplete = false;	// boolean to exit calibration while loop on completion

// combined array for all sensor calibration - not exactly sure what to do here.  starting combined
// take note of order index assignment in readSensors() so iteration in separate arrays works - e.g. capSlides need be set first
const int N_ALL_CAPSENS = 8;   // set this to total # of sensors (butons & slides)
int allCapSens[N_ALL_CAPSENS]; // sensor vars in an array so we can iterate through in for loop

int sensorValue[N_ALL_CAPSENS];	   // the sensor value
int sensorMin[N_ALL_CAPSENS];	   // minimum sensor value
int sensorMax[N_ALL_CAPSENS];	   // maximum sensor value
int sensorValueRaw[N_ALL_CAPSENS]; // raw maximum sensor value - for debuging in loop

int sensorMin_length;
int sensorMax_length;

int sensorMinMIN, sensorMaxMAX; // min max values after calibration
int sensorMinBuffer = 600;		// buffer to prevent sensors from reading too low or 0

// SCALE

// the intervals in a major and natural minor scale:
int major[] = {2, 2, 1, 2, 2, 2, 1};
int naturalMinor[] = {2, 1, 2, 2, 1, 2, 2};

// an array to hold the final notes of the scale:
int scale[8];

// start with middle C:
int tonic = pitchC4;

// note to play:
int scaleValue = tonic;

//LED
int red = 17;	//this sets the red led pin
int green = 18; //this sets the green led pin
int blue = 19;	//this sets the blue led pin

int ledVal = 0;

// CALIBRATION FUNCTIONS

// read sensor values
void readSensors()
{
	allCapSens[0] = slideSensor1.capacitiveSensor(30);
	allCapSens[1] = slideSensor2.capacitiveSensor(30);
	allCapSens[2] = slideSensor3.capacitiveSensor(30);
	allCapSens[3] = slideSensor4.capacitiveSensor(30);
	allCapSens[4] = btnSensor1.capacitiveSensor(30);
	allCapSens[5] = btnSensor2.capacitiveSensor(30);
	allCapSens[6] = btnSensor3.capacitiveSensor(30);
	allCapSens[7] = btnSensor4.capacitiveSensor(30);
}

// used to set can capacitance. used in various thresholds throughout program
void canCalibration()
{
	// purple LED to denote can calibration mode
	analogWrite(red, 80);
	analogWrite(green, 0);
	analogWrite(blue, 80);

	int canCapMillisNow = millis();

	while (millis() < (canCapMillisNow + canCalibrationTime))
	{
		Serial.println("check can capacitance");

		canCap = slideSensor1.capacitiveSensor(30);

		for (int c = 0; c < 5; c++)
		{
			canCap = slideSensor1.capacitiveSensor(30);

			// record the maximum sensor value
			if (canCap > canCapMax)
			{
				canCapMax = canCap;
			}

			// record the minimum sensor value
			if (canCap < canCapMin)
			{
				canCapMin = canCap;
			}

			if (canCap > 20)
			{
				Serial.print("canCap = ");
				Serial.println(canCap);
			}
		}
	}
}

void setLedVal(int myLedVal)
{
	if (sensorValueRaw[myLedVal] > 400)
	{
		ledVal = sensorValue[myLedVal];
	}

	if (ledVal > 220)
	{
		digitalWrite(green, LOW);
		digitalWrite(red, HIGH);
		digitalWrite(blue, HIGH);
	}
	else
	{
		analogWrite(green, ledVal);
		analogWrite(red, ledVal);
		digitalWrite(blue, HIGH);
	}
}

// read/show cap values adjust led usd in loop()
// this is mostly for debugging and can be removed / commented out in final code - js 20200421
void capCalibration_Debug()
{
	// read the sensors:
	readSensors();

	for (int c = 0; c < N_ALL_CAPSENS; c++)
	{
		sensorValue[c] = allCapSens[c];
		sensorValueRaw[c] = allCapSens[c];

		// apply the calibration to the sensor reading
		sensorValue[c] = map(sensorValue[c], sensorMin[c], sensorMax[c], 0, 255);

		// in case the sensor value is outside the range seen during calibration // this is for LED
		sensorValue[c] = constrain(sensorValue[c], 0, 255);

		// set ledVals fade from yellow to green based on calibrated reading
		setLedVal(c);

		int capDebugVals;

		if (canCapMax > 100)
		{
			capDebugVals = canCapMax + sensorMinBuffer;
		}
		else
		{
			capDebugVals = 400;
		}

		//debugging - show values when sensor > 400
		if (allCapSens[c] > capDebugVals)
		{
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
int getMinMax(int *array, int size)
{
	int sensorMinMIN = array[0];
	for (int i = 0; i < size; i++)
	{
		if (array[i] < sensorMinMIN)
		{
			sensorMinMIN = array[i];
		}
	}

	int tempMinimum = sensorMinMIN;
	for (int j = 0; j < size; j++)
	{
		if (array[j] > tempMinimum)
		{
			sensorMaxMAX = array[j];
		}
	}

	// sets maximum in above for loop but returns minimum to prevent 0 value min
	return sensorMinMIN;
}

// used in setup() for calibration
void setMinMaxVals(int myVar)
{
	readSensors();

	sensorValue[myVar] = allCapSens[myVar]; // take sensor X reading

	// record the maximum sensor value
	if (sensorValue[myVar] > sensorMax[myVar])
	{
		sensorMax[myVar] = sensorValue[myVar];
	}

	// record the minimum sensor value
	if (sensorValue[myVar] < sensorMin[myVar])
	{
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

// main calibration fn used in setup()
void runSensorCalibration()
{
	while (calComplete == false)
	{
		// take sensor readings again - more consistent calibration readings
		readSensors();

		// iterate through sensors
		for (int c = 0; c < N_ALL_CAPSENS; c++)
		{
			sensorMin[c] = 1023; // initial minimum sensor value
			sensorMax[c] = 0;	 // initial maximum sensor value

			// do this to only go through the loop once per number of sensors
			if (c < N_ALL_CAPSENS)
			{

				digitalWrite(green, HIGH);
				digitalWrite(red, HIGH);
				digitalWrite(blue, LOW);

				int millisNow = millis();

				// 3000 = take calibration for 3 seconds - set in calibrationTime var
				while (millis() < (millisNow + calibrationTime))
				{
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

		calComplete = true; // after iterating though # sensors
	}

	if (calComplete == true)
	{
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

void checkCapBtnVals(int myVar)
{
	// threshold value is set by canCapMax (set by canCalibration() fn) + sensorMinBuffer. can be adjusted to dial in sensitivity
	if (capBtns[myVar] > canCapMax + sensorMinBuffer)
	{
		capBtnState[myVar] = HIGH;
	}
	else
	{
		capBtnState[myVar] = LOW;
	}
}

void checkCapSlideVals(int myVar)
{
	// threshold value is set by canCapMax (set by canCalibration() fn) + sensorMinBuffer. can be adjusted to dial in sensitivity
	if (capSlides[myVar] > canCapMax + sensorMinBuffer)
	{
		capSlideState[myVar] = HIGH;

		// slide sensors need to always be first in the shared N_ALL_CAPSENS array
		// so we can iterate properly in the N_CAPSLIDES array
		// and properlly set min / max vals
		slideNoteVal[myVar] = map(capSlides[myVar], sensorMin[myVar], sensorMax[myVar], 0, 90); // these values return from calibration fn
	}
	else
	{
		capSlideState[myVar] = LOW;
	}
}

void handleCapSlides()
{
	capSlides[0] = slideSensor1.capacitiveSensor(30);
	capSlides[1] = slideSensor2.capacitiveSensor(30);
	capSlides[2] = slideSensor3.capacitiveSensor(30);
	capSlides[3] = slideSensor4.capacitiveSensor(30);

	for (int b = 0; b < N_CAPSLIDES; b++)
	{
		checkCapSlideVals(b);
	}

	for (int c = 0; c < N_CAPSLIDES; c++)
	{

		if (asSlides)
		{
			slideNote[c] = slideNoteVal[c];
			slideNote[c] = constrain(slideNote[c], 10, 90);
		}
		else
			slideNote[c] = baseNote;

		int capSlideStateVal = capSlideState[c];

		if (capSlideStateVal == HIGH)
		{
			Serial.println("turn midi slide signal on");

			// all cap inputs play scale
			if (doScale == true)
			{
				// midiOn(8 - c, scale[4+c]);
				midiOn(9, scale[4 + c]); // send scale notes all on same channel
				Serial.print("Sending scale note scale[");
				Serial.print(c);
				Serial.println("]");
			}

			// all cap inputs play scale
			if (doScaleMod == true)
			{
				pitchBend = map(slideNoteVal[c], 10, 90, 0, 16383); //constrain values could be adjusted here
				pitchBend = pitchBend << 1;							// shift low byte's msb to high byte
				msb = highByte(pitchBend);							// get the high bits
				lsb = lowByte(pitchBend) >> 1;						// get the low 8 bits

				// print the values, for reference:
				Serial.print("msb = ");
				Serial.print(msb);
				Serial.print(" lsb = ");
				Serial.println(lsb);

				// send the pitch bend message:
				midiCommand(0xE8, lsb, msb, false); // pitchbend weirdness - first value is import here
			}
			else
			{
				midiOn(8 - c, slideNote[c]); // the first value sent in the fn will need to be adjusted for final channel assignment
			}
		}
		else
		{
			// midiOff(8 - c, slideNote[c]);
			// Serial.println("turn midi slide signal off");  // never turning it off helps slide effect
			capSlideState[c] = LOW;
		}

		delay(10);
	}
}

void handleCapBtns()
{
	capBtns[0] = btnSensor1.capacitiveSensor(30);
	capBtns[1] = btnSensor2.capacitiveSensor(30);
	capBtns[2] = btnSensor3.capacitiveSensor(30);
	capBtns[3] = btnSensor4.capacitiveSensor(30);

	for (int b = 0; b < N_CAPBTNS; b++)
	{
		checkCapBtnVals(b);
	}

	for (int c = 0; c < N_CAPBTNS; c++)
	{

		int capBtnStateVal = capBtnState[c];

		if (capBtnStateVal != lastCapBtnState[c])
		{
			if (capBtnStateVal == HIGH)
			{

				// all cap inputs play scale
				if (doScale == true || doScaleMod == true)
				{
					//midiOn(4 - c, scale[c]);
					midiOn(9, scale[c]); // send scale notes all on same channel
					Serial.print("Sending scale note scale[");
					Serial.print(c);
					Serial.println("]");
					myScaleNote = scale[c];
				}
				else
				{
					midiOn(4 - c, baseNote); // the first value sent in the fn will need to be adjusted for final channel assignment
				}
			}
			else
			{
				if (doScale == true || doScaleMod == true)
				{
					midiOff(9, baseNote);
				}
				else
				{
					midiOff(4 - c, baseNote); // the first value sent in the fn will need to be adjusted for final channel assignment
				}

				Serial.println("turn midi signal off");
				capBtnState[c] = LOW;
			}

			lastCapBtnState[c] = capBtnStateVal;
			delay(10);
		}
	}
}

void initCapacitive(){
	// SCALE STUFF
	// fill the scale array with the scale you want:
	// start with the initial note:
	scale[0] = tonic;
	int note = scale[0];

	// iterate over the intervals, adding each to the next note
	// in the scale. You can change major to naturalMinor
	// if you want that kind of scale instead:
	for (int n = 0; n < 7; n++)
	{
		// note = note + naturalMinor[n];
		note = note + major[n];
		scale[n + 1] = note;
	}

	// run callibration
	runSensorCalibration();
}