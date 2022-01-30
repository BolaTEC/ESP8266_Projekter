/* Denne kode uploader et stk. data til TEC SQL server
*/
#include "arduino_secrets.h"   //  <-- I denne fil (se oppe i fanebladene) skal du skrive dine netværk´s oplysninger
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
int status = WL_IDLE_STATUS;     // the WiFi radio's status

const char* ssid = SECRET_SSID;
const char* pass = SECRET_PASS;

const char* serverName = SECRET_SERVER;

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
    // - You can optionally provide additional identity and CA cert (string) parameters if your network requires them:
    //      WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
    // status = WiFi.beginEnterprise(ssid, user, pass); // Enable denne linje (fjern "//"), når du er på TEC / HCØ´s netværk, og rediger arduino_secrets.h filen med dine data
    status = WiFi.begin(ssid, pass);  // Udkommenter (tilføj "//" foran denne linje) når du er på TEC / HCØ's netværk.
    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

}

/*************************************/
/*   readTemp                        */
/*************************************/
float readTemp(){
  
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

/*************************************/
/*   LOOP                            */
/*************************************/
void loop(void) {

  // Read temperature
  float temp = readTemp();
  Serial.print(temp); 
  Serial.println(" degrees C");

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

    // Five minutes delay
    for (int i=0;i<50;i++)
    {
      delay(6000);
    }
  }

}
