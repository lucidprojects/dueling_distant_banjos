#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

	display.setRotation(2);
	display.setTextSize(2);	// Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE); // Draw white text
	display.setCursor(0, 0); // Start at top-left corner
	display.print(F("MIDI_CAN"));

	display.setTextSize(1);	// Normal 1:1 pixel scale
	display.println(F(" TM"));

	display.display();
}