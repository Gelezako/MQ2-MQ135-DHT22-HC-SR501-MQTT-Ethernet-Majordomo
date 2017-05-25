/*
* MQ2-MQTT-Ethernet-Majordomo.ino 
* @author Alex Sokolov <admin@gelezako.com>
* @copyright Alex Sokolov http://www.blog.gelezako.com (c)
* @version 0.1 ([May 22, 2017])
*/

/*
  Скетч для вывода в Монитор порта с датчиков MQ-2 и MQ-135 и отправки их по MQTT в брокер и MajorDoMo.
*/

#include <SPI.h>                 // Библиотека SPI шины
#include <Ethernet.h>            // Ethernet библиотека
#include <PubSubClient.h>        // Библиотека MQTT

const int analogInPin = A0; // Указываем пин, к которому подключен датчик MQ-2
const int analogInPin2 = A1; // Указываем пин, к которому подключен датчик MQ-135
const int ledPin = 13;
int sensorValue = 0; // Объявляем переменную для хранения значений с датчика MQ-2 и задаем ее начальное значение 0
int sensorValue2 = 0; // Объявляем переменную для хранения значений с датчика MQ-125 и задаем ее начальное значение 0


// Задаём mac и ip адреса в Локальной сети
byte mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip{192, 168, 1, 74};      //ip Адрес Ethernet Shild'a Arduino
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
  client.publish("data/mq2", p, length);
  // Освобождаем память
  free(p);
}


void setup()
{
  // 1 бод равно 0.8 бит/сек
  // 1 бит/сек равно 1.25 бод
  Serial.begin(9600);             // Задаём скорость порта в БОД'ах.
  Serial.println("MQ-2 and MQ-135 test!");  // Тестовое сообщ. при откр. Монитора порта
  Ethernet.begin(mac, ip);        // Инициализируем mac, ip
}


void loop()
{
  //MQ-2
  if (isnan(sensorValue))     // Проверка удачно ли прошло считывание с MQ2
  {
    Serial.println("Failed to read from MQ-2");  // Не удалось прочитать
  }
  else
  {
        sensorValue = analogRead(analogInPin); //считываем значения с датчика
        if (sensorValue >= 150) //и если превышен заданный порог
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
        sensorValue = analogRead(analogInPin2); //считываем значения с датчика
        if (sensorValue >= 100) //и если превышен заданный порог
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

  if (client.connect("MQ2Client", "jthjoimh", "T-KYqa3mDF8v"))
  {
      // Преобразуем переменные для отправки в MQTT в Брокер
      static char char_mq2[10];      // Переменная для перевода из int в char
      static char char_mq135[10];      // Переменная для перевода из int в char
      
      dtostrf(sensorValue, 3, 0, char_mq2);    // Перевод из int в char
      dtostrf(sensorValue2, 3, 0, char_mq135);    // Перевод из int в char
      //Отправка данных по MQTT в Брокер
        Serial.println("publish");
      client.publish("data/mq2", char_mq2); //отправляем в Брокер значения
      client.publish("data/mq135", char_mq135); //отправляем в Брокер значения
      delay(3000);              // Отправка данных в Брокер раз в 5 секунд
      client.disconnect();      // Отключиться
  }
  else {  Serial.println("disconect");}
}
