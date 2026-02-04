/*
 * lib_ENS_II1_lcd.h
 *
 *  Created on: 9 oct. 2018
 *  Modified on sept. 2019
 *      Author: juton
 */

#ifndef LIB_ENS_II1_LCD_H_
#define LIB_ENS_II1_LCD_H_

//definitions pour I2C
#define I2CBAUD 		100000
#define LCD_ADDR		0x7C
#define BUFSIZE       	5

void init_lcd(void);
void lcd_puts();
void lcd_putc(char c);
void lcd_gohome();
void lcd_position(char ligne, char colonne);

#endif /* LIB_ENS_II1_LCD_H_ */
