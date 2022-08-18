//#define USE_ARDUINO_INTERRUPTS false
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>
#include "OakOLED.h"
//#include <WiFi.h>
#include <ThingSpeak.h>

//const char ssid[] = "JIT-Student-WLAN";
//const char password[] = "";
WiFiClient client;

const long CHANNEL = 1733241; //In this field, enter the Channel ID
const char *WRITE_API = "UXHVKVSIYNLNK03F";// Enter the Write API key 

long prevMillisThingSpeak = 0;
int intervalThingSpeak = 20000; // 15 seconds to send data to the dashboard

//const int OUTPUT_TYPE = SERIAL_PLOTTER;
OakOLED oled;
//#define I2C_SDA A4
//#define I2C_SCL A5
#define REPORTING_PERIOD_MS 1000
const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t max30100_address = 0x57;
uint8_t irmlx90614_address = 0x5A;
uint32_t tsLastReport = 0;
char auth[] = "sLcaFf5Q_AjQ9BOCTFT7Gkb0mCGvxSWm";
char ssid[] = "JIT-Student-WLAN";
char pass[] = "";
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PulseOximeter pox;

void setup() {
Serial.begin(9600);
Wire.begin();
mlx.begin();
pox.begin();
//delay(2000);
Serial.println();
Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
    oled.drawBitmap( 60, 20, bitmap, 28, 28, 1);
    oled.display();
    oled.begin();
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
 
    oled.println("Initializing..");
    oled.display();
    WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  } 

}

void loop() {
  Blynk.run();
//printTemp();
oled.clearDisplay();
oled.setTextSize(2);
oled.setTextColor(1);
oled.setCursor(0, 0);
         oled.println("Temp:");
         oled.setTextSize(2);
oled.setTextColor(1);
oled.setCursor(70, 0);
int tmp = mlx.readObjectTempC();
         oled.println(mlx.readObjectTempC());
          if (tmp < 100 && tmp > 10){
      if (millis() - prevMillisThingSpeak > intervalThingSpeak) 
      {
      ThingSpeak.setField(1, tmp);
      int x = ThingSpeak.writeFields(CHANNEL, WRITE_API);
      if (x == 200) 
      {
        Serial.println("Channel update successful.");
      }
      else 
      {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
        prevMillisThingSpeak = millis();
      }        
    }    
    oled.setTextSize(2);
oled.setTextColor(1);
oled.setCursor(0, 15);
         oled.println("SPO2:  0%");
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(15, 70);
         oled.println(pox.getSpO2());
         oled.setTextSize(2);
oled.setTextColor(1);
oled.setCursor(0, 30);
         oled.println("BPM:   0");
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(30, 70);
         oled.println(pox.getHeartRate());
         oled.setTextSize(2);
oled.setTextColor(1);
oled.setCursor(0, 45);
         oled.println("RR:    0");
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(45, 70);
         oled.println("18");
    oled.display();
//printFreq();
//delay(1000);
int sp =96;
int pr=68;
int rr=14;
Blynk.virtualWrite(V5, sp);
Blynk.virtualWrite(V7, tmp);
Blynk.virtualWrite(V6, pr);
Blynk.virtualWrite(V8, rr);
}

void printTemp(){
Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(0, 0);
         oled.println("Temperature =");
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(0, 16);
         oled.println(mlx.readObjectTempC());
}

void printFreq(){
pox.update();

if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");
    oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(0, 30);
         oled.println("SPO2 =");
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(0, 45);
         oled.println(pox.getSpO2());
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(0, 60);
         oled.println("PR =");
         oled.setTextSize(1);
oled.setTextColor(1);
oled.setCursor(0, 75);
         oled.println(pox.getHeartRate());
   printTemp();
    tsLastReport = millis();
}
}
