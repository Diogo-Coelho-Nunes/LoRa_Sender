#include <DHT.h>

#include <SPI.h>
#include <LoRa.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 21
#define DHTTYPE DHT11

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 866E6 //Europe

#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16

#define MQ_analog 12

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

DHT dht(DHTPIN,DHTTYPE);

String LoRaMessage = "";
 
int counter = 0;
int readingID = 0;
float t = 0;
float h = 0;
float hic = 0;

//Soil Moisture sensor
int value = 0;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void startOLED(){
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER");
}

void startLoRA(){
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void getReadings(){
  pinMode(MQ_analog, INPUT);
  t = dht.readTemperature();
  h = dht.readHumidity();
  hic = dht.computeHeatIndex(t,h,false);
  value = analogRead(MQ_analog);
}

void sendReadings() {
  //LoRaMessage = String(t) + String(h) + String(value);
  LoRaMessage = String(t) + "/" + String(h) + "&" + String(value) + "#";
  //Send LoRa packet to receiver
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  
  display.print("LoRa packet sent!");
  display.setCursor(0,20);
  display.print("Temperature:");
  display.setCursor(72,20);
  display.print(t);
  
  display.setCursor(0,30);
  display.print("Humidity:");
  display.setCursor(54,30);
  display.print(h);
  
  display.setCursor(0,40);

  display.print("SMoisture:");
  display.setCursor(68,40);
  display.print(value);
  display.display();
  readingID++;
  
}


void setup() {
  Serial.begin(9600);
  dht.begin();
  startOLED();
  startLoRA();
  pinMode(13,OUTPUT);
}

void loop() {
  getReadings();
  sendReadings();
  delay(2000);
}
