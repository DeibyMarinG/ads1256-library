#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>
#include "ADCDEF.h"

class ADCads1256
{
  uint8_t ADS_RST_PIN; //ADS1256 reset pin
  uint8_t ADS_RDY_PIN; //ADS1256 data ready
  uint8_t ADS_CS_PIN; //ADS1256 chip select

public:
  ADCads1256(uint8_t CS,uint8_t RDY,uint8_t RST);
  void initADS();
  long readADS(byte channel);
  long readADSDiff(byte positiveCh, byte negativeCh);
  void calibrateExternalOffset(byte channel1,byte channel2);
  void calibrateExternalGain(byte channel1,byte channel2);
  void calibrateInternalGain();
  void calibrateInternalOffset();
  void calibrateInternal();
  long readRegister(byte registro);
  void writeRegister(byte registro,byte dato);
  unsigned long readRegister2(unsigned char reg);
  void chooseChannel(byte channel, byte channel2);
  void changeDrate(String rate);
};
