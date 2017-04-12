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

#include "button.h"
#include "const.h"
#include "tasks.h"

// current state of the button
enum Buttonstate_t button01State = BUTTON_IDLE;
enum Buttonstate_t button02State = BUTTON_IDLE;

// lists containing listeners
struct ButtonNode* button01Listeners = NULL;
struct ButtonNode* button02Listeners = NULL;

// flag which indicates state cleanup on the next dispatch
uint8_t button01IdleNext = FALSE;
uint8_t button02IdleNext = FALSE;

void __Button01_dispatch(void)
{
  if(button01IdleNext) {
    button01State = BUTTON_IDLE;
    button01IdleNext = FALSE;
  }

  if(button01State != BUTTON_IDLE) {
    struct ButtonNode* node = button01Listeners;
    while(node) {
      node->listener(button01State);
      node = node->next;
    }

    button01IdleNext = TRUE;
  }
}

void __Button02_dispatch(void)
{
  if(button02IdleNext) {
    button02State = BUTTON_IDLE;
    button02IdleNext = FALSE;
  }

  if(button02State != BUTTON_IDLE) {
    struct ButtonNode* node = button02Listeners;
    while(node) {
      node->listener(button02State);
      node = node->next;
    }

    button02IdleNext = TRUE;
  }
}

void __Button01_poll(void)
{
  if((PIND & (1 << 6)) == 0)
    button01State = BUTTON_PUSHED;
}

void __Button02_poll(void)
{
  if((PIND & (1 << 5)) == 0)
    button02State = BUTTON_PUSHED;
}

void Button01_init(void)
{
  DDRD |= 1 << 6;
  PORTD |= 1 << 6;

  // create tasks
  Tasks_create(100, TASK_REPEAT, __Button01_poll, NULL);
  Tasks_create(400, TASK_REPEAT, __Button01_dispatch, NULL);
}

void Button02_init(void)
{
  DDRD |= 1 << 5;
  PORTD |= 1 << 5;

  // create tasks
  Tasks_create(100, TASK_REPEAT, __Button02_poll, NULL);
  Tasks_create(400, TASK_REPEAT, __Button02_dispatch, NULL);
}

void Button01_join(void (*listener)(enum Buttonstate_t))
{
  struct ButtonNode* node = (struct ButtonNode*)malloc(sizeof(struct ButtonNode));

  // it will be the last entry, so no next node
  node->next = NULL;

  // node values
  node->listener = listener;

  if(button01Listeners == NULL) { // no main node yet
    button01Listeners = node;
    } else {
    struct ButtonNode* parent = button01Listeners;
    while(parent->next) {
      parent = parent->next;
    }

    parent->next = node;
  }
}

void Button02_join(void (*listener)(enum Buttonstate_t))
{
  struct ButtonNode* node = (struct ButtonNode*)malloc(sizeof(struct ButtonNode));

  // it will be the last entry, so no next node
  node->next = NULL;

  // node values
  node->listener = listener;

  if(button02Listeners == NULL) { // no main node yet
    button02Listeners = node;
  } else {
    struct ButtonNode* parent = button02Listeners;
    while(parent->next) {
      parent = parent->next;
    }

    parent->next = node;
  }
}
