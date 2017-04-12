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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "pch.h"
#include "remote.h"
#include "preamp.h"

#define CONFIG_MAGIC 0x64FD
#define CONFIG_LOCATION 0x0A

struct Config {
  uint8_t input;
  uint8_t mute;
  uint8_t backlight;
  uint8_t sleep;

  // volume for each input within certain range
  int8_t volume[PREAMP_TOTAL_INPUTS];

  // input names
  char inputNames[PREAMP_TOTAL_INPUTS][PREAMP_INPUT_LENGTH];

  // fixed volume options
  uint8_t fixedVolume[PREAMP_TOTAL_INPUTS];

  // remote codes
  uint16_t remoteCodes[REMOTECODE_TOTAL];

  // master volue
  uint8_t masterVolume;

  // optotransistor is turned on for specific input
  int8_t optoMain;

  // magic value
  uint16_t magic;
} __attribute__((packed));

void Config_init(void);
struct Config* Config_get(void);
void Config_save(void);
void Config_reset(void);

#endif
