#include <RCSwitch.h>

RCSwitch mySwitchRX = RCSwitch();
RCSwitch mySwitchTX = RCSwitch();

const int led = 5;
const int botao = 4;

void setup() 
{
  pinMode(led, OUTPUT);
  pinMode(botao, INPUT);
  Serial.begin(9600);
  mySwitchRX.enableReceive(0);
  mySwitchTX.enableTransmit(3);
  mySwitchTX.setRepeatTransmit(4);

  mySwitchTX.send(2211, 24);
}

void loop()
{

  if (digitalRead(botao) == 1)
  {
    digitalWrite(led, !digitalRead(led));
    mySwitchTX.send(3333, 24);
    Serial.println("envia 3333");
    delay(1000);
  }
  
  if (mySwitchRX.available()) {
    
    int value = mySwitchRX.getReceivedValue();

      
    if (value == 1111)
    {
      digitalWrite(led, HIGH);
      Serial.println("Liga led e envia 1122");
      delay(500);
      mySwitchTX.send(1122, 24);
      
    }
    if (value == 2222) 
    {
      digitalWrite(led, LOW);
      Serial.println("desliga led e envia 2211");
      delay(500);
      mySwitchTX.send(2211, 24);
    }
    if (value == 12611) //remote control on
    {
      digitalWrite(led, HIGH);
      Serial.println("Liga led e envia 1122");
      delay(500);
      mySwitchTX.send(1122, 24);
    }
    if (value == 12608) //remote control off
    {
      digitalWrite(led, LOW);
      Serial.println("desliga led e envia 2211");
      delay(500);
      mySwitchTX.send(2211, 24);
    }
    
    mySwitchRX.resetAvailable();
  }
  
}