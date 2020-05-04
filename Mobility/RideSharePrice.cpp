// This code is programmed to run on a Wio Node
// Connect a Grove SeeedOLED screen on the I2C1 port of the Wio Node and a Grove Hall Sensor to the D0 port of the Win Node.
// Power via USB or Battery

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SeeedOLED.h>


const char* ssid = "MYWIRE-WPA-132";
const char* password = "iefee9aehiem";

#define mqtt_server "192.168.2.247"

unsigned long previousMillis = 0,previousMillis1 = 0;
const long interval = 5000;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("RideSharePrice","openhabian","openhabian")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  char k[5];
  int i=0;
  for (i = 0; i < length; i++)
  {
    k[i]=(char)payload[i];
  }
  k[i]='\0';
  int p = atoi(k);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis1 >= interval) {
    previousMillis1 = currentMillis;

    if(p<=200){
      SeeedOled.clearDisplay();          
      SeeedOled.setNormalDisplay();      
      SeeedOled.setPageMode();           
      SeeedOled.setTextXY(0, 0);
      SeeedOled.putString("Normal Prices");
      client.publish("Fiware/Mobility/CarSharePrices","Normal Prices for Rides!",true);
    }else{
      SeeedOled.clearDisplay();          
      SeeedOled.setNormalDisplay();      
      SeeedOled.setPageMode();           
      SeeedOled.setTextXY(0, 0);         
      SeeedOled.putString("Half Prices");
      client.publish("Fiware/Mobility/CarSharePrices","Half price for rides!",true);
    }
    Serial.println(k);
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
  pinMode(3, INPUT);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    client.subscribe("Fiware/Addix/ParticulateMatter");
  }
}