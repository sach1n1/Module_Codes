#include <Arduino.h>
#include <Wire.h>
#include <SeeedOLED.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <deque>
#include <PubSubClient.h>

const char *ssid     = "MYWIRE-WPA-132";
const char *password = "iefee9aehiem";

#define mqtt_server "192.168.2.247"

unsigned long previousMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int i=0,h,m,b=0;
std::deque<String> busTime;

String str[]={"Dest 1",
                   "Dest 2",
                   "Dest 3",
                   "Dest 4",
                   "Dest 5",
                   "Dest 6",
                   "Dest 7",
                   "Dest 8",
                   "Dest 9",
                  };

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("BusTime","openhabian","openhabian")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

void disp_bus()
{
  if (i>7) 
  {
      i=7;
      SeeedOled.clearDisplay();
  }
  if(h>23)
      h=0;
  if(m>=55)
  {
      h=h+1;
      m=0;
  }
  String min=String(m).c_str();
  String hr=String(h).c_str();
  if(m<10)
    min = "0"+ min;
  if(h<10)
    hr="0" + hr;
  String time= hr + ":" + min;
  int k =random(9);
  String publish = time + "  " + str[k];
  busTime.push_back(publish);
  i=i+1;
  m=m+5;
}


void setup()
{
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
  Wire.begin();
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
  timeClient.update();
  client.setServer(mqtt_server, 1883);
  h=timeClient.getHours();
  m=timeClient.getMinutes();
  SeeedOled.init();  //initialze SEEED OLED display
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
}

int l=0;

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  disp_bus();
  int p=0;
  if(i==8)
  {
        SeeedOled.clearDisplay();
        for(String n : busTime) {
          SeeedOled.setTextXY(p, 0);
          SeeedOled.putString(n.c_str());
          p=p+1;
        }
    client.publish("Fiware/TownHall/BusTime",busTime.front().c_str(),true);
    busTime.pop_front();
    Serial.println(" ");
    delay(5000);
    /*unsigned long currentMillis = millis();
    while(currentMillis - previousMillis <= 5000)
    {
      previousMillis = currentMillis;
    }*/
  }
}