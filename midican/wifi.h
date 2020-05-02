#include <WiFiNINA.h>
#include "secrets.h"

char ssid[] = _SSID;
char pass[] = _PASSWORD;

void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

void initWifi() 
{
	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("Communication with WiFi module failed!");
		while (true);
	}

	String fv = WiFi.firmwareVersion();
	if (fv < WIFI_FIRMWARE_LATEST_VERSION)
	{
		Serial.println("Please upgrade the firmware");
	}

	// attempt to connect to Wifi network:
	int status = WL_IDLE_STATUS;
	while (status != WL_CONNECTED)
	{
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pass);

		// wait 10 seconds for connection:
		delay(10000);
	}

	Serial.println("Connected to WiFi");
	printWifiStatus();

	Serial.println("\nStarting connection to sender server...");
}