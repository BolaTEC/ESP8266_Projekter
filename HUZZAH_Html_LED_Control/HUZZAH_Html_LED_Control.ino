

/*
 * Projekt: Set up Web-side hvorfra 3 LEDs
 * kan tændes og slukkes
 * 
 * Predefinerede konstanter:
 * OUTPUT, HIGH, LOW
 * 
 * Indbyggede funktioner:
 * Serial.begin(speed)
 * Serial.print(val)
 * Serial.println(val)
 * pinMode(pin, mode)
 * digitalWrite(pin, value)
 * delay(ms)
 * 
 * Funktioner fra ESP8266 biliotek
 * WiFiServer
 * WiFi.begin(ssid, password)
 * WiFi.status()
 * WiFi.localIP()
 * 
 * server.begin()
 * server.available()
 * 
 * client.connected()
 * client.available()
 * client.read()
 * client.println
 * 
 * Brugerdefinerede konstanter:
 * ssid, password
 * redLED, greenLED, yellowLED 
 * 
 * Brugerdefinerede variable:
 * header
 * stateRedLED, stateGreenLED, stateYellowLED
 */



// Load Wi-Fi bibliotek til ESP8266
#include <ESP8266WiFi.h>

// Load WIFI secrets form wifi_secrets.h file
#include "wifi_secrets.h"

// SSID to connect to
const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASS;

// web server port sætte til port 80
WiFiServer server(80);

// Variabel der gemmer HTTP request
String header;

// Textstrenge der indholder status for de tre LEDs
String stateRedLED = "off";
String stateYellowLED = "off";
String stateGreenLED = "off";

// de tre LED forbindes til pin 4, 5 og 16
const int greenLED = 4;
const int yellowLED = 5;
const int redLED = 16;



/********************************************************/
/*   SETUP                                              */
/********************************************************/
void setup() {
   // Initialiser seriel kommunikation
  Serial.begin(115200);
  delay(100);
  
  // Initialiser de 3 output variable til OUTPUT
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);

  // Opret forbindelse ti WiFi netværk
  // med ssid and password
  Serial.println();
  Serial.println();
  Serial.print("Opret forbindelse til ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  // Skriv den lokale IP addresse og start web server
  Serial.println("");
  Serial.println("WiFi forbundet.");
  Serial.println("IP addresse: ");
  Serial.println(WiFi.localIP());
  server.begin();
}



/********************************************************/
/*   LOOP                                               */
/********************************************************/
void loop(){
// Lytter efter indkomne klienter
WiFiClient client = server.available();   

// Hvis en klient forbindes
if (client) {   
  // Besked om at der er en ny klient                         
  Serial.println("Ny klient.");

  // Tekststreng der indeholder data fra klient
  String currentLine = "";    
    
  // loop mens klienten er forbundet            
  while (client.connected()) {    
              
    // Hvis der er data fra klienten
    if (client.available()) {   

      // læs en byte    
      char c = client.read();   

      // den læste byte udskrives, og tilføjes til
      // strengen header        
      Serial.write(c);                    
      header += c;

      // hvis den modtagen karakter er et linjeskift
      if (c == '\n') {                   
        // hvis den aktuelle linje er tom, 
        // så er der modtaget to linjeskift i træk,
        // hvilket markerer slutningen på klientens 
        // HTTP request, så sendes et response:
        if (currentLine.length() == 0) {
          // HTTP headers starter altid med en
          // responskode (fx. HTTP/1.1 200 OK)
          // Koder kan tjekkes på: 

// en.wikipedia.org/wiki/List_of_HTTP_status_codes

          // og en indholdstype, så klienten ved hvad der
          // kommer, der afsluttes med en blank linje:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
            
          // tænd og sluk de digitale udgange 2, 4 og 5
          // der styrer de 3 LEDs, og skriv tilstand til 
          // Seriel Monitor i Arduino
          if (header.indexOf("GET /5/on") >= 0) {
            Serial.println("Rød LED on");
            stateRedLED = "on";
            digitalWrite(redLED, HIGH);
          } else if (header.indexOf("GET /5/off") >= 0) {
            Serial.println("Rød LED off");
            stateRedLED = "off";
            digitalWrite(redLED, LOW);
          } else if (header.indexOf("GET /4/on") >= 0) {
            Serial.println("Gul LED on");
            stateYellowLED = "on";
            digitalWrite(yellowLED, HIGH);
          } else if (header.indexOf("GET /4/off") >= 0) {
            Serial.println("Gul LED off");
            stateYellowLED = "off";
            digitalWrite(yellowLED, LOW);
            } else if (header.indexOf("GET /12/on") >= 0) {
            Serial.println("Grøn LED on");
            stateGreenLED = "on";
            digitalWrite(greenLED, HIGH);
          } else if (header.indexOf("GET /12/off") >= 0) {
            Serial.println("Grøn LED off");
            stateGreenLED = "off";
            digitalWrite(greenLED, LOW);
          }
            
          // Vis HTML web page
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link rel=\"icon\" href=\"data:,\">");
          // CSS der styrer udseende af knapper 
          // Ret så du får den ønskede background-color 
          // og font-size attributter,
          // der passser til dit formål
          client.println("<style>html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}");
          client.println(".button { background-color: #A2260AF; border: none; color: white; padding: 16px 40px; ");
          client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
          client.println(".buttonR {background-color: #E82D24;}");
          client.println(".buttonY {background-color: #F7C71B;}");
          client.println(".buttonG {background-color: #A2D040;}</style></head>");
            
          // Overskrift på websiden
          client.println("<body><h1>LED Kontrol - version 1.1</h1>");
            
          // Vis status og knap for tænd/sluk: rød LED
          client.println("<p>RED LED - Tilstand " + stateRedLED + "</p>");
          // Hvis stateRedLED er off, vises ON button       
          if (stateRedLED=="off") {
            client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
          } else {
            client.println("<p><a href=\"/5/off\"><button class=\"button buttonR\">OFF</button></a></p>");
          } 
               
          // Vis status og knapp for tænd/sluk: gul LED  
          client.println("<p>Yellow LED - Tilstand " + stateYellowLED + "</p>");
          // Hvis stateYellowLED er off, vies ON button       
          if (stateYellowLED=="off") {
            client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
          } else {
            client.println("<p><a href=\"/4/off\"><button class=\"button buttonY\">OFF</button></a></p>");
          } 

          // Vis status og knap for tænd/sluk: grøn LED  
          client.println("<p>Green LED - Tilstand " + stateGreenLED + "</p>");
          // Hvis stateGreenLED er off, vies ON button       
          if (stateGreenLED=="off") {
            client.println("<p><a href=\"/12/on\"><button class=\"button\">ON</button></a></p>");
          } else {
            client.println("<p><a href=\"/12/off\"><button class=\"button buttonG\">OFF</button></a></p>");
          }
            
          client.println("</body></html>");
            
          // HTTP response sluttes med endnu 
          // en blank linje
          client.println();
          // Her stoppe while loop med Break kommando
          break;
            
          // hvis det er en ny linje der modtages, 
          // så clear den aktuelle linje
        } else { 
          currentLine = "";
        }
        // hvis der modtages alt andet en retur karakter
      } else if (c != '\r') {  
        // tilføj til slutningen af den aktuelle linje
        currentLine += c;      
      }
    }
  }
  // Overskriv header variable så den er tom
  header = "";

  // Luk forbndelsen ned
  client.stop();
  Serial.println("Klient ikke længere forbundet.");
  Serial.println("");
}
}
