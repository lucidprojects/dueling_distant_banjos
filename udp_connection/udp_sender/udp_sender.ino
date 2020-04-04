#include <WiFiNINA.h>
#include <WiFiUdp.h>


char ssid[] = "wifinet"; // your network SSID (name)
char pass[] = "wifipass"; // your network password (use for WPA, or use as key for WEP)

unsigned int localPort = 2390;  // local port to listen on

WiFiUDP Udp;

unsigned long timestamp;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);  // wait for serial port to connect

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
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

  Udp.begin(localPort);
}

void loop() {
  // periodically send a message
  if (millis() - timestamp > 3000) {

  // for testing localNet
  Udp.beginMulticast(IPAddress(226, 1, 1, 1), 3000);

  // for testing openNet
  // Udp.beginMulticast(IPAddress(xxx, xxx, xxx, xxx), 3000);

    
    Udp.write("hello");
    Udp.endPacket();
    timestamp = millis();
  }

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    char packetBuffer[255]; //buffer to hold incoming packet
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, sizeof(packetBuffer) / sizeof(packetBuffer[0]));
    if (len > 0) {
    packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
  }
}


void printWifiStatus() {
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
