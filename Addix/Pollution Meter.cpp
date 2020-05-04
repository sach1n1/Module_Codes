#include "Arduino.h"
#include "Seeed_HM330X.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MYWIRE-WPA-132";
const char* password =  "iefee9aehiem";

#define mqtt_server "192.168.2.247"

WiFiClient espClient;
PubSubClient client(espClient);

HM330X sensor;
u8 buf[30];

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("PM_Level_Monitor","openhabian","openhabian")) {
      Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

unsigned long previousMillis = 0;
const char *str = "PM2.5 concentration(CF=1,Standard particulate matter,unit:ug/m3): ";

HM330XErrorCode print_result(const char *str, uint16_t value) {
    if (NULL == str) {
        return ERROR_PARAM;
    }
    Serial.print(str);
    Serial.println(value);
    return NO_ERROR;
}


HM330XErrorCode parse_result(uint8_t *data) {
    uint16_t value = 0;
    if (NULL == data)
        return ERROR_PARAM;
    value = (uint16_t) data[3 * 2] << 8 | data[3 * 2 + 1];  
    print_result(str, value);
    client.publish("Fiware/Addix/ParticulateMatter",String(value).c_str(),true);
    return NO_ERROR;
}

HM330XErrorCode parse_result_value(uint8_t *data) {
    if (NULL == data) {
        return ERROR_PARAM;
    }
    uint8_t sum = 0;
    for (int i = 0; i < 28; i++) {
        sum += data[i];
    }
    if (sum != data[28]) {
        Serial.println("wrong checkSum!!!!");
    }
    return NO_ERROR;
}

void setup()
{
    Serial.begin(9600);
    pinMode(15,OUTPUT);
    digitalWrite(15,HIGH);
    delay(100);
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
    Serial.println("Serial start");
    if(sensor.init())
    {
        Serial.println("HM330X init failed!!!");
        while(1);
    }

}

void loop()
{
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5000)
    {
        previousMillis = currentMillis;
        if(sensor.read_sensor_value(buf,29))
        {
            Serial.println("HM330X read result failed!!!");
        }
        parse_result_value(buf);
        parse_result(buf);
    }
}