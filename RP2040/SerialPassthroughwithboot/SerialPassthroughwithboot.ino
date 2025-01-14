/*
  SerialPassthrough - Use tool to flash the CC1352 module

  Andres Sabas @ Electronic Cats
  Eduardo Contreras @ Electronic Cats
  Original Creation Date: Jan 16, 2024

  This code is beerware; if you see me (or any other Electronic Cats
  member) at the local, and you've found our code helpful,
  please buy us a round!
  Distributed as-is; no warranty is given.
  
*/

#include "SerialPassthroughwithboot.h"

catsniffer_t catsniffer;

uint8_t LEDs[3]={LED1,LED2,LED3};
int i=0;

const uint8_t commandID[5]={0xC3, 0xB1, 0xC3, 0xBF, 0x3C};
uint8_t commandCheck[5]={0};
bool asciiRecognized=0;
bool commandRecognized=0;
String commandData="";
uint8_t commandCounter=0;

void setup() {
  pinMode(Pin_Button, INPUT_PULLUP);
  pinMode(Pin_Boot, INPUT_PULLUP);
  pinMode(Pin_Reset, OUTPUT);
  pinMode(Pin_Reset_Viewer, INPUT);
  digitalWrite(Pin_Reset, HIGH);
  
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(CTF1, OUTPUT);
  pinMode(CTF2, OUTPUT);
  pinMode(CTF3, OUTPUT);

  //Make all cJTAG pins an input 
  for(int i=11;i<15;i++){
    pinMode(i,INPUT);
  }

  //Select mode and speed
  if(!digitalRead(Pin_Boot)){
    catsniffer.led_interval=200;
    catsniffer.baud=500000;
    catsniffer.mode=BOOT;
  }
  else{
    catsniffer.led_interval=1000;
    catsniffer.baud=921600;
    catsniffer.mode=PASSTRHOUGH;
  }
  while(!digitalRead(Pin_Boot));

  //Begin Serial ports
  Serial.begin(catsniffer.baud);
  Serial1.begin(catsniffer.baud);

  resetCC();

  if(catsniffer.mode==BOOT){
    bootModeCC();
  }
  
  if(catsniffer.mode==PASSTRHOUGH){
    //Switch Radio for 2.4Ghz BLE by default can be changed on the fly
    changeBand(&catsniffer, GIG);
  }
  
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, catsniffer.mode);
}


void loop() {
  //SerialPassthrough 
  if (Serial.available()) {      // If anything comes in Serial (USB),
    int data = Serial.read();
    if(data==commandID[commandCounter]){
      commandCounter++;
      if(commandCounter==5) //all characters have matched
        commandRecognized=1;
    }else if(!commandRecognized){
      commandCounter=0;
    }
    if(commandRecognized){
      commandData+=String((char)data);
      if(commandData.endsWith(">ÿñ")){
        processCommand(&commandData);
        commandData="";
        commandRecognized=0;
      }
    }
    else{                    //Command not recognized, send out serial
      Serial1.write(data);   // read it and send it out Serial1 (pins 0 & 1)
    }

  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }
    
  if(millis() - catsniffer.previousMillis > catsniffer.led_interval) {
    catsniffer.previousMillis = millis(); 
    if(catsniffer.mode){
      digitalWrite(LEDs[i], !digitalRead(LEDs[i]));
      i++;
      if(i>2)i=0;
    }else{
      digitalWrite(LED3, !digitalRead(LED3));
    }
  }

}

void resetCC(void){
  digitalWrite(Pin_Reset, LOW);
  delay(100);
  digitalWrite(Pin_Reset, HIGH);
  delay(100);
  }

void bootModeCC(void){
  pinMode(Pin_Boot, OUTPUT);
  //Enter bootloader mode function
  digitalWrite(Pin_Boot, LOW);
  delay(100);
  resetCC();
  }

void changeBaud(catsniffer_t *cs, unsigned long newBaud){
  if(newBaud==cs->baud)
    return;
  Serial.flush();
  Serial1.flush();
  Serial.end();
  Serial1.end();
  cs->baud = newBaud;
  Serial.begin(cs->baud);
  Serial1.begin(cs->baud);
  return;
  }


void changeBand(catsniffer_t *cs, unsigned long newBand){
  if(newBand==cs->band)
    return;
  switch(newBand){
    case GIG:   //2.4Ghz CC1352
      digitalWrite(CTF1,  LOW);
      digitalWrite(CTF2,  HIGH);
      digitalWrite(CTF3,  LOW);
    break;

    case SUBGIG_1: //Sub-ghz CC1352
      digitalWrite(CTF1,  LOW);
      digitalWrite(CTF2,  LOW);
      digitalWrite(CTF3,  HIGH);
    break;

    case SUBGIG_2: //LoRa
      digitalWrite(CTF1,  HIGH);
      digitalWrite(CTF2,  LOW);
      digitalWrite(CTF3,  LOW);
    break;

    default:
    break;
    }

  return;
  }


void changeMode(catsniffer_t *cs, unsigned long newMode){
  if(newMode==cs->mode)
    return;
  cs->mode = newMode;
  if(cs->mode==BOOT){
      cs->led_interval=200;
      bootModeCC();
      resetCC();
      delay(200);
      changeBaud(cs, 500000);
  }
  if(cs->mode==PASSTRHOUGH){
      // Update boot state
      digitalWrite(Pin_Boot, HIGH);
      resetCC();
      cs->led_interval=1000;
      changeBaud(cs, 921600);
  }

  return;
  }

void processCommand(String *cmd){
  // ñÿ<Payload>ÿñ Catsnifffer Commands
  cmd->remove(0, 1);
  cmd->remove(cmd->indexOf(">ÿñ"),5);
  //Serial.println(*cmd);
  //enter boot mode
  if("boot" == *cmd){
    changeMode(&catsniffer, BOOT);
    Serial.println("BOOT");
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, catsniffer.mode);
  }
  //exit boot mode
  if("exit" == *cmd){
    changeMode(&catsniffer, PASSTRHOUGH);
    Serial.println("PASSTRHOUGH");
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
  }
    //exit boot mode
  if("band1" == *cmd){
    changeBand(&catsniffer, GIG);
    Serial.println("2.4Ghz Band");
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
  }
  if("band2" == *cmd){
    changeBand(&catsniffer, SUBGIG_1);
    Serial.println("SUB-Ghz Band");
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
  }
  if("band3" == *cmd){
    changeBand(&catsniffer, SUBGIG_2);
    Serial.println("LoRa Band");
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
  }
  //change RF band to work with
  //Ping?
  //Return catsniffer version
    
  return;
 }
