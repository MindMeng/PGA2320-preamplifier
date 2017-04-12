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

#include "welcome.h"
#include "tasks.h"
#include "crypt/base.h"
#include "dev/HD44780.h"

void Welcome_init(void)
{
  Tasks_create(10, TASK_SCHEDULE, Welcome_stepOne, NULL);
}

void Welcome_stepOne(void)
{
  LCD_clear();
  LCD_goto(0, 0);
  LCD_writeText("preamp 1.0.8");
  LCD_goto(0, 1);
  LCD_writeText(" by taz");

  // next step
  Tasks_create(1000, TASK_SCHEDULE, Welcome_stepTwo, NULL);
}

void Welcome_stepTwo(void)
{
  LCD_clear();
  LCD_goto(0, 0);
  LCD_writeText("soft " __DATE__);
  LCD_goto(0, 1);
  LCD_writeText(" by .3lite");

  // next step
  Tasks_create(1000, TASK_SCHEDULE, Welcome_onFinish, NULL);
}

void Welcome_onFinish(void)
{
  // ?
}
