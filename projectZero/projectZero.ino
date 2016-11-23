// ARDUINO NANO 5V
// HC-60        RX=2 TX=5 GND VND=5V
// OLED         SCK=A5 SDA=A4 GND VND=5V
// DHT22        DAT=A3

#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// HC-60
#define RXPIN 5
#define TXPIN 2
SoftwareSerial BlueSerial(RXPIN, TXPIN);

//DHT22
#define DHTPIN 3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
float hum;  //Stores humidity value
float temp; //Stores temperature value

//OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {
  
  //HC60
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);
  BlueSerial.begin(9600);
  BlueSerial.println("BlueSerial start!");

  //SERIAL
  //Serial.begin(9600);
  //Serial.println("Serial start!");
  
  //OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  // DHT22
  dht.begin();
}
String message="";
int maxm = 20;
void loop() {
  // put your main code here, to run repeatedly:
 
   if(BlueSerial.available())
    {
      // Read off all bytes
      char val = (char)BlueSerial.read();
      message = message + val;
      if(message.length() > maxm){
        message= message.substring(message.length() - maxm);
      }
    }
    //delay(1000);
    hum = dht.readHumidity();
    temp= dht.readTemperature();

     // DISPLAY
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(temp);
  display.print((char)247);
  display.println("C");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,9);
  display.println("Blue Message:");
  display.print(message);
  display.display();
}
