/*
* HC-SR501-Ethernet-Majordomo.ino 
* @author Alex Sokolov <admin@gelezako.com>
* @copyright Alex Sokolov http://www.blog.gelezako.com (c)
* @version 0.1 ([July 29, 2017])
*/

/*
  Скетч определяет движение и отправки их по MQTT в брокер и MajorDoMo.
*/

#include <SPI.h>                 // Библиотека SPI шины
#include <UIPEthernet.h>          // Ethernet библиотека
#include <PubSubClient.h>        // Библиотека MQTT

int sensorValue = 0;
#define pin 4
#define LedPin 13

byte mac[]    = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip{192, 168, 0, 80};
IPAddress server{34,251,42,52};

void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient; 
PubSubClient client(server, 13977, callback, ethClient);

void callback(char* topic, byte* payload, unsigned int length)
{
  byte* p = (byte*)malloc(length);
  memcpy(p, payload, length);
  client.publish("data/HCSR501", p, length);
  free(p);
}

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
}

void loop() {
  int sensorValue = digitalRead(pin); 
  Serial.print("move =  ");
  Serial.println(sensorValue);

  if(sensorValue == HIGH)
  {
    digitalWrite(LedPin, HIGH);
     if (client.connect("hcClient", "login", "password"))
      {
      static char char_hc[10];
      dtostrf(sensorValue, 3, 0, char_hc);
      Serial.println("publish");
      client.publish("data/HCSR501", char_hc);
      client.disconnect();
      delay(300000); //sleep 5 min
      }
  else {  Serial.println("disconnect");}
  }
  else
  {
    digitalWrite(LedPin,LOW);
  }
  
   
}
