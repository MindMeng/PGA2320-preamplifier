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

#include "PGA.h"

void PGA_init(void)
{
  PGA_CS_DDR |= PGA_CS_PIN;
  PGA_SDI_DDR |= PGA_SDI_PIN;
  PGA_CLK_DDR |= PGA_CLK_PIN;
  PGA_MUTE_DDR |= PGA_MUTE_PIN;

  // mute
  PGA_MUTE_PORT &= ~(PGA_MUTE_PIN);

  // not possible to write
  PGA_CS_PORT |= PGA_CS_PIN;
}

void PGA_transmit(uint8_t volume, uint8_t mute)
{
  PGA_CS_PORT &= ~(PGA_CS_PIN);
  PGA_DELAY;

  for(uint8_t channel = 1; channel <= PGA_CHANNELS; channel++) {
    for(uint8_t bit = 1 << 7; bit > 0; bit >>= 1) {
      if((volume & bit) == bit)
        PGA_SDI_PORT |= (PGA_SDI_PIN);
      else
        PGA_SDI_PORT &= ~(PGA_SDI_PIN);

      PGA_DELAY;

      // clock
      PGA_CLK_PORT |= (PGA_CLK_PIN);
      PGA_DELAY;
      PGA_CLK_PORT &= ~(PGA_CLK_PIN);
    }
  }

  PGA_DELAY;
  PGA_CS_PORT |= (PGA_CS_PIN);

  if(mute)
    PGA_MUTE_PORT &= ~(PGA_MUTE_PIN);
  else
    PGA_MUTE_PORT |= PGA_MUTE_PIN;
}
