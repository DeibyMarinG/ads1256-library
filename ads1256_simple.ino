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
#include "ads1256u.h"

#define CS    53
#define RDY   49
#define RST   48

float canaloffsetdef[8]={0,0,0,0,0,0,0,0};

void calibracionMejorada(){ //ecuentra la media de error por offset por canal
  int canaloffset[8]={0,0,0,0,0,0,0,0};
  int canaloffsetdef[8]={0,0,0,0,0,0,0,0};
  int contador=0;
  Serial.println("Encontrando offset promedio de canal");
  while(contador<1000){
    for(int y=0;y<8;y++)
      canaloffset[y]+=readADSDiff(y,8);
    contador++;
  }
  for(int p=0;p<8;p++){
    canaloffsetdef[p]=floor(canaloffset[p]/contador);
    Serial.println("El offset promedio del canal ");
    Serial.print(p);
    Serial.print("es: ");
    Serial.println(canaloffsetdef[p]);
  }
}


void setup(){
  Serial.begin(9600);
  initADS(CS,RDY,RST);
  calibrateExternalOffset(7,8);
  calibrateInternalGain();
  calibracionMejorada();
  }
  int canal=0;
void loop(){/*
  long value2=readADSDiff(canal,8);
  double resultado2=value2*(5.00/8388608.00);
  Serial.print(resultado2,8);
  Serial.print(" con canal ");
  Serial.println(canal);
  canal++;
  if(canal>7)
    canal=0;
  delay(500);*/
}
