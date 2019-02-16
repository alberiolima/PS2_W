#include <PS2X_lib.h>  //biblioteca do controle PS2

//Pinos do arduino para a placa receptora do controle PS2
#define PS2_DAT        11 //DI
#define PS2_CMD        10 //D0
#define PS2_SEL         9 //CS
#define PS2_CLK         8 //CLK

//Pinos de sentido da rotação dos motors
#define pinoMotor1A A0
#define pinoMotor1B A1
#define pinoMotor2A A2
#define pinoMotor2B A3

//Pinos de velocidade dos motores
#define pinoVelocidadeMotorA 5
#define pinoVelocidadeMotorB 6

#define incVelocidade   2
#define maxVelocidade 255
#define minVelocidade 120
#define tempoPartida  200
#define veloPartida   220

int iVelocidade = 0;

PS2X ps2x; // Cria a Instancia da classe 

void setup() {
  Serial.begin( 9600 );
  
  pinMode( pinoMotor1A, OUTPUT );
  pinMode( pinoMotor1B, OUTPUT );
  pinMode( pinoMotor2A, OUTPUT );
  pinMode( pinoMotor1B, OUTPUT );
  pinMode( pinoVelocidadeMotorA, OUTPUT );
  pinMode( pinoVelocidadeMotorB, OUTPUT );

  int error = ps2x.config_gamepad( PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
  
  if(error == 0){
    Serial.println("Controle encontrado, configurado com successo");
  } else {
    Serial.println("Erro no controle PS2");
  }
}

void praEsquerda(){
  digitalWrite( pinoMotor1A, HIGH );
  digitalWrite( pinoMotor1B, HIGH );
}

void praDireita(){
  digitalWrite( pinoMotor2A, HIGH );
  digitalWrite( pinoMotor2B, HIGH );
}

void praFrente(){
  digitalWrite( pinoMotor1A, LOW );
  digitalWrite( pinoMotor1B, HIGH );
  digitalWrite( pinoMotor2A, LOW );
  digitalWrite( pinoMotor1B, HIGH );
}

void praTras(){
  digitalWrite( pinoMotor1A, HIGH );
  digitalWrite( pinoMotor1B, LOW );
  digitalWrite( pinoMotor2A, HIGH );
  digitalWrite( pinoMotor1B, LOW );
}

void frear(){
  iVelocidade = 0;
  digitalWrite( pinoMotor1A, HIGH );
  digitalWrite( pinoMotor1B, HIGH);
  digitalWrite( pinoMotor2A, HIGH );
  digitalWrite( pinoMotor1B, HIGH );
  digitalWrite( pinoVelocidadeMotorA, HIGH );
  digitalWrite( pinoVelocidadeMotorB, HIGH );
}

void liberarFreio(){
  digitalWrite( pinoMotor1A, LOW );
  digitalWrite( pinoMotor1B, LOW);
  digitalWrite( pinoMotor2A, LOW );
  digitalWrite( pinoMotor1B, LOW );
  digitalWrite( pinoVelocidadeMotorA, LOW );
  digitalWrite( pinoVelocidadeMotorB, LOW );  
}

void mudaVelocidade( byte velocidade ) {
  analogWrite( pinoVelocidadeMotorA, velocidade );
  analogWrite( pinoVelocidadeMotorB, velocidade );  
}

void loop() {
  //Ler dados da placa-mãe de controle do PS2
  ps2x.read_gamepad(); //read controller and set large motor to spin at 'vibrate' speed

  //freio
  if(ps2x.NewButtonState(PSB_CROSS)) {
    if (ps2x.ButtonPressed(PSB_CROSS) ) {
      frear(); 
    } else if (ps2x.ButtonReleased(PSB_CROSS)) {
      liberarFreio();
    }
  } else if(ps2x.NewButtonState(PSB_CIRCLE)) { //Banguela
    if (ps2x.ButtonPressed(PSB_CIRCLE) ) {
      liberarFreio();
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
      praTras();
      mudaVelocidade( (byte)(iVelocidade * -1) );
    } else {          
      praFrente();      
      if ( iVelocidade == minVelocidade + incVelocidade ) {
        mudaVelocidade( veloPartida );
        delay(tempoPartida);
      }          
      mudaVelocidade( (byte)iVelocidade );
    }

    //Direção
    if ( ps2x.Analog(PSS_RX) > 240 ) { 
      praDireita();
    } else if ( ps2x.Analog(PSS_RX) < 15 ) { 
      praEsquerda();
    } 
  }
}
