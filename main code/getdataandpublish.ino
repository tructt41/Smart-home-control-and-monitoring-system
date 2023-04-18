#include <PubSubClient.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <ArduinoJson.h>

#define TOKEN "ESP8266_MCU"
#define DHTPIN D7  //GPIO02 D4 PIN in wemos or 2 in nodeMCU
#define DHTTYPE DHT22
#define LedPin D6  //GPIO14 D5 PIN in wemos or 14 in nodeMCU
#define GPIO0 D3
#define GPIO2 D5

#define GPIO0_PIN 3
#define GPIO2_PIN 4

const char* ssid = "UiTiOt-E3.1";
const char* password = "UiTiOtAP";
const char mqtt_server[] = "thingsboard.cloud";
const char publishTopic[] = "v1/devices/me/telemetry";

WiFiClient mkr1010Client;
PubSubClient client(mkr1010Client);
long lastData = 0;
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
int automation;
boolean gpioState[] = { false, false };

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
// The callback for when a PUBLISH message is received from the server.
void on_message(const char* topic, byte* payload, unsigned int length) {

  Serial.println("On message");

  char json[length + 1];
  strncpy(json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);

  if (!data.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  // Check request method
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("getGpioStatus")) {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  } else if (methodName.equals("0")) {
    // Update GPIO status and reply
    set_gpio_status(0, data["params"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  } else if (methodName.equals("3")) {
    // Update GPIO status and reply
    set_gpio_status(3, data["params"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  } else if (methodName.equals("4")) {
    // Update GPI2 status and reply
    set_gpio_status(4, data["params"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}

String get_gpio_status() {
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  if (gpioState[0] == true && gpioState[1] == true) data[String(0)] = true;
  else data[String(0)] = false;
  data[String(GPIO0_PIN)] = gpioState[0] ? true : false;
  data[String(GPIO2_PIN)] = gpioState[1] ? true : false;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Get gpio status: ");
  Serial.println(strPayload);
  return strPayload;
}

void set_gpio_status(int pin, boolean enabled) {
  if (pin == GPIO0_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO0, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[0] = enabled;
  } else if (pin == GPIO2_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO2, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[1] = enabled;
  } else if (pin == 0) {
    // Output GPIOs state
    digitalWrite(GPIO0, enabled ? HIGH : LOW);
    digitalWrite(GPIO2, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[0] = enabled;
    gpioState[1] = enabled;
  }
}
void reconnect() {

  while (!client.connected()) {

    Serial.print("Attempting MQTT connection ....");

    if (client.connect("ClientID", TOKEN, NULL)) {

      Serial.println("Connected to MQTT Broker");
      digitalWrite(LED_BUILTIN, HIGH);
      // Subscribing to receive RPC requests
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Sending current GPIO status ...");
      client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
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
  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();  //(SDA, SCL);
  if (lightMeter.begin()) {
    Serial.println(F("BH1750 initialised"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  automation = 0;
  client.setCallback(on_message);
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
    if (l > 2000) {
    Serial.println("Failed to read from Bh1750 sensor");
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