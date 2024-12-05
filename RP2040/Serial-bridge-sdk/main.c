// SPDX-License-Identifier: MIT
#include "usb-uart-controller.h"
#include <hardware/structs/sio.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

//Pin declaration to enter bootloader mode on CC1352
#define Pin_Reset (3)
#define Pin_Reset_Viewer (15)
#define Pin_Boot (2)
#define Pin_Button (2)
#define LED1 (27)
#define LED2 (26)
#define LED3 (28)


#define LED_PIN 27

void core1_entry(void);

int main(void)
{
	int itf;
	gpio_init(LED_PIN);
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
	
	gpio_put(Pin_Reset, 1);
	
	gpio_set_dir(LED1,GPIO_OUT);
	gpio_set_dir(LED2,GPIO_OUT);
	gpio_set_dir(LED3,GPIO_OUT);
	gpio_put(LED1, 1);
	gpio_put(LED2, 1);
	gpio_put(LED3, 1);
	// gpio_set_dir(CTF1, OUTPUT);
	// gpio_set_dir(CTF2, OUTPUT);
	// gpio_set_dir(CTF3, OUTPUT);
  	//Make all cJTAG pins an input 
	for(int i=11;i<15;i++){
		gpio_init(i);
		gpio_set_dir(i, GPIO_IN);
	}

	usbd_serial_init();

	for (itf = 0; itf < CFG_TUD_CDC; itf++)
		init_uart_data(itf);

	// gpio_init(LED_PIN);
	// gpio_set_dir(LED_PIN, GPIO_OUT);

	multicore_launch_core1(core1_entry);

	while (1) {
		update_uart_cfg(0);
		uart_write_bytes(0);
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

		for (itf = 0; itf < CFG_TUD_CDC; itf++) {
			if (tud_cdc_n_connected(itf)) {
				con = 1;
				usb_cdc_process(itf);
			}
		}

		gpio_put(LED_PIN, con);
	}
}