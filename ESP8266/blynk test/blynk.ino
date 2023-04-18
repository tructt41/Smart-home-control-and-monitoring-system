/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6cvq-57t4"
#define BLYNK_TEMPLATE_NAME "ESP8266"
#define BLYNK_AUTH_TOKEN "1vbRZBRZAOMWPW9wfEaj0RO_yKYJ0Tyx"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ESP8266_IOT";
char pass[] = "ESP8266_IOT";

//temperature and humidity sensor pin define
#define DHTPIN 2 //GPIO02 D4 PIN
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE); //define dhtpin and dht type(dht22)

#define LedPin 14 //GPIO14 D5 PIN
#define outlight 13 //GPIO14 D5 PIN
#define device 12 //GPIO14 D5 PIN

int automation;
BlynkTimer timer;
BH1750 lightMeter;


// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0);  // will cause BLYNK_WRITE(V0) to be executed
  Blynk.syncVirtual(V1  );  // will cause BLYNK_WRITE(V0) to be executed
  // Change Web Link Button message to "Congratulations!"
}
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

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
  BLYNK_WRITE(V0) // Executes when the value of virtual pin 0 changes
  {
    if(param.asInt() == 1)
    {
      // execute this code if the switch widget is now ON
      digitalWrite(outlight,HIGH);  // Set digital pin 2 HIGH
    }
    else
    {
      // execute this code if the switch widget is now OFF
      digitalWrite(outlight,LOW);  // Set digital pin 2 LOW    
    }
  }
  BLYNK_WRITE(V1) // Executes when the value of virtual pin 0 changes
  {
    if(param.asInt() == 1)
    {
      // execute this code if the switch widget is now ON
      digitalWrite(device,HIGH);  // Set digital pin 2 HIGH
    }
    else
    {
      // execute this code if the switch widget is now OFF
      digitalWrite(device,LOW);  // Set digital pin 2 LOW    
    }
  }
void setup()
{
  // Debug console
  Serial.begin(115200);

  pinMode(device, OUTPUT);
  pinMode(outlight, OUTPUT);
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);
  Wire.begin();  //(SDA, SCL);
  lightMeter.begin();
  dht.begin();
  setup_wifi();
  automation = 0;
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
  timer.run();
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
  // send data collected from sensors to blynk
  Blynk.virtualWrite(V4,t);
  Blynk.virtualWrite(V5,h);
  Blynk.virtualWrite(V6,l);
}