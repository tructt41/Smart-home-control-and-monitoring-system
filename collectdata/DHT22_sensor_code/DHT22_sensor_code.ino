#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
void setup() {
  Serial.begin(9600);
  // Initialize device.
  dht.begin();
  Wire.begin(8); // khởi tạo I2C với địa chỉ I2C=8
  
  Wire.onRequest(sendInt); // đăng ký hàm sendInt() cho sự kiện I2C request từ master
}
int t,h;
void loop() {
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    t=event.temperature*100;
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    h = event.relative_humidity*100;
    Serial.println(F("%"));
  }
  delay(3000);                           // Đợi 5 giây trước khi đọc lại dữ liệu
}
void sendInt() {
  Wire.write(t >> 8);    // Gửi byte cao của value1
  Wire.write(t & 0xFF);  // Gửi byte thấp của value1
  Wire.write(h >> 8);    // Gửi byte cao của value2
  Wire.write(h & 0xFF);  // Gửi byte thấp của value2
  
  Serial.println(t);
  Serial.println(h);
  delay(1000);                // Chờ 1 giây trước khi gửi lại
}
