#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>
#include "ADCDEF.h"

void initADS(uint8_t CS,uint8_t RDY,uint8_t RST);
//void initADS();
long readADS(byte channel);
long readADSDiff(byte positiveCh, byte negativeCh);
void calibrateExternalOffset(byte channel1,byte channel2);
void calibrateExternalGain(byte channel1,byte channel2);
void calibrateInternalGain();
void calibrateInternalOffset();
void calibrateInternal();
long readRegister(byte registro);
unsigned long readRegister2(unsigned char reg);
