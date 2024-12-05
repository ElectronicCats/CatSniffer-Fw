#ifndef __CATSNIFFER_H__
#define __CATSNIFFER_H__

#include <pico/time.h>
#include <hardware/gpio.h>
#include <string.h>

//Pin declaration to enter bootloader mode on CC1352
#define Pin_Reset (3)
#define Pin_Reset_Viewer (15)
#define Pin_Boot (2)
#define Pin_Button (2)
#define LED1 (27)
#define LED2 (26)
#define LED3 (28)
//Pin Declaration for RF switch
#define CTF1 8
#define CTF2 9
#define CTF3 10

enum MODE{
   PASSTRHOUGH = 0,   //Mode flag = 0; for passthrough @ 921600 bauds 
   BOOT,              //Mode flag = 1; for bootloader options @ 500000 bauds
   LORA               //Mode flag = 2; for LoRaWAN @ 921600 bauds
 };

enum BAND{
   GIG = 0, 
   SUBGIG_1, 
   SUBGIG_2
 };

typedef struct {
  uint8_t mode;
  uint8_t mode_tested;
  uint8_t band;
  unsigned long led_interval;
  unsigned long previousMillis;  // will store last time blink happened
  unsigned long baud;
  uint8_t led_1;
  uint8_t led_2;
  uint8_t led_3;
  uint8_t ctf_1;
  uint8_t ctf_2;
  uint8_t ctf_3;
} catsniffer_t;

void processCommand(catsniffer_t *cs, char *cmd);
void update_leds(catsniffer_t *catsniffer);
void resetCC(void);
void bootModeCC(void);
void changeBaud(catsniffer_t *cs, unsigned long newBaud);
void changeBand(catsniffer_t *cs, unsigned long newBand);
void changeMode(catsniffer_t *cs, unsigned long newMode);


#endif

