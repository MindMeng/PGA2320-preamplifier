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

#include "HD44780.h"

// LCD text buffer
char g_displayBuffer[16];

void _LCD_OutNibble(unsigned char nibbleToWrite)
{
  if(nibbleToWrite & 0x01)
    LCD_DB4_PORT |= LCD_DB4;
  else
    LCD_DB4_PORT  &= ~LCD_DB4;

  if(nibbleToWrite & 0x02)
    LCD_DB5_PORT |= LCD_DB5;
  else
    LCD_DB5_PORT  &= ~LCD_DB5;

  if(nibbleToWrite & 0x04)
    LCD_DB6_PORT |= LCD_DB6;
  else
    LCD_DB6_PORT  &= ~LCD_DB6;

  if(nibbleToWrite & 0x08)
    LCD_DB7_PORT |= LCD_DB7;
  else
    LCD_DB7_PORT  &= ~LCD_DB7;
}

unsigned char _LCD_InNibble(void)
{
  unsigned char tmp = 0;

  if(LCD_DB4_PIN & LCD_DB4)
    tmp |= (1 << 0);
  if(LCD_DB5_PIN & LCD_DB5)
    tmp |= (1 << 1);
  if(LCD_DB6_PIN & LCD_DB6)
    tmp |= (1 << 2);
  if(LCD_DB7_PIN & LCD_DB7)
    tmp |= (1 << 3);
  return tmp;
}

void _LCD_Write(unsigned char dataToWrite)
{
  LCD_DB4_DIR |= LCD_DB4;
  LCD_DB5_DIR |= LCD_DB5;
  LCD_DB6_DIR |= LCD_DB6;
  LCD_DB7_DIR |= LCD_DB7;

  LCD_RW_PORT &= ~LCD_RW;
  LCD_E_PORT |= LCD_E;
  _LCD_OutNibble(dataToWrite >> 4);
  LCD_E_PORT &= ~LCD_E;
  LCD_E_PORT |= LCD_E;
  _LCD_OutNibble(dataToWrite);
  LCD_E_PORT &= ~LCD_E;
  while(LCD_readStatus() & 0x80);
}

unsigned char _LCD_Read(void)
{
  unsigned char tmp = 0;
  LCD_DB4_DIR &= ~LCD_DB4;
  LCD_DB5_DIR &= ~LCD_DB5;
  LCD_DB6_DIR &= ~LCD_DB6;
  LCD_DB7_DIR &= ~LCD_DB7;

  LCD_RW_PORT |= LCD_RW;
  LCD_E_PORT |= LCD_E;
  tmp |= (_LCD_InNibble() << 4);
  LCD_E_PORT &= ~LCD_E;
  LCD_E_PORT |= LCD_E;
  tmp |= _LCD_InNibble();
  LCD_E_PORT &= ~LCD_E;
  return tmp;
}

void LCD_writeCommand(unsigned char commandToWrite)
{
  LCD_RS_PORT &= ~LCD_RS;
  _LCD_Write(commandToWrite);
}

unsigned char LCD_readStatus(void)
{
  LCD_RS_PORT &= ~LCD_RS;
  return _LCD_Read();
}

void LCD_writeData(unsigned char dataToWrite)
{
  LCD_RS_PORT |= LCD_RS;
  _LCD_Write(dataToWrite);
}

unsigned char LCD_readData(void)
{
  LCD_RS_PORT |= LCD_RS;
  return _LCD_Read();
}

void LCD_writeText(char * text)
{
  while(*text)
    LCD_writeData(*text++);
}

void LCD_goto(unsigned char x, unsigned char y)
{
  LCD_writeCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}

void LCD_clear(void)
{
  LCD_writeCommand(HD44780_CLEAR);
  _delay_ms(2);
}

void LCD_home(void)
{
  LCD_writeCommand(HD44780_HOME);
  _delay_ms(2);
}

void LCD_intensity(uint8_t intensity)
{
  OCR2 = intensity;
}

void LCD_init(void)
{
  unsigned char i;

  // LCD Backlight pin
  LCD_LIGHT_DIR |= LCD_LIGHT;
  LCD_LIGHT_PORT |= LCD_LIGHT;

  // fast PWM mode (LCD Backlight)
  TCCR2 |= (1 << WGM20) | (1 << WGM21);
  TCCR2 |= (1 << COM21) | (0 << CS20) | (1 << CS21) | (1 << CS22);

  // maximum light
  OCR2 = 0xFF;

  LCD_DB4_DIR |= LCD_DB4;
  LCD_DB5_DIR |= LCD_DB5;
  LCD_DB6_DIR |= LCD_DB6;
  LCD_DB7_DIR |= LCD_DB7;
  LCD_E_DIR 	|= LCD_E;  
  LCD_RS_DIR 	|= LCD_RS; 
  LCD_RW_DIR 	|= LCD_RW; 
  _delay_ms(15);
  LCD_RS_PORT &= ~LCD_RS;
  LCD_E_PORT &= ~LCD_E;
  LCD_RW_PORT &= ~LCD_RW;
  for(i = 0; i < 3; i++) {
    LCD_E_PORT |= LCD_E;
    _LCD_OutNibble(0x03);
    LCD_E_PORT &= ~LCD_E;
    _delay_ms(5);
  }

  LCD_E_PORT |= LCD_E;
  _LCD_OutNibble(0x02);
  LCD_E_PORT &= ~LCD_E;

  _delay_ms(1);
  LCD_writeCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT);
  LCD_writeCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF);
  LCD_writeCommand(HD44780_CLEAR);
  LCD_writeCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);
  LCD_writeCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK);
}
