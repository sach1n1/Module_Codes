// This code is programmed to run on a NodeMCU
// Connect an Adfruit Color Sensor as below:
// SDA -> D2
// SCL -> D2
// GND -> GND
// 3v3 -> 3v3
// Power via USB

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

const char* ssid = "MYWIRE-WPA-132";
const char* password = "iefee9aehiem";

#define mqtt_server "192.168.2.247"

unsigned long previousMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("SnowWarn","openhabian","openhabian")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}


void setup()
{
	Serial.begin(9600);
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
	if (tcs.begin()) {
		Serial.println("Found sensor");
	} else {
		Serial.println("No TCS34725 found ... check your connections");
		while (1);
	}
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
		uint16_t r, g, b, c, colorTemp, lux;
		tcs.getRawData(&r, &g, &b, &c);
		colorTemp = tcs.calculateColorTemperature(r, g, b);
		lux = tcs.calculateLux(r, g, b);
		if(colorTemp>7000 && colorTemp<8000)
    	{
      		client.publish("Fiware/SkiRes/SnowWarn","Snow Ahead: Drive Carefully",true);
    	}else{
      		client.publish("Fiware/SkiRes/SnowWarn","No Snow: Proceed Ahead",true);
    	}
	}
}