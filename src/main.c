#include "main.h"
#include <cr_section_macros.h>
#include <stdio.h>
#include "LPC8xx.h"
#include <stdint.h>
#include "fro.h"
#include "rom_api.h"
#include "syscon.h"
#include "swm.h"
#include "i2c.h"
#include "spi.h"
#include "ctimer.h"
#include "core_cm0plus.h"
#include "uart.h"
#include "utilities.h"
#include "lib_ENS_II1_lcd_v2.h"

 /*
 * ########## Branchements ########## *
 * 									  *
 *  Mapping sur l'emplacement INOUT : *
 *									  *
 * 		||-------------||			  *
 * 		||             ||			  *
 * 		|| VCC  | SCK  ||			  *
 * 		|| BUSY | DC   ||			  *
 * 		|| -    | CS    < encoche	  *
 * 		|| RST  | -    ||			  *
 * 		|| GND  | GND  ||			  *
 * 		||             ||			  *
 * 		||-------------||			  *
 *									  *
 *  Mapping sur l'emplacement UART :  *
 *									  *
 * 			||-----||				  *
 * 			|| DIN ||			      *
 * 			||  -  ||				  *
 * 			||  -  ||				  *
 * 			||  -  ||				  *
 * 			||-----||				  *
 *									  *
 * ################################## *
 */

volatile uint32_t millis = 0;

void Config_Spi(void){
	// Enable clocks to relevant peripherals
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON | GPIO0 | SWM | CTIMER0 | GPIO_INT | SPI0);


	// Configure the SWM (see peripherals_lib and swm.h)
	ConfigSWM(SPI0_SCK, SCK_PIN);
	ConfigSWM(SPI0_MOSI, MOSI_PIN);
	ConfigSWM(SPI0_SSEL0, SSEL_PIN);

	// Give SPI0 a reset
	LPC_SYSCON->PRESETCTRL[0] &= (SPI0_RST_N);
	LPC_SYSCON->PRESETCTRL[0] |= ~(SPI0_RST_N);

	// Enable main_clk as function clock to SPI
	LPC_SYSCON->SPI0CLKSEL = FCLKSEL_MAIN_CLK;

	// Get main_clk frequency
	SystemCoreClockUpdate();

	// Configure the SPI master's clock divider (value written to DIV divides by value+1)
	LPC_SPI0->DIV = (main_clk/SPIBAUD) - 1;

	// Configure the CFG register:
	// Enable=true, master, no LSB first, CPHA=0, CPOL=0, no loop-back, SSEL active low
	LPC_SPI0->CFG = SPI_CFG_ENABLE | SPI_CFG_MASTER;

	// Configure the SPI delay register (DLY)
	// Pre-delay = 0 clocks, post-delay = 0 clocks, frame-delay = 0 clocks, transfer-delay = 0 clocks
	LPC_SPI0->DLY = 0x0000;

	// Configure the SPI control register
	// Master: End-of-frame true, End-of-transfer true, RXIGNORE true, LEN 8 bits.
	LPC_SPI0->TXCTL = SPI_CTL_EOF | SPI_CTL_EOT | SPI_CTL_RXIGNORE | SPI_CTL_LEN(8);
}

void Config_Digital_Write(uint32_t port){
	Config_LED(port);
}

void Config_Digital_Read(uint32_t port){
	if (port <= 31) {
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= GPIO0;   // Turn on clock to GPIO0
	LPC_GPIO_PORT->DIRSET[0]= 0<<port;
	}
	else if (port <= 63) {
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= GPIO1;   // Turn on clock to GPIO1
	LPC_GPIO_PORT->DIRSET[1]= 0<<(port-32);
	}
}

void Digital_Write(uint32_t pin, uint32_t value){
	if(value){
		LPC_GPIO_PORT->B0[pin] = 1;
	} else{
		LPC_GPIO_PORT->B0[pin] = 0;
	}
}

uint32_t Digital_Read(uint32_t pin){
	return LPC_GPIO_PORT->B0[pin];
}

void init_ctimer0_ms(void)
{
    // Activer l'horloge pour CTIMER0
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 7); // CTIMER0

    // Reset du timer
    LPC_SYSCON->PRESETCTRL0 &= ~(1 << 7);
    LPC_SYSCON->PRESETCTRL0 |= (1 << 7);

    // Mode timer normal
    LPC_CTIMER0->TCR = 0x02;   // reset
    LPC_CTIMER0->PR  = 15;     // prescaler = 15, tick = 1 µs à 15 MHz
    LPC_CTIMER0->MR[0] = 1000; // match à 1000 µs = 1 ms
    LPC_CTIMER0->MCR = (1 << 0) | (1 << 1); // interrupt + reset sur MR0

    // Activer l'IRQ dans le NVIC
    NVIC_EnableIRQ(CTIMER0_IRQn);

    // Démarrer le timer
    LPC_CTIMER0->TCR = 0x01;
}


void CTIMER0_IRQHandler(void)
{
    if (LPC_CTIMER0->IR & (1 << 0)) {   // MR0 match flag
        millis++;                        // incrémente le compteur millisecondes
        LPC_CTIMER0->IR |= (1 << 0);     // clear flag
    }
}

void Delay_ms(uint32_t ms)
{
    uint32_t start = millis;
    while ((millis - start) < ms)
    {
        __WFI(); // optionnel, économise CPU
    }
}

int main(void) {
	// Configuration de l'horloge à 15 MHz
	LPC_PWRD_API->set_fro_frequency(30000);
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON | GPIO0 | SWM | CTIMER0 | GPIO_INT | SPI0);

	// Initialisation du compteur
    SystemCoreClockUpdate();
    __enable_irq();
    init_ctimer0_ms();

	// Initialisation du SPI
	Config_Spi();

	// Initialisation des entrées et sorties GPIO
	Config_Digital_Write(LED_D2);
	Config_Digital_Write(LED_D4);
	Config_Digital_Read(BP1);
	Config_Digital_Read(BP2);

	Config_Digital_Read(BUSY_PIN);
	Config_Digital_Write(RST_PIN);
	Config_Digital_Write(DC_PIN);
	//Config_Digital_Write(SSEL_PIN);


	// Initialisation de l'écran
	init_lcd();
	lcd_position(0, 0);
	lcd_puts("test de l'écran :");

	Delay_ms(1000);  // test délai 1 seconde

	lcd_puts("fin du test.     ");

	// Test de l'écran
	EPD_test();

	while (1) {
		__WFI(); // optionnel, économise CPU
	}
	return 0;
}
