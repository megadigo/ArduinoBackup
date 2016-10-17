// ARDUINO NANO 5V
// HC-60        RX=3 TX=2 GND VND=5V
// OLED         SCK=A5 SDA=A4 GND VND=5V
// DHT22        DAT=A1

#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// HC-60
#define RXPIN 2
#define TXPIN 3
SoftwareSerial BlueSerial(RXPIN, TXPIN);

//DHT22
#define DHTPIN 2     // what pin we're connected to
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
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
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
  Serial.begin(9600);
  Serial.println("Serial start!");
  
  //OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  // DHT22
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
   if(BlueSerial.available() > 0)
    {
        // Read off all bytes
        char val = BlueSerial.read();
        Serial.print(val);
        delay(100);  
    }
    delay(1000);
    hum = dht.readHumidity();
    temp= dht.readTemperature();

     // DISPLAY
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Temperatura:");
  display.println(temp);
  display.display();

}
