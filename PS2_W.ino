/* Connections:
 
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

#define PS2_DAT        6 //Marrom   (usar pull-up)
#define PS2_CMD        4 //Laranja
#define PS2_SEL        3 //Amarelho (usar pull-up)
#define PS2_CLK        5 //Azul
//VCC 3.3V   (5V?)        //Vermelho
//GND/Terra               //Preto

/*
  Selecione os modos do controle:
    - pressures = lendo botões do controle analógico
    - rumble    = motor de vibração
*/

#define pressures   false //true
#define rumble      false //true

/*
  Atualmente não é suportado que o controle seja plugado depois do arduino ter inicializado, então
  Sempre que plugar o controle será necessário reiniciar o arduino ou charmar a função config_gamepad(pins)
*/

int error = 0;
byte type = 0;
byte vibrate = 0;

const uint8_t PIN_NRF_RST = 10;
const uint8_t PIN_NRF_CS  =  9;
const uint8_t PIN_NRF_PKT =  8;

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; // Cria a Instancia da classe 


void setup()
{
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

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.println("Controle encontrado, configurado com successo");
  } else {
    Serial.println("Erro no controle PS2");
  }
  

}

void loop()
{
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    //envio de dados
    uint8_t data[21];
    
    for (byte i = 0; i< 22;i++){
      data[i] = ps2x.Analog(i);
      //Descomente para depurar
      //Serial.print(data[i]);
      //Serial.print( " " );
    }
    Serial.println();

    lt.sendPacket(data, 21);
        
    delay(50);
}
