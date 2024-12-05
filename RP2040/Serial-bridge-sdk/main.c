// SPDX-License-Identifier: MIT

/*
  SerialPassthrough - Use tool to control the CC1352 module

  Eduardo Contreras @ Electronic Cats
  Original Creation Date: Dic 5, 2024

  This code is beerware; if you see me (or any other Electronic Cats
  member) at the local, and you've found our code helpful,
  please buy us a round!
  Distributed as-is; no warranty is given.
  
*/

#include "usb-uart-controller.h"
#include "catsniffer.h"
#include <hardware/structs/sio.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

extern uint8_t commandRecognized;
extern uint8_t commandCounter;
extern uint8_t commandData[5];

catsniffer_t catsniffer;

void core1_entry(void);

int main(void)
{
	int itf;
	gpio_init(Pin_Button);
	gpio_init(Pin_Boot);
	gpio_init(Pin_Reset);
	gpio_init(Pin_Reset_Viewer);

	gpio_init(LED1);
	gpio_init(LED2);
	gpio_init(LED3);

	gpio_set_dir(Pin_Button, GPIO_IN);
	gpio_pull_up(Pin_Button);
	gpio_set_dir(Pin_Boot, GPIO_IN);
	gpio_pull_up(Pin_Boot);
	gpio_set_dir(Pin_Reset, GPIO_OUT);
	gpio_set_dir(Pin_Reset_Viewer, GPIO_IN);
	
	gpio_set_dir(LED1,GPIO_OUT);
	gpio_set_dir(LED2,GPIO_OUT);
	gpio_set_dir(LED3,GPIO_OUT);
	// gpio_put(LED1, 1);
	// gpio_put(LED2, 1);
	// gpio_put(LED3, 1);
	gpio_set_dir(CTF1, GPIO_OUT);
	gpio_set_dir(CTF2, GPIO_OUT);
	gpio_set_dir(CTF3, GPIO_OUT);

  	//Make all cJTAG pins an input 
	for(int i=11;i<15;i++){
		gpio_init(i);
		gpio_set_dir(i, GPIO_IN);
	}


	usbd_serial_init();

	for (itf = 0; itf < CFG_TUD_CDC; itf++)
		init_uart_data(itf);

	if (!gpio_get(Pin_Boot)) { // Check if Pin_Boot is LOW
        catsniffer.led_interval = 200;
        catsniffer.baud = 921600;
        catsniffer.mode = BOOT;
    } else {
        catsniffer.led_interval = 1000;
        catsniffer.baud = 921600;
        catsniffer.mode = PASSTRHOUGH;
    }

    // Wait until Pin_Boot goes HIGH
    while (!gpio_get(Pin_Boot)) {
        tight_loop_contents(); // Idle while waiting for Pin_Boot to go HIGH
    }

	if(catsniffer.mode==BOOT){
    	bootModeCC();
	}else{
		resetCC();
	}

	if(catsniffer.mode==PASSTRHOUGH){
    	//Switch Radio for 2.4Ghz BLE by default can be changed on the fly
    	changeBand(&catsniffer, GIG);
  	}

	multicore_launch_core1(core1_entry);

	while (1) {
		update_uart_cfg(0);
		uart_write_bytes(0);
		// Process the command
		if(commandRecognized){	
			// Check for end of command
			if (strstr(commandData, ">ÿñ")) {
				// Process the command
				processCommand(&catsniffer, commandData); // Implement or call an existing function
				commandData[0] = '\0';       // Clear the command buffer
				commandRecognized = 0;         // Reset recognition
				commandCounter = 0;            // Reset command matching
			}
		}

		update_leds(&catsniffer);
	}

	return 0;
}


void core1_entry(void)
{
	tusb_init();

	while (1) {
		int itf;
		int con = 0;

		tud_task();

		if (tud_cdc_n_connected(0)) {
			con = 1;
			usb_cdc_process(0);
		}
	}
}