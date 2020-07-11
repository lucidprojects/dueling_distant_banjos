#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String modeText;
int disMode;
int disCh = 0;

bool calComplete = false; // boolean to exit calibration while loop on completion

void draw()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Display setup failed");
    while (true)
      ;
  }

  // Clear the buffer
  display.clearDisplay();
  //  display.setFont(&FreeSans9pt7b);
  display.setRotation(4);
  display.setTextSize(2);	// Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0); // Start at top-left corner
  //	display.print(F("MIDI_CAN"));
  display.print(F("MUSIC_CAN"));

  display.setTextSize(1);	// Normal 1:1 pixel scale
  display.println(F(" TM"));

  display.display();
}


void drawMode(int disMode, int rtCounter)
{


  //disMode = 8;
  //
  switch (disMode)
  {
    case 0: // set default encoderMode
      modeText = "Explore";
      break;
    case 1: // channel select mode;
      modeText = "Ch Select";
      break;
    case 2: // Play chords - 4btns as chords sliders modify btns
      modeText = "Chords";
      break;
    case 3: // All cap inputs as scale buttons mode
      modeText = "Scales";
      break;
    case 4: // Mod scale - 4btns as scales sliders modify btns
      modeText = "Scales pb";
      break;
    case 5: // Slides or notes mode
      modeText = "Slds_or_note";
      break;
    case 6: // Explore mode - encoder as pot
      modeText = "Pot mode";
      break;
    case 7: // volume adjust per channel
      modeText = "Volume Adj";
      break;
    case 8: // Broadcast X CHANNEL
      modeText = "Broadcast";
      break;
    case 9: // Adjust channel volume ON/OFF
      modeText = "Mute on/off";
      break;
    case 10: // Calibration
      modeText = "Calibration";
      break;
    case 11: // connecting to wifi
      modeText = "Wifi Connect";
      break;
    case 12: // wifi connected
      modeText = "Connected";
      break;
    case 13: // wifi failed
      modeText = "Wifi Failed";
      break;      

    default:
      break;
  }



  display.setRotation(4);
  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 15); // Start at top-left corner
  display.print(F("M:"));

  display.setCursor(13, 15); // Start at top-left corner
  display.setTextColor(WHITE, BLACK);
  display.print(F("            "));
  display.setCursor(13, 15); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(modeText);

  display.setCursor(0, 24); // Start at top-left corner
  display.setTextColor(WHITE, BLACK);
  display.print(F("            "));
  display.setCursor(0, 24); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(F("E:"));

  display.setCursor(13, 24); // Start at top-left corner
  display.setTextColor(WHITE, BLACK);
  display.print(F("            "));
  display.setCursor(13, 24); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(rtCounter);

  display.display();
}





void drawCh(int myCh)
{

  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setRotation(4);
  display.setCursor(90, 15); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(F("C:"));
  display.setCursor(103, 15); // Start at top-left corner
  display.setTextColor(WHITE, BLACK);
  display.print(F(" "));
  display.setCursor(103, 15); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(myCh);

  display.display();
}


void drawSensorCal(int disSensor) {
  display.setRotation(4);
  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setCursor(90, 15); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(F("S:"));
  display.setCursor(103, 15); // Start at top-left corner
  display.setTextColor(WHITE, BLACK);
  display.print(F(" "));
  display.setCursor(103, 15); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(disSensor);

  display.display();

}

void drawModeSelect(bool disModeSelect) {
  if (disModeSelect == true)
  {
    display.setRotation(4);
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(120, 15); // Start at top-left corner
    display.println(F("*"));
  }
  else
  {
    display.setRotation(4);
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setTextColor(WHITE, BLACK); // Draw white text
    display.setCursor(120, 15); // Start at top-left corner
    display.println(F(" "));
  }
  display.display();
}

void drawOctaveSelect(bool disOctaveSelect) {
  if (disOctaveSelect == true)
  {
    display.setRotation(4);
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(120, 24); // Start at top-left corner
    display.println(F("*"));
  }
  else
  {
    display.setRotation(4);
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setTextColor(WHITE, BLACK); // Draw white text
    display.setCursor(120, 24); // Start at top-left corner
    display.println(F(" "));
  }
  display.display();
}


void drawSlidesONotes(bool disSlide) {
  display.setRotation(4);
  display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setTextColor(WHITE, BLACK); // Draw white text
    display.setCursor(60, 24); // Start at top-left corner
    display.println(F(" "));

  if (disSlide == true)
  {
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(60, 24); // Start at top-left corner
    display.println(F("S"));
  }
  else
  {
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(60, 24); // Start at top-left corner
    display.println(F("N"));
  }
  display.display();
}


void drawOctave(int disOct) {
  display.setRotation(4);
  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setCursor(90, 24); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(F("O:"));
  display.setTextColor(WHITE, BLACK); // Draw white text
  display.setCursor(103, 24); // Start at top-left corner
  display.println(F(" "));
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(103, 24); // Start at top-left corner
  display.println(disOct);

  display.display();
}




void drawBroadcast(int disBroadBool) {
  display.setRotation(4);
  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setCursor(30, 24); // Start at top-left corner
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(F("B:"));
  display.setTextColor(WHITE, BLACK); // Draw white text
  display.setCursor(43, 24); // Start at top-left corner
  display.println(F(" "));
  if (disBroadBool == true)
  {
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(43, 24); // Start at top-left corner
    display.println(F("1"));
  }
  else
  {
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(43, 24); // Start at top-left corner
    display.println(F("0"));
  }



  display.display();
}
