#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

int buzzerPin = 9;
#define DHTPIN 2       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22  // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  dht.begin();
  Serial.println(F("DHT22 Sensor"));
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
}
float temper;
void loop() {
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print("Lux: ");
  Serial.println(lux);
  //
  if (lux <= 20) {
    digitalWrite(4, HIGH);
  } else {
    digitalWrite(4, LOW);
  }
  //
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    temper= event.temperature;
    if (temper >= 26.00) {
      Serial.println(temper);
      digitalWrite(3, HIGH);
      tone(buzzerPin,2093,250);
    } else {
      Serial.println(temper);
      digitalWrite(3, LOW);
    }
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  delay(5000);
}
