/*
 * Copyright (c) 2014-2015
 * Konrad Kusnierz <iryont@gmail.com>
 *
 * All information, intellectual and technical concepts contained herein is,
 * and remains the property of the above person. Dissemination of this
 * information or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained.
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
