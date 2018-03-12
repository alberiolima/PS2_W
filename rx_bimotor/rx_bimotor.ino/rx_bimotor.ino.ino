#include <PS2X_lib.h> 
#include <SPI.h>
#include "LT8900.h"

#define accIncremento 10
#define limiteDirecao (acceleration / 1.5)
#define pinMotorL 3
#define pinMotorR 5

const uint8_t PIN_NRF_RST = 10;
const uint8_t PIN_NRF_CS  =  9;
const uint8_t PIN_NRF_PKT =  8;

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; 

void setup(){
 
  Serial.begin(9600);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV4);

  delay(500);

  lt.begin();
  lt.setCurrentControl(4,15);
  lt.setDataRate(LT8900::LT8910_62KBPS);
  lt.setChannel(0x06);

  lt.startListening();  
  
  pinMode( pinMotorL, OUTPUT );
  pinMode( pinMotorR, OUTPUT );
  digitalWrite( pinMotorL, LOW );
  digitalWrite( pinMotorR, LOW );

}

byte acceleration = 0;
byte vMotorL = 0;
byte vMotorR = 0;

void loop() {
  
    if (lt.available())  {

      //Ler dados do módulo RF
      uint8_t buf[32];
      int packetSize = lt.read(buf, 32);
      if (packetSize > 0)  {
        for(int i = 0; i < packetSize; i++) {
          ps2x.PS2data[i] = buf[i];         
        }
        ps2x.last_buttons = ps2x.buttons; //store the previous buttons states
        ps2x.buttons = *(uint16_t*)(ps2x.PS2data+3);   //store as one value for multiple functions 
      }     

      lt.startListening();      
      if ( ps2x.Analog(PSS_LY) < 20 ) {
        if ( acceleration + accIncremento > 255 ) {
          acceleration = 255;
        } else {
          acceleration += accIncremento;
        }        
      } else if ( ps2x.Analog(PSS_LY) > 235 ) {
        if ( acceleration - accIncremento < 0 ) {
          acceleration = 0;
        } else {
          acceleration -= accIncremento;
        }
      }
      if ( ps2x.Analog(PSS_RX) < 20 ) {        
        if ( vMotorL + accIncremento > limiteDirecao ) {
          vMotorL = limiteDirecao;
        } else {
          vMotorL += accIncremento;  
        }
        vMotorR = 0;
      } else if ( ps2x.Analog(PSS_RX) > 235 ) {        
        if ( vMotorR + accIncremento > limiteDirecao ) {
          vMotorR = limiteDirecao;
        } else {
          vMotorR += accIncremento;  
        }
        vMotorL = 0;
      } else {
        if ( vMotorR - accIncremento < 0 ) {
          vMotorR = 0;  
        } else{
          vMotorR -= accIncremento;
        }
        if ( vMotorL - accIncremento < 0 ) {
          vMotorL = 0;  
        } else{
          vMotorL -= accIncremento;
        }        
      }
      analogWrite( pinMotorL, acceleration - vMotorL );
      analogWrite( pinMotorR, acceleration - vMotorR );
      
      Serial.print( '\n' );
      Serial.print( acceleration - vMotorL );
      Serial.print( '\t' );
      Serial.print( acceleration - vMotorR );
    }
}
