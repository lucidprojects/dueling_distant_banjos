#include <Encoder.h>

#include "button_multipress.h"

Encoder knob(A6, A7); // initialize encoder

//const int buttonPin = 5; // defining rotary button pin in button_multipress.h
int lastKnobState = 0; // set last knob state
int rtCounter = 0;	   // incremet rtCounter for accurate counts instead of encoder variance.
int lastRtCounter = 0;

int isPot = 0;
int b;

int channel;	  //store channel value
int savedChannel; //preserve channel when changing octave

bool selectCh = false;
bool asSlides = true;
bool doScale = false;
bool doRecord = false;
bool doBroadCast = false;
bool doScaleMod = false;
bool asChords = false;
bool chordsOctave = false;
bool firstOctaveChange = true;
bool octaveChanged = false;

// constrain encoder values
void constrainEnc(int minE, int maxE)
{
	rtCounter = constrain(rtCounter, minE, maxE);
}

// eneable channel select in the loop
void selectChannel()
{
	if (b == 1)
	{
		b = 0; //set b = 0 to prevent click entering mode
	}

	selectCh = true; // set mode bool to true for handling in loop
}


// use slides as slides or notes
void slidesOrNotes()
{
	asSlides = !asSlides;
	Serial.println(asSlides);
}

// use cap buttons a scales
void capsAsScales()
{
	doScale = !doScale;
	Serial.println(doScale);
}

void doRecordLoop()
{
	if (b == 1)
	{
		b = 0; //set b = 0 to prevent click entering mode
	}

	doRecord = true; // set mode bool to true for handling in loop
	Serial.println(doRecord);
}

void broadCast()
{
	//forward midi cmd per channel to receiver.
	//need to set channel to pass somehow.
	if (b == 1)
	{
		b = 0; //set b = 0 to prevent click entering mode
	}
	doBroadCast = true; // set mode bool to true for handling in loop
}

void modScale()
{
	//  Mod scale - 4btns as scales sliders modify btns
	doScaleMod = !doScaleMod; // toggle scale mod on and off
	Serial.println(doScaleMod);
}

void modChords()
{
	//  Mod chords - 4btns as chordss sliders modify btns
	asChords = true; // toggle scale mod on and off
	//  Serial.println("asChords = ");
	//  Serial.println(asChords);
}

void setMode(int myMode)
{
	/*
   		NOTE: MODES
		0) explore 
			- different instruments all 8 channels
		1) ch select
		2) chords (use ch select)  
			-  w pitch bend
		3) scales (use ch select) (all 8 cap sensors) 
			- TODO: need to figure out why slides play scale & explore???
		4) scales (use ch select)  
			- w pitch bend
		5) slides as slides 
			- defaults to slides (1)
		6) pot mode
		7) WARN: NOT DOING 
			- volume adjust per channel - to play with things on and off
		8) broadcast
  */

	Serial.print("set mode = ");
	Serial.println(myMode);

	switch (myMode)
	{
	case 0: // set default encoderMode
      //OLED display.write("select mode");
      isPot = 0;
      selectCh = false;
      doScaleMod = false;
      asSlides = true;
      doScale = false;
      asChords = false;
      break;
    case 1: // channel select mode;
      isPot = 2;
      Serial.println("channel select mode");
      //OLED display.write("select channel");
      selectChannel();
      break;
    case 2: // Play chords - 4btns as chords sliders modify btns
      Serial.println("chord mode w/ pitch bend");
      //OLED display.write(""Chord mode");
      isPot = 6;
      modChords();
      break;
    case 3: // All cap inputs as scale buttons mode
      Serial.println("All cap inputs as scale buttons mode");
      //OLED display.write("asCaps" + asCaps");
      capsAsScales();
      break;
    case 4: // Mod scale - 4btns as scales sliders modify btns
      Serial.println("Mod Scale mode");
      isPot = 3;
      //OLED display.write(""Mod Scale mode");
      modScale();
      break;
    case 5: // Slides or notes mode
      Serial.println("Slides or notes mode");
      //OLED display.write("asSlides" + asSlides");
      slidesOrNotes();
      break;
    case 6: // Explore mode - encoder as pot
      Serial.println("Explore mode enc as pot");
      isPot = 1;
      //OLED display.write("Explore mode");
      break;
   case 7: // volume adjust per channel
      Serial.println("volume adjust X CHANNEL");
      isPot = 5;
      //OLED display.write(""volume adjust channel" + channel);
     // volumeAdjust();
      break;
   case 8: // Broadcast X CHANNEL
      Serial.println("Broadcast X CHANNEL");
      isPot = 2;
      //OLED display.write("broadcast channel" + channel);
      broadCast();
      break;    
    default:
      break;
	}
}

void encoderButton(int b)
{

	switch (b)
	{
	case 0:
		break;
	case 1:
		Serial.println("single click");

		if (isPot == 0)
		{
			setMode(rtCounter);
		}

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

int readEnc(int encMode)
{
	// Tom's encoder code
	// checks for absolute value to check for forward or back movement
	// incrementing rtCounter for better accuracy from the encoder
	int knobState = knob.read();
	int knobChange = knobState - lastKnobState;

	// if it's changed by 4 or more pulses:
	if (abs(knobChange) >= 4)
	{
		//want direction of change, not value, so
		// divide value by abs. value to get 1 or -1:
		knobChange = knobChange / abs(knobChange);

    // save the current state for comparison next time:
    lastKnobState = knobState;
    
		// if knobChange == 1, turned up/right rtCounter++, else rtCounter--
		if (knobChange == 1)
		{
			rtCounter++;
		}
		else
		{
			rtCounter--;
		}

//		// save the current state for comparison next time:
//		lastKnobState = knobState;

		// handle whether encoder acts as encoder or "potentiometer" and the range
		switch (encMode)
		{
		case 0: //default - mode select encoder
			if (rtCounter < 1)
			{
				rtCounter = 8;
			}

			if (rtCounter > 8)
			{
				rtCounter = 1;
			}

			break;
		case 1:	// encoder as "pot" for note exploration
			constrainEnc(10, 110); // MIDI audible note values
			break;
		case 2:	// encoder for channel select 
			constrainEnc(1, 9); // # of MIDI channels
			break;
		case 3:	// encoder for pitchbend
			constrainEnc(0, 1); // on/off for pitchbend
			break;
		case 4:	//encoder for chord octave switching
			constrainEnc(0, 6); // n * 12 to change 7 octaves
			break;
		case 5:	//encoder for volume adjustment
			constrainEnc(1, 127); //  changes volume 1-127
			break;
		case 6:	//encoder for chord channels 0 - 9 (1-9)
			constrainEnc(1, 9); //   channels 0 - 9 (1-9)
			break;
		default:
			break;
		}
		// Serial.print("rtCounter from readEnc() = ");
		
		// if (asChords == true && chordsOctave == false)
		// {
		// 	Serial.println(rtCounter);
		// }
		// else
		// {
		// 	Serial.println(rtCounter);
		// }

		Serial.println(rtCounter);
		//OLED display.write("mode" + rtCounter);
	}
}
