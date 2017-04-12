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

#include "encoder.h"
#include "tasks.h"

// current state of the encoder
enum Encoderstate_t encoder01State = ENCODER_IDLE;
enum Encoderstate_t encoder02State = ENCODER_IDLE;

// list containing listeners
struct EncoderNode* encoder01Listeners = NULL;
struct EncoderNode* encoder02Listeners = NULL;

void __Encoder01_tick(void)
{
  struct EncoderNode* node = encoder01Listeners;
  while(node) {
    node->listener(encoder01State);
    node = node->next;
  }
}

void __Encoder02_tick(void)
{
  struct EncoderNode* node = encoder02Listeners;
  while(node) {
    node->listener(encoder02State);
    node = node->next;
  }
}

void __Encoder01_dispatch(void)
{
  if(encoder01State != ENCODER_IDLE) {
    __Encoder01_tick();	
    encoder01State = ENCODER_IDLE;
  }
}

void __Encoder02_dispatch(void)
{
  if(encoder02State != ENCODER_IDLE) {
    __Encoder02_tick();
    encoder02State = ENCODER_IDLE;
  }
}

void Encoder01_init(void)
{
  DDRD &= ~(1 << 1 | 1 << 2);
  PORTD |= (1 << 1 | 1 << 2);

  MCUCR |= (1 << ISC00);
  GICR |= (1 << INT0);

  // create task
  Tasks_create(125, TASK_REPEAT, __Encoder01_dispatch, NULL); 
}

void Encoder02_init(void)
{
  DDRD &= ~(1 << 3 | 1 << 4);
  PORTD |= (1 << 3 | 1 << 4);

  MCUCR |= (1 << ISC10);
  GICR |= (1 << INT1);

  // create task
  Tasks_create(50, TASK_REPEAT, __Encoder02_dispatch, NULL);
}

void Encoder01_join(void (*listener)(enum Encoderstate_t))
{
  struct EncoderNode* node = (struct EncoderNode*)malloc(sizeof(struct EncoderNode));

  // it will be the last entry, so no next node
  node->next = NULL;

  // node values
  node->listener = listener;

  if(encoder01Listeners == NULL) { // no main node yet
    encoder01Listeners = node;
  } else {
    struct EncoderNode* parent = encoder01Listeners;
    while(parent->next) {
      parent = parent->next;
    }

    parent->next = node;
  }
}

void Encoder02_join(void (*listener)(enum Encoderstate_t))
{
  struct EncoderNode* node = (struct EncoderNode*)malloc(sizeof(struct EncoderNode));

  // it will be the last entry, so no next node
  node->next = NULL;

  // node values
  node->listener = listener;

  if(encoder02Listeners == NULL) { // no main node yet
    encoder02Listeners = node;
  } else {
    struct EncoderNode* parent = encoder02Listeners;
    while(parent->next) {
      parent = parent->next;
    }

    parent->next = node;
  }
}

ISR(INT0_vect)
{
  uint8_t graycode = 0;

  if(PIND & (1 << 2))
    graycode |= 1 << 1;

  if(PIND & (1 << 1))
    graycode |= 1 << 0;

  if(graycode == 1 || graycode == 2) // 01 or 10
    encoder01State = ENCODER_LEFT;
  else // 11 or 00
    encoder01State = ENCODER_RIGHT;
}

ISR(INT1_vect)
{
  uint8_t graycode = 0;

  if(PIND & (1 << 4))
    graycode |= 1 << 1;

  if(PIND & (1 << 3))
    graycode |= 1 << 0;

  if(graycode == 1 || graycode == 2) // 01 or 10
    encoder02State = ENCODER_RIGHT;
  else // 11 or 00    
    encoder02State = ENCODER_LEFT;
}
