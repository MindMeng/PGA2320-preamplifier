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

#ifndef PGA_H_
#define PGA_H_

#include "pch.h"

#define PGA_DELAY asm("nop")

#define PGA_CHANNELS 2

#define PGA_CS_DDR DDRC
#define PGA_CS_PORT PORTC
#define PGA_CS_PIN 1 << 7

#define PGA_SDI_DDR DDRC
#define PGA_SDI_PORT PORTC
#define PGA_SDI_PIN 1 << 6

#define PGA_CLK_DDR DDRC
#define PGA_CLK_PORT PORTC
#define PGA_CLK_PIN 1 << 5

#define PGA_MUTE_DDR DDRC
#define PGA_MUTE_PORT PORTC
#define PGA_MUTE_PIN 1 << 4

void PGA_init(void);
void PGA_transmit(uint8_t volume, uint8_t mute);

#endif
