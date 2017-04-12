/*
 * Copyright (c) 2014-2015
 * Konrad Kusnierz <iryont@gmail.com>
 *
 * All information, intellectual and technical concepts contained herein is,
 * and remains the property of the above person. Dissemination of this
 * information or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained.
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
