/* 
 *  Denne kode uploader et stk. data til TEC SQL server via TEC netværksforbindelse
*/
#include "arduino_secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

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
const char serverName[] = SECRET_SERVER;


uint8_t target_esp_mac[6] = {0xcc, 0x50, 0xe3, 0x08, 0xa4, 0xe8};

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

//ADC Pin Variables
const int adcPin = 0;   

/*************************************/
/*   SETUP                           */
/*************************************/
void setup(void) {
  pinMode(adcPin,INPUT);
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
void loop(void) {

  // Read temperature
  float temp = digitalRead(adcPin); // ESP-01S does not have an Analog port
  Serial.print(temp); 
  Serial.println(" value read from GPIO-0");

  // If still connected to WiFi
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
  
    // Prepare your HTTP POST request data
    String httpRequestData = String(serverName) + "&temperatur=" + String(temp);

    http.begin(client,httpRequestData);
    
    // Specify content-type header
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.GET();

    Serial.println(httpCode);
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
#ifdef DEBUG
    delay(5000); // Delay 5 sec DEBUG ONLY
#else    
    // Five minutes delay
    for (int i=0;i<50;i++)
    {
      delay(6000);
    }
#endif
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
