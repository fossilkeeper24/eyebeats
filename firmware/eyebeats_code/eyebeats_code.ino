#include <Wire.h>

#define HRM_ADDR 0x57
#define IOE_ADDR 0x20
#define OLED_ADDR 0x3C


void setup() {
  Serial.begin(9600);
  Wire.begin();

}

void loop() {
  Wire.beginTransmission(HRM_ADDR);
  Wire.write(0x56);
  Wire.endTransmission(false);

}
