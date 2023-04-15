#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
}

void loop() {
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print("Lux: ");
  Serial.println(lux);
  delay(1000);
}
