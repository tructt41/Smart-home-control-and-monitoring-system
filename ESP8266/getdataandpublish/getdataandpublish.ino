#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ThingSpeak.h>
#include <BH1750.h>
#include <Wire.h>

#define DHTPIN  D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define BH1750_ADDRESS 0x23
BH1750 lightMeter;

const char* ssid = "ESP8266_IOT";
const char* password = "ESP8266_IOT";
const char* server = "api.thingspeak.com";
const char* apiKey = "2SJUIEQIRZMXE39P";
unsigned long myChannelNumber = 2091496;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  uint16_t lux = lightMeter.readLightLevel();
  if (isnan(h) || isnan(t) || isnan(lux)) {
    Serial.println("Failed to read from sensors!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  if (client.connect(server, 80)) {
    String postStr = String(apiKey);
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(lux);
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n");
    client.print("X-THINGSPEAKAPIKEY: " + String(apiKey) + "\n");
    client.print("\n");
    client.print(postStr);
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C\t");
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx\t");
  }
  client.stop();
  delay(150000);
}
