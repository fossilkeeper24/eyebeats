//used this guide https://www.instructables.com/Guide-to-Using-MAX30102-Heart-Rate-and-Oxygen-Sens/

#include <Wire.h>
#include "MAX30105.h"
#include "heartrate.h"
#include <Adafruit_PCF8574.h> 
#include <Adafruit_SSD1306.h>

MAX30105 particleSensor;
//PCF8574 i2c_ctrl(0x38); //0x20
Adafruit_PCF8574 pcf;

#define screenWidth 128;
#define screenHeight 64

const byte RATE_SIZE = 4; //increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //time at which last beat ocurred

float beatsPerMinute;
int beatAvg;

int BUT1 = A0;
int BUT2 = A1;

//int LED1 = P0;
//int LED2 = P1;
//int LED3 = P2;
//int LED4 = P3;
//int LED5 = P4;
//int LED6 = P5;
//int LED7 = P6;
//int LED8 = P7;


void setup() {
  int x;
  Wire.setClock(400000);
  Serial.begin(9600);

  while(!Serial) {delay(10);}
  Serial.println("Adafruit PCF8574 LED blink test");

  if(!pcf.begin(0x20, &Wire)) {
    Serial.println("Couldn't find PCF8574");
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
  pcf.digitalWrite(0, HIGH);
  delay(250);
  pcf.digitalWrite(1, HIGH);
  delay(250);
  pcf.digitalWrite(2, HIGH);
  delay(250);
  pcf.digitalWrite(3, HIGH);
  delay(250);
  pcf.digitalWrite(4, HIGH);
  delay(250);
  pcf.digitalWrite(5, HIGH);
  delay(250);
  pcf.digitalWrite(6, HIGH);
  delay(250);
  pcf.digitalWrite(7, HIGH);
  delay(250);

  for( x = 0 ; x < 8 ; x++) { //turn the lights off
    pcf.digitalWrite(x, LOW);
  }

  //pcf.end();



  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while(1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  //particleSensor.end();

  display.begin(SSD1306_SWITCHCAPVCC ,0x3C);





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
   
  }

//what the sensor set up guide said to get serial outputs
 Serial.print("IR=");
 Serial.print("irValue");
 Serial.print(", BPM=");
 Serial.print(beatsPerMinute);
 Serial.print(", Avg BPM=");
 Serial.print(beatAvg);
 
 if (irValue < 50000){
   Serial.print("No finger?");
 }

 Serial.println();
}

//#define HRM_ADDR 0x57
//#define IOE_ADDR 0x20
//#define OLED_ADDR 0x3C


//void setup() {
//  Serial.begin(9600);
//  Wire.begin();
//
//}

//void loop() {
//  Wire.beginTransmission(HRM_ADDR);
//  Wire.write(0x56);
//  Wire.endTransmission(false);
//
//}
