
 *  Denne kode får din ESP8266 modul på et privat netværk
 *  Ændr data i "arduino_secrets.h" filen og upload til dit ESP8266 modul.
 *  Værktøjet her aflæser én værdi på Analog 0 (A0) og sender til ThingSpeak.com servicen
*/
#include "arduino_secrets.h"   //  <-- I denne fil (se oppe i fanebladene) skal du skrive dine netværk´s oplysninger
#include <ESP8266WiFi.h>

extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"
}

// SSID to connect to
const char ssid[] = SECRET_SSID;
const char username[] = SECRET_USER;
const char identity[] = SECRET_USER;
const char password[] = SECRET_PASS;

uint8_t target_esp_mac[6] = {0xcc, 0x50, 0xe3, 0x08, 0xa4, 0xe8};

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String APIKey = WRITE_API;             // enter your channel's Write API Key in the arduino_secrets.h file
const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;




//ADC Pin Variables
int adcPin = A0;        

//the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures
int tempReading;        // the analog reading from the sensor

/*************************************/
/*   SETUP                           */
/*************************************/
void setup() {

  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  delay(1000);
  Serial.setDebugOutput(true);
  Serial.printf("SDK version: %s\n", system_get_sdk_version());
  Serial.printf("Free Heap: %4d\n",ESP.getFreeHeap());
  
  // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(STATION_MODE);

  struct station_config wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  strcpy((char*)wifi_config.password, password);

  wifi_station_set_config(&wifi_config);
  wifi_set_macaddr(STATION_IF,target_esp_mac);
  

  wifi_station_set_wpa2_enterprise_auth(1);

  // Clean up to be sure no old data is still inside
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_clear_enterprise_identity();
  wifi_station_clear_enterprise_username();
  wifi_station_clear_enterprise_password();
  wifi_station_clear_enterprise_new_password();
  
  wifi_station_set_enterprise_identity((uint8*)identity, strlen(identity));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen((char*)password));

  
  wifi_station_connect();
   Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }


 // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  printWifiData();

}


/*************************************/
/*   LOOP                            */
/*************************************/
void loop() {
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
