/* Sensor test sketch
  for more information see http://www.ladyada.net/make/logshield/lighttemp.html
  */


//ADC Pin Variables
int adcPin = A0;        

int tempReading;        // the analog reading from the sensor

/*************************************/
/*   SETUP                           */
/*************************************/
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(115200);   

  delay(2);
  Serial.print("Ready!");

}


/*************************************/
/*   LOOP                            */
/*************************************/
void loop(void) {

  // Read temperature
  //int temp = readTemp();
  float raw = analogRead(adcPin); 

  Serial.print(raw); 
  Serial.print(", "); 
  float volt = (raw/1024.0)*1.835; // Huzzah ADC is MAX 1.0 Volt
  Serial.print(volt); 
  Serial.print(", ");
  float temp = (volt-0.5)*100.0;    // TMP 36 has 500mV offset and 1 deg. /10 mV
  Serial.print(temp); 
  Serial.println(" degrees C");

    // Five minutes delay
//    for (int i=0;i<50;i++)
    {
      delay(500);
    }

}
