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
/**
This is an example where I used 2 ADC1256 connected to Arduino Due
I even use the DAC of the arduino Due to write signals, and  the MPC4725 when I burn a DAC pin.

This example was used for a communication with a QT aplication where the GUI sends through serial some instructions and the microcontroller responds with an OK.
This implementation was used for transistor characterization, on this version 1.0 the system works but a better SYNC arduino-gui was needed.
**/


#define calibrationchannel 8 //this channel connected to analog ground

#include "ADCads1256.h"
#include "ads1256u.h"
#include <Wire.h>
//#include <Adafruit_MCP4725.h>
//#define DAC_RESOLUTION    (12)
#define CS    41
#define RDY   43
#define RST   39
#define CS2   35
#define RDY2  37
#define RST2  33

#define VCCOFF 1427
#define VINOFF 1577
//Adafruit_MCP4725 MCPDAC;

float canaloffsetdef[8]={0,0,0,0,0,0,0,0};
ADCads1256 ADS1(CS,RDY,RST);
ADCads1256 ADS2(CS2,RDY2,RST2);
bool START=0;
int canal=0;
int canal2=0;
long value1[7],value2[4];
double resultado1;
bool rdytopush=0;
int curva=0;

void setup(){
  pinMode(23,OUTPUT);
  digitalWrite(23,HIGH);

  analogWriteResolution(12);
  Serial.begin(115200);
  ADS1.initADS();
  ADS2.initADS();
  analogWrite(DAC1,VCCOFF);
  analogWrite(DAC0,VINOFF);
  delay(100);
  //MCPDAC.begin(0x60);
  //MCPDAC.setVoltage(0,false);
  //delay(1000);
  Serial.println("ADSRDY");

  }
int i,j,k,l;
String cadenalectura,cadenaDAC0,cadenaDAC1;
long DAC0resol,DAC1resol;
int inputpar[5];
String cadenaEnviar="";

void loop(){
  if(START==1){
      switch (curva) {
        case 1:
              inputpar[0]=ADS1.readADS(5);  //VBE bajas
              inputpar[1]=ADS2.readADS(2);  //VBE altas
              inputpar[2]=ADS1.readADS(0);  //IB
              inputpar[3]=ADS1.readADS(3);  //VCE bajas
              inputpar[4]=ADS2.readADS(0);  //VCE altas
              cadenaEnviar="INPUT,"+String(inputpar[0])+','+String(inputpar[1])+','+String(inputpar[2])+','+String(inputpar[3])+','+String(inputpar[4])+',';
              Serial.println(cadenaEnviar);
        break;

      }

  }
cadenalectura="";
cadenaDAC0="";
cadenaDAC1="";
  if(Serial.available()>3){
      cadenalectura=Serial.readString();
      Serial.println(cadenalectura);

      if(cadenalectura.indexOf("ADSINPUT")>=0){
      curva=1;
      Serial.println("INPUTOK");
      }

      if(cadenalectura.indexOf("ADSOUTPUT")>=0)
      curva=2;
      if(cadenalectura.indexOf("ADSTRANSFER")>=0)
      curva=3;
      if(cadenalectura.indexOf("ADSREACTION")>=0)
      curva=4;

      if(cadenalectura.indexOf("VCC,")>=0){
        cadenaDAC0=cadenalectura.substring(cadenalectura.indexOf(',')+1);
        DAC0resol=cadenaDAC0.toInt();
        analogWrite(DAC1,DAC0resol);
        Serial.println(cadenaEnviar+'\0'+'\n');
      }
      if(cadenalectura.indexOf("VIN,")>=0){
        cadenaDAC1=cadenalectura.substring(cadenalectura.indexOf(',')+1);
        //Serial.println(cadenaDAC1);
        DAC1resol=cadenaDAC1.toInt();
        //Serial.println(DAC1resol);
        //digitalWrite(23,HIGH);
        //MCPDAC.setVoltage(DAC1resol,false);
        analogWrite(DAC0,DAC1resol);
        Serial.println("VINOK");

      }
      if(cadenalectura.indexOf("RATE,")>=0){
        cadenaDAC0=cadenalectura.substring(cadenalectura.indexOf(',')+1);
        ADS1.changeDrate(cadenaDAC0);
        ADS2.changeDrate(cadenaDAC0);
      }
      if(cadenalectura.indexOf("ADSSTART")>=0){
        Serial.println("STARTOK");
        START=1;
      }
      if(cadenalectura.indexOf("ADSSTOP")>=0){
        Serial.println("STOPOK");
        START=0;
      }
      if(cadenalectura.indexOf("ADSOFF")>=0){
        START=0;
        Serial.println("OFFOK");
        analogWrite(DAC1,VCCOFF);
        analogWrite(DAC0,VINOFF);
        //MCPDAC.setVoltage(0,false);
      }

      if(cadenalectura.indexOf("ADSCAL")>=0){
        Serial.println("CALOK");
        ADS1.calibrateExternalOffset(6,8);
        ADS2.calibrateExternalOffset(3,8);
        ADS1.calibrateInternalGain();
        ADS2.calibrateInternalGain();
        unsigned long statusreg=ADS1.readRegister2(STATUS);
        Serial.print ("El registro status ");
        Serial.println(statusreg);
        unsigned long dratereg=ADS1.readRegister2(DRATE);
        Serial.print ("El registro data rate ");
        Serial.println(dratereg);
        unsigned long adconreg=ADS1.readRegister2(ADCON);
        Serial.print ("El registro ADCON ");
        Serial.println(adconreg);
        unsigned long statusreg2=ADS2.readRegister2(STATUS);
        Serial.print ("El registro status ");
        Serial.println(statusreg2);
        unsigned long dratereg2=ADS2.readRegister2(DRATE);
        Serial.print ("El registro data rate ");
        Serial.println(dratereg2);
        unsigned long adconreg2=ADS2.readRegister2(ADCON);
        Serial.print ("El registro ADCON ");
        Serial.println(adconreg2);
      }
  }
  delay(2);
}


void calibracionMejorada(int num){ //encuentra la media de error por offset por canal
  long canaloffset[8]={0,0,0,0,0,0,0,0};
  int contador=0;
  Serial.println("Encontrando offset promedio de canal");
  long tiempoactual=millis();
  while(contador<num){
    for(int y=0;y<8;y++)
      canaloffset[y]+=ADS1.readADSDiff(y,8);
    contador++;
  }
  for(int p=0;p<8;p++){
    canaloffsetdef[p]=floor(canaloffset[p]/contador);
    Serial.print("El offset promedio del canal ");
    Serial.print(p);
    Serial.print("es: ");
    Serial.println(canaloffsetdef[p]);
  }
  Serial.print("Tiempo tomado en ms");
  tiempoactual=millis()-tiempoactual;
  Serial.println(tiempoactual);

}
