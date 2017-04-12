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

#ifndef TASKS_H_
#define TASKS_H_

#include "pch.h"
#include "const.h"

#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

enum Schedule_t {
  TASK_REPEAT = 0,
  TASK_SCHEDULE = 1
};

struct Task {
  // list containing all tasks
  struct Task* next;

  // node values
  uint16_t millis;
  enum Schedule_t schedule;
  void (*listener)(void);
  void (*callback)(void);

  // ticks related values
  uint16_t ticks;
  BOOL running;
};

void Tasks_init(void);
void Tasks_poll(void);
uint16_t Tasks_size(void);
struct Task* Tasks_create(uint16_t millis, enum Schedule_t schedule, void (*listener)(void), void (*callback)(void));

#endif
