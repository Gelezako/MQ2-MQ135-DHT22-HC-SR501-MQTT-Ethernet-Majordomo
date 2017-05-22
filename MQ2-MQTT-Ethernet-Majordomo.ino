/*
* MQ2-MQTT-Ethernet-Majordomo.ino 
* @author Alex Sokolov <admin@gelezako.com>
* @copyright Alex Sokolov http://www.blog.gelezako.com (c)
* @version 0.1 ([May 22, 2017])
*/

#include <SPI.h>                
#include <Ethernet.h>           
#include <PubSubClient.h>   

const int analogInPin = A0; // Аналоговый пин к датчику
const int ledPin = 13;
int sensorValue = 0; // Объявляем переменную для хранения значений с датчика 

// Задаём mac и ip адреса в Локальной сети
byte mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip{192, 168, 1, 74};      //ip Адрес Ethernet Shild'a Arduino
IPAddress server{34,251,42,52};  //ip Адрес для MQTT Брокера

void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;                                
PubSubClient client(server, 13977, callback, ethClient); 

void callback(char* topic, byte* payload, unsigned int length)
{
  byte* p = (byte*)malloc(length);
  memcpy(p, payload, length);
  client.publish("data/mq2", p, length);
  free(p);
}


void setup()
{
  Serial.begin(9600);            
  Serial.println("MQ2 test!");  
  Ethernet.begin(mac, ip); 
}


void loop()
{
  if (isnan(sensorValue))     // Проверка удачно ли прошло считывание с MQ2
  {
    Serial.println("Failed to read from MQ2");  // Не удалось прочитать
  }
  else
  {
        sensorValue = analogRead(analogInPin); //считываем значения с датчика
        if (sensorValue >= 100) //и если превышен заданный порог
        {
          digitalWrite(ledPin, HIGH); // то включаем светодиод.
        }
        else // а если нет
        {
          digitalWrite(ledPin, LOW); // то выключаем
        }
        Serial.print("MQ2 value= "); //Для отслеживания данных с датчика транслируем их в монитор порта
        Serial.println(sensorValue);
        delay(1000);
  }

  if (client.connect("MQ2Client", "login", "pass"))
  {
      // Преобразуем переменные для отправки в MQTT в Брокер
      static char char_gas[10];      // Переменная для перевода из int в char
      dtostrf(sensorValue, 3, 0, char_gas);    // Перевод из int в char
      //Отправка данных по MQTT в Брокер
      Serial.println("publish");
      client.publish("data/mq2", char_gas); //отправляем в Брокер значения
      delay(3000);              // Отправка данных в Брокер раз в 5 секунд
      client.disconnect();      // Отключиться
  }
  else {  Serial.println("disconect");}
}
