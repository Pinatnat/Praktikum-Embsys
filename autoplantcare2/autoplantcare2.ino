/*
Reference :
1.https://arduinogetstarted.com/tutorials/arduino-water-sensor
2.https://lastminuteengineers.com/water-level-sensor-arduino-tutorial/
3.http://robojax.com/node/894
4.https://microcontrollerslab.com/arduino-timer-interrupts-tutorial/

*/
#include "DHT.h"

#include <avr/io.h>
#include <avr/interrupt.h>

const int LED_pin = 13;
unsigned int reload = 0xF424;
volatile int count;

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTPIN 2
DHT dht(DHTPIN, DHTTYPE);
int pin  = A5;
int value = 0; // variable to store the sensor value
char test[128];
char test2[128];
float temp;
float humid;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = reload;
  TCCR1B = (1 << WGM12) | (1 << CS12);
  TIMSK1 = (1 << OCIE1A);
  sei();

  dht.begin();
  if (Serial.available()){
    Serial.write("available");
  }
}

void loop() {

  if (Serial.available()) {
    humid = dht.readHumidity();
    temp = getTemp("c");
    char temp2[6];
    char humid2[6];
    dtostrf(temp, 4, 2, temp2);
    dtostrf(humid, 4, 2, humid2);
    sprintf(test, "T: %s ; ", temp2);
    sprintf(test2, "H: %s", humid2);
    if (Serial.available()) {
      if (value < 100) {
        if (temp > 34){
          Serial.write("2\n");
        }
        else {
          Serial.write("1\n");
        }
      }
      else if (value >= 100) {
        if (temp > 34){
          Serial.write("3\n");
        }
        else {
          Serial.write("0\n");
        }
      }
    }
    delay(50);
  };
}
ISR(TIMER1_COMPA_vect) {
  //read water level sensor
  value = analogRead(pin);
  count++;
}

float getTemp(String req)
{

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    //Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Kelvin
  float k = t + 273.15;
  if (req == "c") {
    return t;//return Cilsus
  } else if (req == "f") {
    return f;// return Fahrenheit
  } else if (req == "h") {
    return h;// return humidity
  } else if (req == "hif") {
    return hif;// return heat index in Fahrenheit
  } else if (req == "hic") {
    return hic;// return heat index in Cilsus
  } else if (req == "k") {
    return k;// return temprature in Kelvin
  } else {
    return 0.000;// if no reqest found, retun 0.000
  }

}
