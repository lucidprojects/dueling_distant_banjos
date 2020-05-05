#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_JSON.h>

#include "secrets.h"

// CAPACITIVE

int sensorMinBuffer = 600;

// WIFI

char ssid[] = _SSID;
char pass[] = _PASSWORD;

// TCP

int status = WL_IDLE_STATUS;

WiFiServer server(5555);

String response;

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

	// start the web server on port 5555
	server.begin();
	Serial.println("\nStarting server on port 5555");
}

void handleResponse(WiFiClient client)
{
	// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
	// and a content-type so the client knows what's coming, then a blank line:
	client.println("HTTP/1.1 200 OK");
	client.println("Content-type:application/json");
	client.println();

	// the content of the HTTP response follows the header:
	client.print(response);

	// The HTTP response ends with another blank line:
	client.println();
}

void handleData(String currentLine)
{
	// Check to see if the client request was "GET /H" or "GET /L":
	if (currentLine.startsWith("data"))
	{
		if (currentLine.endsWith("}"))
		{
			response = "{\"message\":\"success\"}";

			currentLine.remove(0, 6);
			
			JSONVar newData = JSON.parse(currentLine);

			// JSON.typeof(jsonVar) can be used to get the type of the var
			if (JSON.typeof(newData) == "undefined")
			{
				Serial.println("Parsing input failed!");
				response = "{\"error\":true}";
				return;
			}

			if (newData.hasOwnProperty("capBuff"))
			{
				sensorMinBuffer = newData["capBuff"];
				Serial.print("sensorMinBuffer set to: ");
				Serial.println(sensorMinBuffer);
			}

		}
	}

	if (currentLine.endsWith("GET /"))
	{
		Serial.println("GET req made");
		response = "{\"capBuff\":\"" + String(sensorMinBuffer) + "\", \"message\":\"success\"}";
	}

}

// TODO: Handle in LOOP (make fn)
void webApi()
{
	WiFiClient client = server.available(); // listen for incoming clients

	if (client)
	{								  
		Serial.println("new client");
		String currentLine = ""; // make a String to hold incoming data from the client

		// loop while the client's connected
		while (client.connected())
		{
			// if there's bytes to read from the client,
			if (client.available())
			{							
				char c = client.read(); // read a byte, then

				// if the byte is a newline character
				if (c == '\n')
				{
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0)
					{
						handleResponse(client);
						break;
					}
					// if you got a newline, then clear currentLine
					else
					{
						currentLine = "";
					}
				}

				// if you got anything else but a carriage return character,
				// add it to the end of the currentLine
				else if (c != '\r')
				{					  
					currentLine += c;
				}

				handleData(currentLine);
			}
		}

		// close the connection:
		client.stop();
		Serial.println("client disconnected");
	}
}
