/*
 * lib_ENS_II1_lcd.c
 *
 *  Created on: 9 oct. 2018
 *  Modified on sept 2019
 *      Author: juton
 */

#include "lib_ENS_II1_lcd_v2.h"
#include <stdio.h>
#include "fro.h"
#include "rom_api.h"
#include "syscon.h"
#include "swm.h"
#include "i2c.h"

void init_lcd(void) {

	uint32_t i;
	uint8_t I2CMasterBuffer[BUFSIZE];
	uint32_t I2CWriteLength;

	// Provide main_clk as function clock to I2C0
	LPC_SYSCON->I2C0CLKSEL = FCLKSEL_MAIN_CLK;

	LPC_SYSCON->SYSAHBCLKCTRL0 |= I2C0 | SWM;

	//configuration des pins 7 (SDA) et 14 (SCL) en open_drain
	LPC_IOCON->PIO0_7 = 0x000004A0;
	LPC_IOCON->PIO0_14 = 0x000004A0;

	//assignation des pins 7 à SDA et 14 à SCL
	LPC_SWM->PINASSIGN5 = 0xFFFF0E07;

	// Give I2C0 a reset
	LPC_SYSCON->PRESETCTRL0 &= (I2C0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(I2C0_RST_N);

	//horloge I2c
	LPC_I2C0->DIV = 1;
	LPC_I2C0->CFG = CFG_MSTENA;

	for (i = 0; i < 8000; i++)
		;
	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CWriteLength = 1;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x38; /* configuration value, no change from default */
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CWriteLength = 2;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x39;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x14;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x74;							//contrast
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x54;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x6F;							//follower
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x0E;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x01;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x06;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;

	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CMasterBuffer[2] = 0x02;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;
}

void lcd_puts(char *text_input) {
	uint8_t I2CMasterBuffer[BUFSIZE];
	uint32_t I2CWriteLength;
	int i = 0;
	while (text_input[i] != 0) {
		while (!(LPC_I2C0->STAT & STAT_MSTPEND))
			; // Wait for the master state to be idle
		I2CWriteLength = 2;
		I2CMasterBuffer[0] = LCD_ADDR;
		I2CMasterBuffer[1] = 0x40;
		I2CMasterBuffer[2] = text_input[i];
		I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
		i++;
	}
}

void lcd_putc(char c) {
	uint8_t I2CMasterBuffer[BUFSIZE];
	uint32_t I2CWriteLength;
	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CWriteLength = 2;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x40;
	I2CMasterBuffer[2] = c;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
}

void lcd_gohome(void) {
	uint8_t I2CMasterBuffer[BUFSIZE];
	uint32_t I2CWriteLength, i;
	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CWriteLength = 2;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x0;
	I2CMasterBuffer[2] = 0x02;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
	for (i = 0; i < 8000; i++)
		;
}

void lcd_position(char ligne, char colonne) {
	uint8_t I2CMasterBuffer[BUFSIZE];
	uint32_t I2CWriteLength;
	char cmd;
	if (ligne > 1)
		ligne = 1;
	if (colonne > 16)
		colonne = 16;
	cmd = 0x80 + (ligne * 0x40) + colonne;
	while (!(LPC_I2C0->STAT & STAT_MSTPEND))
		; // Wait for the master state to be idle
	I2CWriteLength = 2;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x0;
	I2CMasterBuffer[2] = cmd;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength);
}
