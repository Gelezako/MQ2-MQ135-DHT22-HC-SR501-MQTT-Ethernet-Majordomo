/*
* mq2+mq135+dht22-Majordomo.ino 
* @author Alex Sokolov <admin@gelezako.com>
* @copyright Alex Sokolov http://www.blog.gelezako.com (c)
* @version 0.1 ([July 07, 2017])
*/

#include <SPI.h>                 // Библиотека SPI шины
#include <UIPEthernet.h>          // Ethernet библиотека
#include <PubSubClient.h>        // Библиотека MQTT
#include "DHT.h"

#define DHTPIN 3

const int analogInPin = A0;
const int analogInPin2 = A1; // Указываем пин, к которому подключен датчик MQ-135
const int ledPin = 13;
int sensorValue = 0;
int sensorValue2 = 0; // Объявляем переменную для хранения значений с датчика MQ-135 и задаем ее начальное значение 0
int h = 0;
int t = 0;


// Задаём mac и ip адреса в Локальной сети
byte mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip{192, 168, 0, 177};      //ip Адрес Ethernet Shild'a Arduino
IPAddress server{34,251,42,52};  //ip Адрес для MQTT Брокера

// Шапка Функции Callback (обратный вызов)
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;                                 //Инициализируем Ethernet клиент
PubSubClient client(server, 13977, callback, ethClient);   //Инициализируем MQTT клиент


// Функция Callback
void callback(char* topic, byte* payload, unsigned int length)
{
  // Выделяем необходимое кол-во памяти для копии payload
  byte* p = (byte*)malloc(length);
  // Копирование payload в новый буфер
  memcpy(p, payload, length);
  client.publish("data/mq135", p, length);
  // Освобождаем память
  free(p);
}

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);             // Задаём скорость порта в БОД'ах.
  Ethernet.begin(mac, ip);        // Инициализируем mac, ip
  dht.begin();
}


void loop()
{
    //MQ-2
    if (isnan(sensorValue))     // Проверка удачно ли прошло считывание с MQ-135
  {
    Serial.println("Failed to read from MQ-135");  // Не удалось прочитать
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
        Serial.print("MQ-2 value= "); //Для отслеживания данных с датчика транслируем их в монитор порта
        Serial.println(sensorValue);
        delay(1000);
  }
  
    //MQ-135
    if (isnan(sensorValue2))     // Проверка удачно ли прошло считывание с MQ-135
  {
    Serial.println("Failed to read from MQ-135");  // Не удалось прочитать
  }
  else
  {
        sensorValue2 = analogRead(analogInPin2); //считываем значения с датчика
        if (sensorValue2 >= 100) //и если превышен заданный порог
        {
          digitalWrite(ledPin, HIGH); // то включаем светодиод.
        }
        else // а если нет
        {
          digitalWrite(ledPin, LOW); // то выключаем
        }
        Serial.print("MQ-135 value= "); //Для отслеживания данных с датчика транслируем их в монитор порта
        Serial.println(sensorValue2);
        delay(1000);
  }

//DHT22
  digitalWrite(ledPin, HIGH); // то включаем светодиод.

  h = dht.readHumidity();
  t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  //Serial.print("Heat index: ");
  //Serial.print(hic);
  //Serial.println(" *C ");

  

  if (client.connect("MQClient", "login", "password"))
  {
      static char char_mq2[10];
      static char char_mq135[10];
      dtostrf(sensorValue, 3, 0, char_mq2); 
      dtostrf(sensorValue2, 3, 0, char_mq135); 
      Serial.println("publish");
      client.publish("data/mq2", char_mq2);
      delay(1000);
      client.publish("data/mq135", char_mq135);
      client.disconnect();
  }
  else {  Serial.println("disconect");}

    if (client.connect("DHT22Client", "login", "password"))
  {
      static char char_dht22h[10];      
      static char char_dht22t[10];  
      dtostrf(h, 3, 0, char_dht22h);    
      dtostrf(t, 3, 0, char_dht22t); 
      Serial.println("publish");
      client.publish("data/dht22h", char_dht22h);
      delay(1000);
      client.publish("data/dht22t", char_dht22t);
      client.disconnect();
  }
  else {  Serial.println("disconect");}

  
}
