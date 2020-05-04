#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>

const char* ssid = "MYWIRE-WPA-132";
const char* password =  "iefee9aehiem";
 
#define mqtt_server "192.168.2.247"

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Temperature","openhabian","openhabian")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=Berlin,de&APPID=";
const String key = "50a65c65132cd5c771d05bf5cf28753c";

DynamicJsonDocument doc(2024);
Adafruit_SSD1306 display(-1);

void setup() { 
  Serial.begin(9600);
  WiFi.begin(ssid, password); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqtt_server, 1883);
}


void loop() {
  if (!client.connected()) {
    	reconnect();
  }
  client.loop();
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(endpoint + key);
    int httpCode = http.GET();
    if (httpCode > 0) {
        deserializeJson(doc,http.getString());
        float temp = doc["main"]["temp"];
        temp=temp-273.15;
        Serial.println(temp);
        display.setTextSize(1);
        display.setCursor(0,21);
	      display.setTextColor(WHITE);
	      display.println("Temperature:");
        display.println(temp);
        client.publish("Fiware/SkiRes/Temp",String(temp).c_str(),true);
        display.display();
    } 
    else {
      Serial.println("Error on HTTP request");
    }
    http.end();
    display.clearDisplay();
  }
  delay(30000);
}