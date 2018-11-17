/* ADS1256 simple library for Arduino
   ADS1256, datasheet: http://www.ti.com/lit/ds/sbas288j/sbas288j.pdf
   connections to Atmega328 (UNO)
    CLK  - pin 13
    DIN  - pin 11 (MOSI)
    DOUT - pin 12 (MISO)
    CS   - pin 10
    DRDY - pin 9
    RESET- pin 8 (or tie HIGH?)
    DVDD - 3V3
    DGND - GND
*/
#include "ads1256uq.h"

void setup(){
  Serial.begin(9600);
  initADS();
}
void loop(){
  byte chp=0,chn=1;
  readADS(chp);
}
