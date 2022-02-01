/* 
 *  Denne kode får din ESP8266 modul på et privat netværk
 *  Ændr data i "arduino_secrets.h" filen og upload til dit ESP8266 modul.
 *  Værktøjet her aflæser én værdi på Analog 0 (A0) og sender til ThingSpeak.com servicen
*/
#include "arduino_secrets.h"   //  <-- I denne fil (se oppe i fanebladene) skal du skrive dine netværk´s oplysninger
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = SECRET_SSID;
const char* pass = SECRET_PASS;
int status = WL_IDLE_STATUS;     // the WiFi radio's status

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String APIKey = WRITE_API;             // enter your channel's Write API Key in the arduino_secrets.h file
const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;

//ADC Pin Variables
int adcPin = A0;        

//the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures
int tempReading;        // the analog reading from the sensor

/*************************************/
/*   SETUP                           */
/*************************************/
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(115200);   

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA2 enterprise network:
    status = WiFi.begin(ssid, pass);  // Udkommenter (tilføj "//" foran denne linje) når du er på TEC / HCØ's netværk.
    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  printWifiData();

}


/*************************************/
/*   LOOP                            */
/*************************************/
void loop(void) {
  // Get current temperature
  float temperature = ReadTemp();
  
  // Print Update Response to Serial Monitor
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected) {
    Serial.println("...disconnected");
    Serial.println();
    client.stop();
  }

  // Update ThingSpeak
  if (!client.connected() && ((millis() - lastConnectionTime) > updateThingSpeakInterval)) {
    updateThingSpeak("field1=" + String(temperature));
    Serial.print("temp. sent to ThingSpeak: ");
    Serial.println(temperature);
    delay(1000);
  }
  lastConnected = client.connected();
}

/*********************************************************************/
//   readTemp                        
/*********************************************************************/
float ReadTemp(){
  
  delay(100);
  tempReading = analogRead(adcPin);  
 
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * 6.6;
  voltage /= 1024.0; 
  
  // now print out the temperature
  float temperatureC = (voltage - 0.25) * 10 ;  // converting from 10 mv per degree wit 250 mV offset
                                                // due to voltage divider
                                               //to degrees ((volatge - 250mV) times 10)
  return temperatureC;
}

/*********************************************************************/
// UPDATE THINGSPEAK
/*********************************************************************/
void updateThingSpeak(String tsData) {
  if (client.connect(thingSpeakAddress, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + APIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);
    lastConnectionTime = millis();

    if (client.connected()) {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
    }
  }
}

/*********************************************************************/
// printWifiData
/*********************************************************************/
void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}


/*********************************************************************/
//printMacAddress
/*********************************************************************/
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
