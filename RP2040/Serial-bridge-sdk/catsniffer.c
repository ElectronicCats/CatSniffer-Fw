#include "catsniffer.h"

uint8_t LEDs[3]={LED1,LED2,LED3};
int led_counter=0;

void resetCC(void) {
    gpio_put(Pin_Reset, 0);
    sleep_ms(100);
    gpio_put(Pin_Reset, 1);
    sleep_ms(100);
}

void bootModeCC(void) {
    gpio_set_dir(Pin_Boot, GPIO_OUT); // Set Pin_Boot as output
    gpio_put(Pin_Boot, 0);
    sleep_ms(100);
    resetCC();
}

void changeBaud(catsniffer_t *cs, unsigned long newBaud) {
    // if (newBaud == cs->baud) {
    //     return;
    // }

    // uart_deinit(uart0);       // Deinitialize UART
    // cs->baud = newBaud;

    // uart_init(uart0, cs->baud);  // Reinitialize UART with new baud rate
}


void changeBand(catsniffer_t *cs, unsigned long newBand) {
    if (newBand == cs->band) {
        return;
    }

    switch (newBand) {
        case GIG: // 2.4 GHz CC1352
            gpio_put(CTF1, 0);
            gpio_put(CTF2, 1);
            gpio_put(CTF3, 0);
            break;

        case SUBGIG_1: // Sub-GHz CC1352
            gpio_put(CTF1, 0);
            gpio_put(CTF2, 0);
            gpio_put(CTF3, 1);
            break;

        case SUBGIG_2: // LoRa
            gpio_put(CTF1, 1);
            gpio_put(CTF2, 0);
            gpio_put(CTF3, 0);
            break;

        default:
            break;
    }

    cs->band = newBand;
}



void changeMode(catsniffer_t *cs, unsigned long newMode) {
    if (newMode == cs->mode) {
        return;
    }

    cs->mode = newMode;

    if (cs->mode == BOOT) {
        cs->led_interval = 200;
        bootModeCC();
        resetCC();
        sleep_ms(200);
    }

    if (cs->mode == PASSTRHOUGH) {
        gpio_put(Pin_Boot, 1); // Update boot state
        resetCC();
        cs->led_interval = 1000;
    }
}


void processCommand(catsniffer_t *cs, char *cmd) {
    if (strncmp(cmd, "boot", 4) == 0) {
        changeMode(cs, BOOT);
        gpio_put(LED1, 0);
        gpio_put(LED2, 0);
        gpio_put(LED3, cs->mode);
    } else if (strncmp(cmd, "exit", 4) == 0) {
        changeMode(cs, PASSTRHOUGH);
        gpio_put(LED1, 0);
        gpio_put(LED2, 0);
        gpio_put(LED3, 0);
    } else if (strncmp(cmd, "band1", 5) == 0) {
        changeBand(cs, GIG);
        gpio_put(LED1, 0);
        gpio_put(LED2, 0);
        gpio_put(LED3, 0);
    } else if (strncmp(cmd, "band2", 5) == 0) {
        changeBand(cs, SUBGIG_1);
        gpio_put(LED1, 0);
        gpio_put(LED2, 0);
        gpio_put(LED3, 0);
    } else if (strncmp(cmd, "band3", 5) == 0) {
        changeBand(cs, SUBGIG_2);
        gpio_put(LED1, 0);
        gpio_put(LED2, 0);
        gpio_put(LED3, 0);
    }
}


void update_leds(catsniffer_t *cs) {
    unsigned long currentMillis = to_ms_since_boot(get_absolute_time());

    if (currentMillis - cs->previousMillis > cs->led_interval) {
        cs->previousMillis = currentMillis;

        if (cs->mode==BOOT) {
            gpio_put(LEDs[led_counter], !gpio_get(LEDs[led_counter])); // Toggle LED
            led_counter++;
            if (led_counter > 2) led_counter = 0; // Wrap around to the first LED
        } else {
            gpio_put(LED3, !gpio_get(LED3)); // Toggle LED3
        }
    }
}