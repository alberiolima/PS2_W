/* 
 *  Módumo emissor para controle remoto sem fio com base no PS2 e Arduino
 *  Francisco Albério Lima - Brasil
 *  
 *  Repositório: https://github.com/alberiolima/PS2_W
 *  
 
 
 
 Connections: 
 Arduino -> LT8900

 GND        GND
 3v3        VCC
 8          PKT
 9          CS
 10         RST
 11         MOSI
 12         MISO
 13         SCK

*/

#include <PS2X_lib.h>  //for v1.6
#include <SPI.h>
#include "LT8900.h"

//Pinos do arduino para a placa mãe do controle PS2
#define PS2_DAT      6 //Marrom  
#define PS2_CMD      4 //Laranja
#define PS2_SEL      3 //Amarelho
#define PS2_CLK      5 //Azul

//Pinos do arduino para o LT8900
#define PIN_NRF_RST 10
#define PIN_NRF_CS   9
#define PIN_NRF_PKT  8

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; // Cria a Instancia da classe 

void setup() {
  
  //Serial.begin(9600); //Descomente para depurar

  //Inicializa protocolo SPI que será utlizado pelo módulo LT8900
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV4);

  delay(500);

  //Inicializa módulo LT8900
  lt.begin();
  lt.setCurrentControl(4,15);
  lt.setDataRate(LT8900::LT8910_62KBPS);
  lt.setChannel(0x06);

  int error = ps2x.config_gamepad( PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
  
  if(error == 0){
    Serial.println("Controle encontrado, configurado com successo");
  } else {
    Serial.println("Erro no controle PS2");
  }
  
}

void loop(){
  //Ler dados da placa-mãe de controle do PS2
  ps2x.read_gamepad(); //read controller and set large motor to spin at 'vibrate' speed
    
  //envio de dados

  /* descomente para depurar
  for (byte i = 0; i< 22;i++){
    //Descomente para depurar
    Serial.print(ps2x.PS2data[i]);
    Serial.print( " " );
  }
  Serial.println();
  */

  lt.sendPacket(ps2x.PS2data, 9); //Envia apenas 9 bytes pois "pressure", que é a sensibilidade das teclas não está sendo ativada, 
                                  //acreidto que na proxima versão utlize as posições de 1 a 3 para identificacão do controle
  
  delay(50);
}
