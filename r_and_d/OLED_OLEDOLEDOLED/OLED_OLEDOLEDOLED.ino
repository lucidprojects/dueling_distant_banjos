/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
	Serial.begin(9600);
	while (!Serial);

	Serial.begin(9600);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
		Serial.println("Display setup failed");
		while (true);
	}

	// Clear the buffer
	display.clearDisplay();
	
	display.setRotation(1);

	// Draw a single pixel in white
	display.drawPixel(10, 10, SSD1306_WHITE);

	// Show the display buffer on the screen. You MUST call display() after
	// drawing commands to make them visible on screen!
	display.display();
	delay(2000);
}

void loop()
{
	testdrawstyles(); // Draw 'stylized' characters
}

void testdrawstyles(void)
{
	display.clearDisplay();

	display.setTextSize(1);				 // Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE); // Draw white text
	display.setCursor(0, 0);			 // Start at top-left corner
	display.println(F("M: 1"));
	display.println();
	display.println(F("Ch: 4"));
	display.println();
	display.println(F("A: 1"));
	display.println();

	// display.setTextSize(2); // Draw 2X-scale text

	display.display();
	delay(2000);
}