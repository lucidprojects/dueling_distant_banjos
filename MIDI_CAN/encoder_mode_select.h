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
bool chMute = false;
bool capChMute = false;
bool chVolume = false;
bool volAdjust = false;
bool capChVol = false;

int lastVolInc = 0;

//String modeText;


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
  //  drawSlidesONotes(asSlides);
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


void volumeAdjust() {
  volAdjust = true; // set mode bool to true for adjusting volume
}



void chMuteOnOff()
{
  chMute = !chMute;
  
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

void modeChSelect() {

  // isPot = 6;
  //  rtCounter = savedChannel + 1; // set rtCounter back to saved channel when entering back into chords play mode
  //  channel = savedChannel;
  Serial.print("modeChSelect on");

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
      //asSlides = true;
      doScale = false;
      asChords = false;
      chVolume = false;
      chMute = false;
      volAdjust = false;
      disMode = 0;
      drawMode(0, 0);
      drawModeSelect(true);
      drawSlidesONotes(asSlides);
      drawOctaveSelect(false);
      doBroadCast = false;
      rtCounter = 0;
      break;

    case 1: // Play chords - 4btns as chords sliders modify btns
      Serial.println("chord mode w/ pitch bend");
      //OLED display.write(""Chord mode");
      isPot = 6;
      modChords();
      disMode = 1;
      //      modeText = "Chords";
      drawMode(1, rtCounter);
      //      drawModeSelect(false);
      break;
    case 2: // All cap inputs as scale buttons mode
      Serial.println("All cap inputs as scale buttons mode");
      //OLED display.write("asCaps" + asCaps");
      isPot = 6;
      capsAsScales();
      disMode = 2;
      drawMode(2, rtCounter);
      //      drawModeSelect(false);
      modeChSelect();
      break;
    case 3: // Adjust channel volume ON/OFF
      Serial.println("Adjust channel mute ON/OFF");
      isPot = 6;  //isPot = 3; (pot 3 is 0 or 1, pot 6 is 1 - 9)
      //OLED display.write("CH volue" + chVolume);
      chMuteOnOff();
      disMode = 3;
      drawMode(3, rtCounter);
      rtCounter = 1;
      //      drawModeSelect(false);
      break;

    case 4: // Mod scale - 4btns as scales sliders modify btns
      Serial.println("Mod Scale mode");
      //      isPot = 3;
      //OLED display.write(""Mod Scale mode");
      isPot = 6;
      modScale();
      disMode = 4;
      drawMode(4, rtCounter);
      //      modeChSelect();
      //      drawModeSelect(false);
      break;
    case 5: // Slides or notes mode
      Serial.println("Slides or notes mode");
      //OLED display.write("asSlides" + asSlides");
      slidesOrNotes();
      disMode = 5;
      drawMode(5, rtCounter);
      drawSlidesONotes(asSlides);
      //      drawModeSelect(false);
      break;
    case 6: // channel select mode;
      isPot = 2;
      Serial.println("channel select mode");
      //OLED display.write("select channel");
      selectChannel();
      disMode = 6;
      //      modeText = "Ch Select";
      drawMode(6, rtCounter);
      drawModeSelect(false);

      break;
    case 7: // Explore mode - encoder as pot
      Serial.println("Explore mode enc as pot");
      isPot = 1;
      disMode = 7;
      drawMode(7, rtCounter);
      //      drawModeSelect(false);
      //OLED display.write("Explore mode");
      break;
    case 8: // volume adjust per channel
      Serial.println("volume adjust X CHANNEL");
      isPot = 5;
      disMode = 8;
      drawMode(8, rtCounter);
      //      drawModeSelect(false);
      //OLED display.write(""volume adjust channel" + channel);
      volumeAdjust();
      break;
    case 9: // Broadcast X CHANNEL
      Serial.println("Broadcast X CHANNEL");
      //      drawBroadcast(fwd[rtCounter - 1]);
      isPot = 2;
      //OLED display.write("broadcast channel" + channel);
      broadCast();
      disMode = 9;
      drawMode(9, rtCounter);
      //      drawModeSelect(false);
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
      drawModeSelect(false);
      if (isPot == 0)
      {
        setMode(rtCounter);
      }

      break;
    case 2:
      Serial.println("double click");
      setMode(0);
      drawModeSelect(true);
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

  //drawSlidesONotes(asSlides);

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

    // handle display



    // handle whether encoder acts as encoder or "potentiometer" and the range
    switch (encMode)
    {
      case 0: //default - mode select encoder
        if (rtCounter < 0)
        {
          rtCounter = 9;
        }

        if (rtCounter > 9)
        {
          rtCounter = 0;
        }


        switch (rtCounter)
        {
          case 0: // set default encoderMode
            drawMode(0, rtCounter);
            break;
          case 1: // Play chords - 4btns as chords sliders modify btns
            drawMode(1, rtCounter);
            break;
          case 2: // All cap inputs as scale buttons mode
            drawMode(2, rtCounter);
            break;
          case 3: // Adjust channel mute ON/OFF
            drawMode(3, rtCounter);
            break;
          case 4: // Mod scale - 4btns as scales sliders modify btns
            drawMode(4, rtCounter);
            break;
          case 5: // Slides or notes mode
            drawMode(5, rtCounter);
            break;
          case 6: // channel select mode;
            drawMode(6, rtCounter);
            break;
          case 7: // Explore mode - encoder as pot
            drawMode(7, rtCounter);
            break;
          case 8: // volume adjust per channel
            drawMode(8, rtCounter);
            break;
          case 9: // Broadcast X CHANNEL
            drawMode(9, rtCounter);
            break;
          
            break;

          default:
            break;
        }


        break;
      case 1:	// encoder as "pot" for note exploration
        constrainEnc(10, 110); // MIDI audible note values
        drawMode(disMode, rtCounter);
        break;
      case 2:	// encoder for channel select
        constrainEnc(1, 9); // # of MIDI channels
        drawMode(disMode, rtCounter);
        drawCh(rtCounter);
        break;
//      case 3:	// encoder for pitchbend
//        constrainEnc(0, 1); // on/off for pitchbend
//        drawMode(disMode, rtCounter);
//        //        drawCh(rtCounter);
//
//        break;
      case 4:	//encoder for chord octave switching
        constrainEnc(0, 6); // n * 12 to change 7 octaves
        drawMode(disMode, rtCounter);
        break;
      case 5:	//encoder for volume adjustment //not using just going to be boolean on/off js 20200620
        constrainEnc(1, 127); //  changes volume 1-127
        drawMode(disMode, rtCounter);
        break;
      case 6:	//encoder for chord channels 0 - 9 (1-9)
        constrainEnc(1, 9); //   channels 0 - 9 (1-9)
        drawMode(disMode, rtCounter);
        drawCh(rtCounter);
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
