#ifndef __USB_UART_CONTROLLER__
#define __USB_UART_CONTROLLER__

#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <string.h>
#include <tusb.h>

#if !defined(MIN)
#define MIN(a, b) ((a > b) ? b : a)
#endif /* MIN */

#define BUFFER_SIZE 2560

#define DEF_BIT_RATE 921600
#define DEF_STOP_BITS 1
#define DEF_PARITY 0
#define DEF_DATA_BITS 8


typedef struct {
	uart_inst_t *const inst;
	uint irq;
	void *irq_fn;
	uint8_t tx_pin;
	uint8_t rx_pin;
} uart_id_t;

typedef struct {
	cdc_line_coding_t usb_lc;
	cdc_line_coding_t uart_lc;
	mutex_t lc_mtx;
	uint8_t uart_buffer[BUFFER_SIZE];
	uint32_t uart_pos;
	mutex_t uart_mtx;
	uint8_t usb_buffer[BUFFER_SIZE];
	uint32_t usb_pos;
	mutex_t usb_mtx;
} uart_data_t;



// Utilities for USB to UART
static inline uint databits_usb2uart(uint8_t data_bits);
static inline uart_parity_t parity_usb2uart(uint8_t usb_parity);
static inline uint stopbits_usb2uart(uint8_t stop_bits);

//UART Functions
void init_uart_data(uint8_t itf);
void uart0_irq_fn(void);
void update_uart_cfg(uint8_t itf);
void uart_write_bytes(uint8_t itf);
void init_uart_data(uint8_t itf);

//USB functions
void usb_read_bytes(uint8_t itf);
void usb_write_bytes(uint8_t itf);
void usb_cdc_process(uint8_t itf);


#endif