// This code is programmed to run on a Wio Node
// Connect a Grove SeeedOLED screen on the I2C1 port of the Wio Node
// Can be easily modified to adapt to a Generic OLED I2C screen with a NodeMCU or Arduino
// Power via USB or Battery
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SeeedOLED.h>

const char* ssid = "WIFI";
const char* password = "WIFI_PASS";

#define mqtt_server "SERVER IP"

unsigned long previousMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ParkingLot","openhabian","openhabian")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

void setup()
{
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
  Serial.begin(9600);
  Wire.begin(4,5);
  SeeedOled.init();
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 5000) {
    previousMillis = currentMillis;
    SeeedOled.clearDisplay();          
    SeeedOled.setNormalDisplay();      
    SeeedOled.setPageMode();           
    SeeedOled.setTextXY(0, 0);
    int p=random(1024);
    SeeedOled.putString("Parking Slots");
    SeeedOled.setTextXY(1, 0);     
    SeeedOled.putString("Available :");
    SeeedOled.setTextXY(2, 0);
    SeeedOled.putString(String(p).c_str());
    client.publish("Fiware/EntComp/ParkingCount",String(p).c_str(),true);
  }
}