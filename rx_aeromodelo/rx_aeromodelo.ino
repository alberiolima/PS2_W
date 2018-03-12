/*
 * 
 * 
 * Repositório em:https://github.com/alberiolima/PS2_W
 * 
 */

#include <PS2X_lib.h> 
#include <SPI.h>
#include "LT8900.h"
#include <Servo.h>

#define PIN_NRF_RST 10
#define PIN_NRF_CS   9
#define PIN_NRF_PKT  8

#define pinoSERVO1   3
#define pinoSERVO2   5
#define pinoSERVO3   6
#define pinoSERVO4   9
#define pinoSERVO5  10
#define pinoSERVO6  11

#define accIncremento     1
#define limiteAceleracao  179

byte iPosiSERVO3 = 0; //motor

Servo servo1; 
Servo servo2; 
Servo servo3; 
Servo servo4; 
Servo servo5; 
Servo servo6; 

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; 

void setup(){


  servo1.attach(pinoSERVO1);
  servo2.attach(pinoSERVO2);
  servo3.attach(pinoSERVO3);
  servo4.attach(pinoSERVO4);
  //servo5.attach(pinoSERVO5);
  //servo6.attach(pinoSERVO6);  
  
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
  servo3.write(0);
  delay(10);
  servo3.write(179);
  delay(10);
  servo3.write(179);
  servo3.write(0);

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
      //Aceleração feita por PSS_LY
      if ( ps2x.Analog(PSS_LY) < 20 ) { //Acelera
        if ( iPosiSERVO3 + accIncremento > limiteAceleracao ) {
          iPosiSERVO3 = limiteAceleracao;
        } else {
          iPosiSERVO3 += accIncremento;
        }        
      } else if ( ps2x.Analog(PSS_LY) > 235 ) { //Desacelera
        if ( iPosiSERVO3 - accIncremento < 0 ) {
          iPosiSERVO3 = 0;
        } else {
          iPosiSERVO3 -= accIncremento;
        }
      }
      
      servo1.write( map( ps2x.Analog(PSS_RX), 0, 255, 0, 179 ) ); //aileron
      servo2.write( map( ps2x.Analog(PSS_RY), 0, 255, 0, 179 ) ); //profundor
      servo3.write( iPosiSERVO3 ); //motor
      //servo4.write( map( ps2x.Analog(PSS_LX), 0, 255, 0, 179 ) ); //leme
      Serial.println( iPosiSERVO3 );
      
    }  
}
