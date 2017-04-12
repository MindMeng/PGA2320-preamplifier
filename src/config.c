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

#include "config.h"

// config object
struct Config* config = NULL;

// inputs available in the preamplifier
char* inputs[PREAMP_TOTAL_INPUTS] = {"PC", "CD", "TUNER", "AUX", "DAC", "PHONO"};

void Config_init(void)
{
  if(config == NULL) {
    config = (struct Config*)malloc(sizeof(struct Config));
  }  

  // wait until EEPROM is ready
  eeprom_busy_wait();

  // load config from EEPROM
  eeprom_read_block(config, (void*)CONFIG_LOCATION, sizeof(struct Config));

  // check magic value
  if(config->magic != CONFIG_MAGIC)
    Config_reset();
}

struct Config* Config_get(void)
{
  return config;
}

void Config_save(void)
{
  // wait until EEPROM is ready
  eeprom_busy_wait();

  // save config to EEPROM
  eeprom_write_block(config, (void*)CONFIG_LOCATION, sizeof(struct Config));
}

void Config_reset(void)
{
  memset(config, 0, sizeof(struct Config));

  // copy default input names
  for(int8_t i = 0; i < PREAMP_TOTAL_INPUTS; i++) {
    memset(config->inputNames[i], ' ', PREAMP_INPUT_LENGTH - 1);
    memcpy(config->inputNames[i], inputs[i], strlen(inputs[i]));
  }

  // set MAIN opto to OFF
  config->optoMain = PREAMP_TOTAL_INPUTS;

  // backlight at full
  config->backlight = 0xFF;

  // input volume is zero (no gain or loss towards master volume)
  for(int8_t i = 0; i < PREAMP_TOTAL_INPUTS; i++)
    config->volume[i] = 0;

  // fixed volume to false (0xff)
  for(int8_t i = 0; i < PREAMP_TOTAL_INPUTS; i++)
    config->fixedVolume[i] = 0xff;

  // master volue to minimim
  config->masterVolume = PREAMP_GAIN_MIN;

  // magic value
  config->magic = CONFIG_MAGIC;
}
