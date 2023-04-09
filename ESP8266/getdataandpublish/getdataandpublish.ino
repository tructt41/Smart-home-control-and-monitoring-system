#include <PubSubClient.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

#define TOKEN "ESP8266_MCU"
#define DHTPIN 2
#define DHTTYPE DHT22
#define LedPin 14

const char* ssid = "ESP8266_IOT";
const char* password = "ESP8266_IOT";
const char mqtt_server[] = "thingsboard.cloud";
const char publishTopic[] = "v1/devices/me/telemetry";

WiFiClient mkr1010Client;
PubSubClient client(mkr1010Client);
long lastData = 0;
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
int automation;
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Attempting MQTT connection ....");

    if (client.connect("ClientID", TOKEN, NULL)) {

      Serial.println("Connected to MQTT Broker");
      digitalWrite(LED_BUILTIN, HIGH);
    }

    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 second");
      digitalWrite(LED_BUILTIN, LOW);
      delay(5000);
    }
  }
}

void setup() {

  pinMode(5, OUTPUT);
  pinMode(LedPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();  //(SDA, SCL);
  lightMeter.begin();
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  automation = 0;
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  uint16_t l = lightMeter.readLightLevel();
  if (l >= 10) {
    automation = 0;
    digitalWrite(LedPin, LOW);
  } else {
    automation = 1;
    digitalWrite(LedPin, HIGH);
  }
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }
  String ledstate = String(automation);
  String temperature = String(t);
  String humidity = String(h);
  String intensity = String(l);

  String payload = "{\"temperature\":";
  payload += temperature;
  payload += ",\"humidity\":";
  payload += humidity;
  payload += ",\"light_intensity\":";
  payload += intensity;
  payload += ",\"led_state\":";
  payload += ledstate;
  payload += "}";

  char attributes[1000];
  long now = millis();

  if (now - lastData > 5000) {

    lastData = now;
    payload.toCharArray(attributes, 1000);
    client.publish(publishTopic, attributes);
    Serial.println(attributes);
  }
}