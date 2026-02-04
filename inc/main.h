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

#include "EPD_Test.h"

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

#define BUSY_PIN	P0_10 // BUSY
#define RST_PIN		P0_16 // RST
#define DC_PIN		P0_15 // DC

#define SSEL_PIN    P0_21 // CS
#define SCK_PIN     P0_19 // CLK
#define MOSI_PIN    P0_18 // DIN
#define SPIBAUD     115200

// Les pins P0_0 et P0_4 pourront être utilisés par l'UART à l'avenir

#define LED_D2 		P0_17
#define LED_D4 		P0_11
#define BP1 		P0_13
#define BP2 		P0_12

#define SYSTEM_CORE_CLOCK 15000000UL  // fréquence CPU
#define SYSTICK_TIME 15000 // 1 ms pour clk 15 MHz

void Config_Digital_Write(uint32_t pin);
void Config_Digital_Read(uint32_t pin);
void Digital_Write(uint32_t pin, uint32_t value);
uint32_t Digital_Read(uint32_t pin);
void init_ctimer0(void);
void Delay_ms(uint32_t ms);
