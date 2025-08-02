//used this guide https://www.instructables.com/Guide-to-Using-MAX30102-Heart-Rate-and-Oxygen-Sens/
//something to look at https://projecthub.arduino.cc/najad/interfacing-and-displaying-images-on-oled-08b4f2
//example code for oled featherwing https://github.com/adafruit/Adafruit_SSD1306/blob/master/examples/OLED_featherwing/OLED_featherwing.ino

#include <Wire.h>
#include "MAX30105.h"
#include "heartrate.h"
#include <Adafruit_PCF8574.h> 
#include <Adafruit_SSD1306.h>
#include <SPI.h>

MAX30105 particleSensor;
//PCF8574 i2c_ctrl(0x38); //0x20
Adafruit_PCF8574 pcf;

#define screenWidth 128;
#define screenHeight 64;
#define HRM_ADDR 0x57;

const byte RATE_SIZE = 4; //increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //time at which last beat ocurred

float beatsPerMinute;
int beatAvg;

int BUT1 = A0;
int BUT2 = A1;

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define WIRE Wire

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

void setup() {
  int x;
  //Wire.setClock(400000);
  Serial.begin(9600);

  Wire.begin();

  // text display tests
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.print("connected!");
  display.println("IP: 10.0.1.23");
  display.println("Sending val #0");
  display.setCursor(0,0);
  display.display(); // actually display all of the above
  delay(100);

  initMPU();

  while(!Serial) {delay(10);}
  display.print("Adafruit PCF8574 LED blink test");

  if(!pcf.begin(0x20, &Wire)) {
    display.print("Couldn't find PCF8574");
    while(1);
  }
  pcf.pinMode(0, OUTPUT);
  pcf.pinMode(1, OUTPUT);
  pcf.pinMode(2, OUTPUT);
  pcf.pinMode(3, OUTPUT);
  pcf.pinMode(4, OUTPUT);
  pcf.pinMode(5, OUTPUT);
  pcf.pinMode(6, OUTPUT);
  pcf.pinMode(7, OUTPUT);

  //set rp2040 input pins
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);

  pcf.begin();

  //lights turn on one by one
  for( x = 0 ; x < 8 ; x++) { //turn the lights on
    pcf.digitalWrite(x, HIGH);
    delay(250);
  }

  for( x = 0 ; x < 8 ; x++) { //turn the lights off
    pcf.digitalWrite(x, LOW);
  }

  //pcf.end();

  display.print("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    display.print("MAX30102 was not found. Please check wiring/power. ");
    while(1);
  }
  display.print("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) { //We sensed a beat!
   long delta = millis() - lastBeat;
   lastBeat = millis();
 
   beatsPerMinute = 60 / (delta / 1000.0);

   if (beatsPerMinute < 255 && beatsPerMinute > 20) {
     rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
     rateSpot %= RATE_SIZE; //Wrap variable

     //Take average of readings
     beatAvg = 0;
     for (byte x = 0 ; x < RATE_SIZE ; x++)
       beatAvg += rates[x];
     beatAvg /= RATE_SIZE;
    }

  if(!digitalRead(BUTTON_A)) display.print("A");
  if(!digitalRead(BUTTON_B)) display.print("B");
  if(!digitalRead(BUTTON_C)) display.print("C");
  delay(10);
  yield();
  display.display();
   
  }

//what the sensor set up guide said to get serial outputs  but make it display output
 display.print("IR=", irValue, ", BPM = ", beatsPerMinute, ", Avg BPM = ", beatAvg);
 
 if (irValue < 50000){
   display.print("No finger?");
 }

 display.print();
}

void initHRM() {
  Wire.beginTransmission(HRM_ADDR);
  // Wire.write(0x6B);  //sleep mode address? idk if there is one for hrm :idk:
  Wire.write(0x00);  //hex to start sensor

  Wire.write(0x1B);  //calibration address
  Wire.write(0x00);  //hex for +/- 250 range
  Wire.endTransmission();
}