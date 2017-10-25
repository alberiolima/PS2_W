#include <PS2X_lib.h>  //for v1.6
#include <SPI.h>
#include "LT8900.h"

const uint8_t PIN_NRF_RST = 10;
const uint8_t PIN_NRF_CS = 9;
const uint8_t PIN_NRF_PKT = 8;
const uint8_t PIN_ROLE = A3;

bool writer;

LT8900 lt(PIN_NRF_CS, PIN_NRF_PKT, PIN_NRF_RST);

PS2X ps2x; // Cria a Instancia da classe 


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

  char sbuf[32];

  //verify chip registers.
  for (int i = 0; i <= 50; i++)
  {
    uint16_t value = lt.readRegister(i);

    sprintf_P(sbuf, PSTR("%d = %04x\r\n"), i, value);
    Serial.print(sbuf);
  }

  lt.startListening();  

}

void loop() {
  
    //ler rf

    if (lt.available())  {
      
      uint8_t buf[32];

      int packetSize = lt.read(buf, 32);
      if (packetSize > 0)  {
        //dump the packet.
        for(int i = 0; i < packetSize; i++) {
          ps2x.PS2data[i] = buf[i];
        }
        ps2x.last_buttons = ps2x.buttons; //store the previous buttons states
        ps2x.buttons = *(uint16_t*)(ps2x.PS2data+3);   //store as one value for multiple functions 
      }     

      lt.startListening();
        
    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");      

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }   

    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if(ps2x.Button(PSB_TRIANGLE))
        Serial.println("Triangle pressed");        
    }

    if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      Serial.println("Circle just pressed");
    if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
      Serial.println("X just changed");
    if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
      Serial.println("Square just released");     

    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
      Serial.print("Stick Values:");
      Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_LX), DEC); 
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_RY), DEC); 
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_RX), DEC); 
    }     
  }
  delay(50);  
}
