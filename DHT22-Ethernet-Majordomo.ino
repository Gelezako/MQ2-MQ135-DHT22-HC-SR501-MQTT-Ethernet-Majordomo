/*
* DHT22-Ethernet-Majordomo.ino 
* @author Alex Sokolov <admin@gelezako.com>
* @copyright Alex Sokolov http://www.blog.gelezako.com (c)
* @version 0.1 ([July 07, 2017])
*/

/*
  Скетч для вывода данных в монитор порта с датчика DHT22 и отправки их по MQTT в брокер и MajorDoMo.
*/


#include "DHT.h"
#include <SPI.h>                 // Библиотека SPI шины
#include <UIPEthernet.h>          // Ethernet библиотека
#include <PubSubClient.h>        // Библиотека MQTT

#define DHTPIN 3     // what digital pin we're connected to

const int ledPin = 13;
int sensorValue = 0; // Объявляем переменную для хранения значений с датчика DHT22 и задаем ее начальное значение 0


// Задаём mac и ip адреса в Локальной сети
byte mac[]    = { 0xDE, 0xED, 0xDE, 0xFE, 0xFE, 0xED };
IPAddress ip{192, 168, 0, 179};      //ip Адрес Ethernet Shild'a Arduino
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
  client.publish("data/dht22", p, length);
  // Освобождаем память
  free(p);
}

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT22 test!");
  Ethernet.begin(mac, ip);        // Инициализируем mac, ip
  dht.begin();
}

void loop() {
  digitalWrite(ledPin, HIGH); // то включаем светодиод.
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  //float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  //Serial.print(f);
  //Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  //Serial.print(hif);
  //Serial.println(" *F");

    if (client.connect("DHT22Client", "login", "password"))
  {
      // Преобразуем переменные для отправки в MQTT в Брокер
      static char char_dht22h[10];      // Переменная для перевода из int в char
      static char char_dht22t[10];      // Переменная для перевода из int в char
      dtostrf(h, 3, 0, char_dht22h);    // Перевод из int в char
      dtostrf(t, 3, 0, char_dht22t);    // Перевод из int в char
      //Отправка данных по MQTT в Брокер
        Serial.println("publish");
      client.publish("data/dht22h", char_dht22h); //отправляем в Брокер значения
      client.publish("data/dht22t", char_dht22t); //отправляем в Брокер значения
      delay(3000);              // Отправка данных в Брокер раз в 3 секунд
      client.disconnect();      // Отключиться
  }
  else {  Serial.println("disconect");}
}
