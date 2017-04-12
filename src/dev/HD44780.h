/*
 * PGA2320 preamplifier for Atmel AVR microcontrollers
 * Copyright (c) 2017 Konrad Kusnierz <iryont@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef HD44780_H_
#define HD44780_H_

#include "pch.h"

#define LCD_LIGHT_DIR DDRD
#define LCD_LIGHT_PORT PORTD
#define LCD_LIGHT_PIN PIND
#define LCD_LIGHT   (1 << 7)

#define LCD_RS_DIR		DDRD
#define LCD_RS_PORT 	PORTD
#define LCD_RS_PIN		PIND
#define LCD_RS			(1 << 0)

#define LCD_RW_DIR		DDRB
#define LCD_RW_PORT		PORTB
#define LCD_RW_PIN		PINB
#define LCD_RW			(1 << 4)

#define LCD_E_DIR		DDRB
#define LCD_E_PORT		PORTB
#define LCD_E_PIN		PINB
#define LCD_E			(1 << 3)

#define LCD_DB4_DIR		DDRB
#define LCD_DB4_PORT	PORTB
#define LCD_DB4_PIN		PINB
#define LCD_DB4			(1 << 1)

#define LCD_DB5_DIR		DDRB
#define LCD_DB5_PORT	PORTB
#define LCD_DB5_PIN		PINB
#define LCD_DB5			(1 << 0)

#define LCD_DB6_DIR		DDRA
#define LCD_DB6_PORT	PORTA
#define LCD_DB6_PIN		PINA
#define LCD_DB6			(1 << 0)

#define LCD_DB7_DIR		DDRA
#define LCD_DB7_PORT	PORTA
#define LCD_DB7_PIN		PINA
#define LCD_DB7			(1 << 1)


#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
#define HD44780_EM_SHIFT_CURSOR		0
#define HD44780_EM_SHIFT_DISPLAY	1
#define HD44780_EM_DECREMENT		0
#define HD44780_EM_INCREMENT		2

#define HD44780_DISPLAY_ONOFF			0x08
#define HD44780_DISPLAY_OFF			0
#define HD44780_DISPLAY_ON			4
#define HD44780_CURSOR_OFF			0
#define HD44780_CURSOR_ON			2
#define HD44780_CURSOR_NOBLINK		0
#define HD44780_CURSOR_BLINK		1

#define HD44780_DISPLAY_CURSOR_SHIFT	0x10
#define HD44780_SHIFT_CURSOR		0
#define HD44780_SHIFT_DISPLAY		8
#define HD44780_SHIFT_LEFT			0
#define HD44780_SHIFT_RIGHT			4

#define HD44780_FUNCTION_SET			0x20
#define HD44780_FONT5x7				0
#define HD44780_FONT5x10			4
#define HD44780_ONE_LINE			0
#define HD44780_TWO_LINE			8
#define HD44780_4_BIT				0
#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET				0x40

#define HD44780_DDRAM_SET				0x80

void LCD_writeCommand(unsigned char);
unsigned char LCD_readStatus(void);
void LCD_writeData(unsigned char);
unsigned char LCD_readData(void);
void LCD_writeText(char *);
void LCD_goto(unsigned char, unsigned char);
void LCD_clear(void);
void LCD_home(void);
void LCD_intensity(uint8_t intensity);
void LCD_init(void);

extern char g_displayBuffer[];

#endif
