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

#define pin1MOTOR   A0
#define pin2MOTOR   A1
#define pinVelMOTOR  5

//Em graus
#define centroSERVO1 86  //Não é 90 pois o controle retorna123 como centro e não 128
#define limiteGiro   15  //Limite parao giro das rodas, esse valor corresponde a metade do giro total
#define correcaoGiro 15  //Corecao quando o servo está mal posicionado

#define incVelocidade   2
#define maxVelocidade 255
#define minVelocidade 120
#define tempoPartida  200
#define veloPartida   220

int iPosiSERVO1 = 0; //motor
int iVelocidade = 0;

Servo servo1; 
Servo servo2; 

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; 

void setup(){

  servo1.attach(pinoSERVO1);

  pinMode( pin1MOTOR, OUTPUT);
  pinMode( pin2MOTOR, OUTPUT);
  pinMode( pinVelMOTOR, OUTPUT);
  
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

      //freio
      if(ps2x.NewButtonState(PSB_CROSS)) {
        if (ps2x.ButtonPressed(PSB_CROSS) ) {
          iVelocidade = 0;
          digitalWrite( pinVelMOTOR, HIGH );
          digitalWrite( pin1MOTOR, HIGH );
          digitalWrite( pin2MOTOR, HIGH );  
        } else if (ps2x.ButtonReleased(PSB_CROSS)) {
          analogWrite( pinVelMOTOR, 0 );
          digitalWrite( pin1MOTOR, LOW );
          digitalWrite( pin2MOTOR, LOW );
        }
      } else if(ps2x.NewButtonState(PSB_CIRCLE)) { //Banguela
        if (ps2x.ButtonPressed(PSB_CIRCLE) ) {
          digitalWrite( pin1MOTOR, LOW );
          digitalWrite( pin2MOTOR, LOW );          
        }
      } else { //Velocidade
        if ( ps2x.Analog(PSS_LY) > 240 ) { 
          if ( iVelocidade == 0 ) {
            iVelocidade = minVelocidade * -1;
          }
          iVelocidade -= incVelocidade;
          if ( iVelocidade < maxVelocidade * -1 ){
            iVelocidade = maxVelocidade * -1;
          }
        } else if ( ps2x.Analog(PSS_LY) < 15 ) {
          if ( iVelocidade == 0 ) {
            iVelocidade = minVelocidade;
          }          
          iVelocidade += incVelocidade;
          if ( iVelocidade > maxVelocidade ){
            iVelocidade = maxVelocidade;
          }
        }
        if ( iVelocidade < 0 ) {
          digitalWrite( pin1MOTOR, LOW );
          digitalWrite( pin2MOTOR, HIGH ); 
          analogWrite( pinVelMOTOR, iVelocidade * -1 );
        } else {          
          digitalWrite( pin1MOTOR, HIGH ); 
          digitalWrite( pin2MOTOR, LOW );           
          analogWrite( pinVelMOTOR, iVelocidade );
          if ( iVelocidade == minVelocidade + incVelocidade ) {
            analogWrite( pinVelMOTOR, veloPartida );
            delay(tempoPartida);
          }          
        }
      }

      //direcao       
      int iDirecao = map( ps2x.Analog(PSS_RX), 0, 255, centroSERVO1 - limiteGiro, centroSERVO1 + limiteGiro ); //Converte leitura analógica para graus
      iDirecao += correcaoGiro; //Correção do giro, caso o servo esteja num local que não permita correção mecânica/manual
      servo1.write( iDirecao ); //direcao
            
    }  
}
