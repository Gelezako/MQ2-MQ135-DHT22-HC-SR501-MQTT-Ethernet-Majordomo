#include <SPI.h>                 // Библиотека SPI шины
#include <UIPEthernet.h>          // Ethernet библиотека
#include <PubSubClient.h>        // Библиотека MQTT

// =================================
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED_PIN    3
#define LED_COUNT 30
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}


// ==================================
int sensorValue = 0;
#define pin 4
#define LedPin 13

byte mac[]    = { 0x00, 0xBB, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip{192,168,88,203};
IPAddress server{192,168,88,206};

void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient; 
PubSubClient client(server, 1883, callback, ethClient);

void callback(char* topic, byte* payload, unsigned int length)
{
  client.publish("data/badroom/nightstand/HCSR501", payload, length);
}

void setup() {
  // ===================
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  // ===================

  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
}

void loop() {
  strip.show();
  digitalWrite(LedPin,LOW);
  int sensorValue = digitalRead(pin); 
  Serial.print("move =  ");
  Serial.println(sensorValue);


  if(isnan(sensorValue))
  {
    Serial.println("Failed to read from MQ-2");  // Не удалось прочитать
  }
  else
  {
    if(sensorValue == 1){
      Serial.print("led block ");
      colorWipe(strip.Color(255,   0,   0), 50); // Red
      colorWipe(strip.Color(  0, 255,   0), 50); // Green
      colorWipe(strip.Color(  0,   0, 255), 50); // Blue
      delay(4000);
      strip.clear();
      strip.show();
      //delay(10000); //sleep 1 min
   }
    if (client.connect("hcClient", "", ""))
      {
      //static char char_hc[10];
      //dtostrf(sensorValue, 3, 0, char_hc);
      Serial.println("publish");
      //digitalWrite(LedPin, HIGH);
      //client.publish("data/badroom/nightstand/HCSR501", char_hc);
      client.publish("data/badroom/nightstand/HCSR501", sensorValue ? "1" : "0");
      client.disconnect();
      //delay(10000); //sleep 1 min
      }
  else {  
    //digitalWrite(LedPin,LOW);
    Serial.println("disconnect");}
  }   
}
