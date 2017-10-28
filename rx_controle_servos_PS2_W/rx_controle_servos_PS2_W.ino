/* 
 * Código exemplo de como controlar dois servos
 * Usando o controle remoto sem fio 
 * Esse exemplo usa apelas o stick (analógico) da direita
 * Repositório em:https://github.com/alberiolima/PS2_W
 */

#include <PS2X_lib.h> 
#include <SPI.h>
#include "LT8900.h"
#include <Servo.h>

const uint8_t PIN_NRF_RST = 10;
const uint8_t PIN_NRF_CS  =  9;
const uint8_t PIN_NRF_PKT =  8;

#define pinoSERVO1 3
#define pinoSERVO2 5

int iPosiSERVO1 = 90;
int iPosiSERVO2 = 90;

#define servoInc1 5
#define servoInc2 5

Servo servo1; 
Servo servo2; 

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; 

void setup(){


  servo1.attach(3);
  servo2.attach(5);
  
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

}

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
      
      //Stick direito acionado para cima
      if ( ps2x.Analog(PSS_RY)  < 10 ) { 
        if ( iPosiSERVO1 < 178 ) { 
          iPosiSERVO1 += servoInc1; 
        }        
      }
      
      //Stick direito acionado para baixo
      if ( ps2x.Analog(PSS_RY) > 240  ) {
        if ( iPosiSERVO1 > 0 ) { 
          iPosiSERVO1 -= servoInc1; 
       }        
      }
      
      //Stick direito acionado para esquerda
      if ( ps2x.Analog(PSS_RX)  < 10 ) {
        if ( iPosiSERVO2 < 178 ) { 
         iPosiSERVO2 += servoInc2; 
        }       
      } 
      
      //Stick direito acionado para direita
      if ( ps2x.Analog(PSS_RX) > 240  ) {
        if ( iPosiSERVO2 > 0 ) { 
          iPosiSERVO2 -= servoInc2;
        }               
      }
      
      servo1.write( iPosiSERVO1 );
      servo2.write( iPosiSERVO2 );
      
      Serial.print( iPosiSERVO1 );
      Serial.print( " " );
      Serial.print( iPosiSERVO2 );
      Serial.println();
    }  
}
