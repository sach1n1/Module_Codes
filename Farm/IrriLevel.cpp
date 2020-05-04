// This code is programmed to run on a NodeMCU
// Connect RGB LED as below:
// BLUE -> D2
// RED -> D3
// GREEN -> D4
// GND -> GND
// D8 _/ _ 3v3
// Power via USB

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MYWIRE-WPA-132";
const char* password = "iefee9aehiem";

#define mqtt_server "192.168.2.247"

unsigned long previousMillis = 0;
int i=0;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("IrriLevel","openhabian","openhabian")) {
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
	pinMode(D2, OUTPUT);
  	pinMode(D3, OUTPUT);
  	pinMode(D4, OUTPUT);
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
		switch(i)
		{
  			case 0: 
			  	digitalWrite(D4, HIGH);
				client.publish("Fiware/Farm/IrriLevel","Water Level: Good",true);
			  	i++;
			  	break;
 			case 1:
				digitalWrite(D4, LOW);
				digitalWrite(D2, HIGH);
				client.publish("Fiware/Farm/IrriLevel","Water Level: Okay",true);
				i++;
				break;
 			case 2:
				digitalWrite(D2, LOW);
				digitalWrite(D3, HIGH);
				client.publish("Fiware/Farm/IrriLevel","Water Level: Critcial - Irrigate Now",true);
				i++;
				break;
	  		case 3:
				if(digitalRead(D8)==HIGH)
				{
					digitalWrite(D3, LOW);
					i=0;
				}
				else
				{
					i=3;
				}
				break;
		}
	}
}