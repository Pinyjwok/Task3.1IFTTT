#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password

WiFiClient client;

char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "/trigger/sunlight_detected/with/key/czawHzGErAtP6VByRSDbHe";
String queryString = "?value1=57&value2=25";

char HOST_NAME2[] = "maker.ifttt.com";
String PATH_NAME2 = "/trigger/sunlight_stop/with/key/czawHzGErAtP6VByRSDbHe";
String queryString2 = "?value1=57&value2=25";

bool sunlightDetected = false;

void setup() {
  // initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  // attempt to connect to WiFi network
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    delay(1000);
  }

  Serial.println("Connected to WiFi");

  // wait for connection to be established
  delay(1000);

  Wire.begin();
  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  // Read light level
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (lux >= 4000 && !sunlightDetected) {
    // Sunlight detected for the first time
    sunlightDetected = true;
    Serial.println("Sunlight has been detected.");
    // Connect to server
    Serial.print("Connecting to server: ");
    if (client.connect(HOST_NAME, 80)) {
      Serial.println("Connected to server");
      // Send request
      client.print("GET " + PATH_NAME + queryString + " HTTP/1.1\r\n");
      client.print("Host: " + String(HOST_NAME) + "\r\n");
      client.print("Connection: close\r\n\r\n");
      // Wait for response from server
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.print(c);
        }
      }
      // Close connection
      client.stop();
    } else {
      Serial.println("Connection failed");
    }
  } else if (lux < 4000 && sunlightDetected) {
    // Sunlight is no longer detected
    sunlightDetected = false;
    Serial.println("Sunlight is no longer detected.");
    // Connect to server
    Serial.print("Connecting to server: ");
    if (client.connect(HOST_NAME2, 80)) {
      Serial.println("Connected to server");
      // Send request
      client.print("GET " + PATH_NAME2 + queryString2 + " HTTP/1.1\r\n");
      client.print("Host: " + String(HOST_NAME2) + "\r\n");
      client.print("Connection: close\r\n\r\n");
      // Wait for response from server
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.print(c);
        }
      }
      // Close connection
      client.stop();
      // Delay before checking sunlight again
      delay(1000); 
    } else {
      Serial.println("Connection failed");
    }
  } else {
    // Delay before checking sunlight again
    delay(1000);
  }
}
