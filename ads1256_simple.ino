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
#include <SPI.h>
#include "ads1256uq.h"
#define ADS_SPISPEED   1536000 //Tclkin*5
#define tCLKIN         1.302083333e-7 // cristal oscilador
#define ADS_RST_PIN    48 //ADS1256 reset pin
#define ADS_RDY_PIN    49 //ADS1256 data ready
#define ADS_CS_PIN     53 //ADS1256 chip select
// 11, 12 and 13 are taken by the SPI

void initADS();
long readADS(byte channel);
long readADSDiff(byte positiveCh, byte negativeCh);




void setup(){
  Serial.begin(9600);
  initADS();
}
long valor=0;
void loop(){
  byte chp=0,chn=1;
  readADSDiff(chp,chn);
  //delay(3000);
  }

void writeRegister(byte registro,byte dato){
  digitalWrite(ADS_CS_PIN, LOW); // ser maestro
  delayMicroseconds(50);
  while (digitalRead(ADS_RDY_PIN));// esperar hasta que este listo
  SPI.transfer(WREG|registro);
  SPI.transfer(0);
  SPI.transfer(dato);
  delayMicroseconds(10);
}

void initADS(){
  pinMode(ADS_CS_PIN, OUTPUT);
  pinMode(ADS_RDY_PIN, INPUT);
  pinMode(ADS_RST_PIN, OUTPUT);

  //Reset ads1256
  digitalWrite(ADS_RST_PIN, LOW);
  delay(200); // LOW at least 4 clock cycles of onboard clock. 100 microsecons is enough
  digitalWrite(ADS_RST_PIN, HIGH); // now reset to deafult values
  delay(150);
  SPI.begin();

  digitalWrite(ADS_CS_PIN, LOW); // ser maestro
  //inicio de comunicacion
  while (digitalRead(ADS_RDY_PIN)) {}  // espera que el ads este listo //wait for ready_line to go low
  SPI.beginTransaction(SPISettings(ADS_SPISPEED, MSBFIRST, SPI_MODE1));
  delayMicroseconds(10);


  //Reset to Power-Up Values (FEh)
  SPI.transfer(RESET);
  delayMicroseconds(100);

  writeRegister(STATUS,BUFFER_OFF_ACAL_ON);  //sin buffer sin calibracion automatica
  byte adcon_data=0x20|ADS1256_GAIN_1; //Clock Out Frequency = fCLKIN, Sensor Detect OFF, gain 1
  writeRegister(ADCON,adcon_data);

  //Data de muestreo
  writeRegister(DRATE,ADS1256_DRATE_2_5SPS);

  digitalWrite(ADS_CS_PIN, HIGH); //avisa que termina de escribir
  Serial.println("ADS1256 configured");
}

long readADS(byte channel) {
   long adc_val = 0; // unsigned long is on 32 bits
  digitalWrite(ADS_CS_PIN, LOW);//inicia comunicacion
  delayMicroseconds(50);
  SPI.beginTransaction(SPISettings(ADS_SPISPEED, MSBFIRST, SPI_MODE1)); // empieza la adq
  delayMicroseconds(10);


  while (digitalRead(ADS_RDY_PIN));

  byte data = (channel << 4) | ADS1256_MUXN_AINCOM; //AIN-channel and AINCOM
  writeRegister(MUX,data); //entra a modo mux y escoje el canal

  //Sincronizar
  SPI.transfer(SYNC);
  delayMicroseconds(10);

  //Despertar
  SPI.transfer(WAKEUP);
  delayMicroseconds(10);
  //while (digitalRead(ADS_RDY_PIN));

  //Entrar a modo lectura
  SPI.transfer(RDATA);
  delayMicroseconds(50*tCLKIN); //t6 50 cycles Tclkin page 6
  while (digitalRead(ADS_RDY_PIN));
  unsigned long _highByte = SPI.transfer(NOP);
  unsigned long _midByte = SPI.transfer(NOP);
  unsigned long _lowByte = SPI.transfer(NOP);

  // Combine all 3-bytes to 24-bit data using byte shifting.
  adc_val = (_highByte << 16) + (_midByte << 8) + (_lowByte);
  adc_val = (_highByte&0x80)?(0xff000000L)|adc_val:adc_val;
  delayMicroseconds(10);

  digitalWrite(ADS_CS_PIN, HIGH);
  delayMicroseconds(50);
  SPI.endTransaction();

  Serial.print("Medida obtenida ");
  Serial.print(adc_val);
  //Serial.print(" complemento a dos: ");
  //Serial.print((~adc_val)+1);
  Serial.println("");
  return adc_val;


}


long readADSDiff(byte positiveCh, byte negativeCh) {
  long adc_val = 0; // unsigned long is on 32 bits

  digitalWrite(ADS_CS_PIN, LOW);
  delayMicroseconds(50);
  SPI.beginTransaction(SPISettings(ADS_SPISPEED, MSBFIRST, SPI_MODE1));
  delayMicroseconds(10);

  while (digitalRead(ADS_RDY_PIN)) {} ;

  byte data = (positiveCh << 4) | 0x08; //xxxx1000 - AINp = positiveCh, AINn = negativeCh
  SPI.transfer(0x50 | 1); // write (0x50) MUX register (0x01)
  SPI.transfer(0x00);   // number of registers to be read/written − 1, write one register only
  SPI.transfer(data);   // write the databyte to the register
  delayMicroseconds(10);

  //SYNC command 1111 1100
  SPI.transfer(0xFC);
  delayMicroseconds(10);

  //WAKEUP 0000 0000
  SPI.transfer(0x00);
  delayMicroseconds(10);

  SPI.transfer(0x01); // Read Data 0000  0001 (01h)
  delayMicroseconds(10);

  adc_val = SPI.transfer(0);
  adc_val <<= 8; //shift to left
  adc_val |= SPI.transfer(0);
  adc_val <<= 8;
  adc_val |= SPI.transfer(0);

  delayMicroseconds(10);

  digitalWrite(ADS_CS_PIN, HIGH);
  delayMicroseconds(50);

  if (adc_val > 0x7fffff) { //if MSB == 1
    adc_val = adc_val - 16777216; //do 2's complement, keep the sign this time!
  }
  Serial.print("Got diff measurement from ADS ");
  Serial.println(adc_val);

  return adc_val;
}
