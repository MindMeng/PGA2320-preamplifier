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

#include "dev/button.h"
#include "dev/encoder.h"
#include "dev/HD44780.h"
#include "dev/PGA.h"
#include "dev/relays.h"
#include "dev/twi.h"
#include "config.h"
#include "preamp.h"
#include "remote.h"
#include "tasks.h"
#include "welcome.h"

int main(void)
{
  DDRA = DDRB = DDRC = DDRD = 0xFF;
  PORTA = PORTB = PORTC = PORTD = 0;

  LCD_init();
  PGA_init();
  TWI_init();
  Relays_init();
  Tasks_init();
  Config_init();
  Remote_init();
  Encoder01_init();
  Encoder02_init();
  Button01_init();
  Button02_init();

  // enable all interrupts
  sei();

  // initialize
  Tasks_create(0, TASK_SCHEDULE, Welcome_init, Preamp_init);

  while(TRUE)
    Tasks_poll();
}
