// This code is programmed to run on a Wio Link
// Programmed to run on two Ultrasonic Rangers (Can be expanded to 4)
// Connect a Grove SeeedOLED screen on the I2C port of the Wio Link
// Connect Ultrasonic Rangers to D2 and D1 part
// Connect two more Ultrasonic rangers to D0 and A0 port to expand
// Power via USB or Battery

#include <Arduino.h>
#include "Ultrasonic.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SeeedOLED.h>

Ultrasonic ultrasonic1(13);
Ultrasonic ultrasonic2(12);

const char* ssid = "MYWIRE-WPA-132";
const char* password = "iefee9aehiem";

#define mqtt_server "192.168.2.247"

unsigned long previousMillis = 0,previousMillis1 = 0;
const long interval = 1500;

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

void callback(char* topic, byte* payload, unsigned int length){
  char k=(char)payload[0];
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis1 >= interval) {
    previousMillis1 = currentMillis;
    SeeedOled.clearDisplay();          
    SeeedOled.setNormalDisplay();      
    SeeedOled.setPageMode();           
    SeeedOled.setTextXY(0, 0);         
    SeeedOled.putString("Number of ships");
    SeeedOled.setTextXY(1, 0);
    SeeedOled.putString("Available:");
    SeeedOled.setTextXY(2, 0);
    SeeedOled.putString(String(k).c_str());
    }
}

void setup()
{
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
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
  client.setCallback(callback);
}
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int state1=0,state2=0,i=0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println("No. of cars available :");
    state1 = (ultrasonic1.MeasureInInches() < 2)?1:0;
    state2 = (ultrasonic2.MeasureInInches() < 2)?1:0;
    if(state1 ==1)
      i++;
    if(state2 ==1)
      i++;
    Serial.println(i);
    client.publish("Fiware/Dock/ShipParkingLot",String(i).c_str(),true);
    client.subscribe("Fiware/Dock/ShipParkingLot");
  }
}