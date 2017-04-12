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

#ifndef PREAMP_H_
#define PREAMP_H_

#include "pch.h"
#include "remote.h"
#include "dev/encoder.h"
#include "dev/button.h"
#include "dev/opto.h"

// alphnumeric charset length
#define PREAMP_CHARSET_LENGTH 37

// total amount of inputs
#define PREAMP_TOTAL_INPUTS 6

// maximum input length including the termination character
#define PREAMP_INPUT_LENGTH 9

// backlight step range (51 = 5 steps since 255 / 5 = 51)
#define PREAMP_BACKLIGHT_STEP 51

// minimum gain
#define PREAMP_GAIN_MIN 0

// maximum gain
#define PREAMP_GAIN_MAX 212

// gain step (2 = 1 dB)
#define PREAMP_GAIN_STEP 2

// input range in dB (+- towards the master volume)
#define PREAMP_INPUT_VOL_RANGE 6

enum PreampMode_t {
  PREAMP_MODE_FIRST = 0,
  PREAMP_MODE_NORMAL = PREAMP_MODE_FIRST,
  PREAMP_MODE_LEARN_REMOTE,
  PREAMP_MODE_LEARN_INPUT,
  PREAMP_MODE_LEARN_OPTO,
  PREAMP_MODE_VOLUME_FIXED,
  PREAMP_MODE_VOLUME_INPUT,

  // not listed since we can't navigate to it
  PREAMP_MODE_STANDBY,

  // last one
  PREAMP_MODE_LAST = PREAMP_MODE_VOLUME_INPUT
};

struct Preamp {
  struct Config* config;
  enum PreampMode_t mode;

  uint8_t mustUpdateRelay;
  uint8_t mustUpdateDevice;
  uint8_t mustUpdateDisplay;
  uint8_t mustSaveConfig;
  uint8_t mustIgnoreRemote[REMOTECODE_TOTAL];

  int8_t learnInput;
  int8_t learnInputCursorPosition;
  int8_t learnRemoteCode;
  int8_t learnFixedVolume;

  int8_t attenuation;

  struct Task* updateRelayTask;
  struct Task* updateConfigTask;
};

extern struct Preamp* preamp;

void Preamp_init(void);

void Preamp_updateRelay(void);
void Preamp_updateDevice(void);
void Preamp_updateConfig(void);
void Preamp_updateDisplay(void);
void Preamp_updateStatus(void);

void Preamp_onSourceChange(enum Encoderstate_t state);
void Preamp_onVolumeChange(enum Encoderstate_t state);
void Preamp_onMuteChange(enum Buttonstate_t state);
void Preamp_onModeChange(enum Buttonstate_t state);

void Preamp_setBacklight(int8_t step);
void Preamp_setInput(int8_t input);
void Preamp_wakeup();
void Preamp_sleep();

#endif
