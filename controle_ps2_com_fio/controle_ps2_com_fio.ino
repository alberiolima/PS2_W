#include <PS2X_lib.h>  
#include <Servo.h>

//Pinos do arduino para a placa mãe do controle PS2
#define PS2_DAT      6 //Marrom  
#define PS2_CMD      4 //Laranja
#define PS2_SEL      3 //Amarelho
#define PS2_CLK      5 //Azul

#define pinoSERVO1 3
#define pinoSERVO2 5

int iPosiSERVO1 = 90;
int iPosiSERVO2 = 90;

#define servoInc1 5
#define servoInc2 5

Servo servo1; 
Servo servo2; 

PS2X ps2x; // Cria a Instancia da classe 

void setup() {

  Serial.begin(9600);

  servo1.attach(pinoSERVO1);
  servo2.attach(pinoSERVO2);
  
  int error = ps2x.config_gamepad( PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
  
  if(error == 0){
    Serial.println("Controle encontrado, configurado com successo");
  } else {
    Serial.println("Erro no controle PS2");
  }

}

void loop() {
  ps2x.read_gamepad(); //read controller and set large motor to spin at 'vibrate' speed

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
