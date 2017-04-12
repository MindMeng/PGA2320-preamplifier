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

#include "preamp.h"
#include "tasks.h"
#include "config.h"
#include "dev/HD44780.h"
#include "dev/PGA.h"
#include "dev/relays.h"

// preamp structure with all the necessary information (input, mute, flags)
struct Preamp* preamp = NULL;

// remote names for learning purpose
char* buttons[REMOTECODE_TOTAL] = {"Volume up", "Volume down", "Mute", "Light up", "Light down", NULL, NULL, NULL, NULL, NULL, NULL, "Toggle Opto-MAIN", "Standby", "Attenuation"};

// ignore times for remote buttons
int8_t ignore[REMOTECODE_TOTAL] = {1, 1, 12, 3, 3, 9, 9, 9, 9, 9, 9, 9, 9, 9};

// charset for input names
char charset[PREAMP_CHARSET_LENGTH] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ' '};

void Preamp_init(void)
{
  // create preamp structure
  preamp = (struct Preamp*)malloc(sizeof(struct Preamp));

  // zero-fill
  memset(preamp, 0, sizeof(struct Preamp));

  // load configuration
  preamp->config = Config_get();

  // copy input names to button names - addresses only
  for(int8_t i = 0; i < PREAMP_TOTAL_INPUTS; i++)
    buttons[REMOTECODE_BACKLIGHT_DOWN + 1 + i] = preamp->config->inputNames[i];

  // flags
  preamp->mustUpdateRelay = TRUE;
  preamp->mustUpdateDevice = TRUE;
  preamp->mustUpdateDisplay = TRUE;
  preamp->mustSaveConfig = FALSE;

  // normal mode or go to sleep
  if(preamp->config->sleep)
    Preamp_sleep();
  else
    preamp->mode = PREAMP_MODE_NORMAL;

  Encoder01_join(Preamp_onSourceChange);
  Encoder02_join(Preamp_onVolumeChange);
  Button01_join(Preamp_onModeChange);
  Button02_join(Preamp_onMuteChange);

  // create private tasks
  Tasks_create(100, TASK_REPEAT, Preamp_updateDevice, NULL);
  Tasks_create(100, TASK_REPEAT, Preamp_updateDisplay, NULL);
  Tasks_create(100, TASK_REPEAT, Preamp_updateStatus, NULL);

  // create public tasks
  preamp->updateRelayTask = Tasks_create(500, TASK_REPEAT, Preamp_updateRelay, NULL);
  preamp->updateConfigTask = Tasks_create(4000, TASK_REPEAT, Preamp_updateConfig, NULL);
}

void Preamp_updateRelay(void)
{
  if(preamp->mustUpdateRelay) {
    if(preamp->mode != PREAMP_MODE_STANDBY) {
      Relays_open(preamp->config->input);

      if(preamp->config->optoMain == preamp->config->input || preamp->config->optoMain > PREAMP_TOTAL_INPUTS)
        Opto_on(PREAMP_OPTO_MAIN);
      else
        Opto_off(PREAMP_OPTO_MAIN);

      // AUX opto is always on
      Opto_on(PREAMP_OPTO_AUX);
    }

    preamp->mustUpdateRelay = FALSE;
  }
}

void Preamp_updateDevice(void)
{
  if(preamp->mustUpdateDevice) {
    if(preamp->mode == PREAMP_MODE_STANDBY)
      PGA_transmit(0, TRUE);
    else {
      // is volume fixed?
      BOOL fixed = preamp->config->fixedVolume[preamp->config->input] != 0xff;

      // attenuation value for the PGA
      int16_t volume = 0;

      // calculate attenuation
      if(fixed)
        volume = preamp->config->fixedVolume[preamp->config->input];
      else
        volume = MAX(PREAMP_GAIN_MIN, MIN(PREAMP_GAIN_MAX, (int16_t)preamp->config->masterVolume + preamp->config->volume[preamp->config->input] - preamp->attenuation));

      // send to PGA
      PGA_transmit(volume, preamp->config->mute);
    }

    preamp->mustUpdateDevice = FALSE;
  }
}

void Preamp_updateConfig(void)
{
  if(preamp->mustSaveConfig) {
    Config_save();
    preamp->mustSaveConfig = FALSE;
  }
}

void Preamp_updateDisplay(void)
{
  if(preamp->mustUpdateDisplay) {
    LCD_clear();

    // light intensity
    if(preamp->mode == PREAMP_MODE_STANDBY)
      LCD_intensity(0);
    else
      LCD_intensity(preamp->config->backlight);

    switch(preamp->mode) {
      case PREAMP_MODE_LEARN_REMOTE: {
        LCD_goto(0, 0);
        LCD_writeText("Learn button:");
        LCD_goto(0, 1);
        LCD_writeText(buttons[preamp->learnRemoteCode]);
        break;
      }

      case PREAMP_MODE_LEARN_INPUT: {
        LCD_goto(preamp->learnInputCursorPosition, 0);
        LCD_writeText("v");

        LCD_goto(11, 0);
        sprintf(g_displayBuffer, "IN %d:", preamp->learnInput + 1);
        LCD_writeText(g_displayBuffer);

        LCD_goto(0, 1);
        LCD_writeText(preamp->config->inputNames[preamp->learnInput]);
        break;
      }

      case PREAMP_MODE_LEARN_OPTO: {
        int8_t optoInput = preamp->config->optoMain;

        LCD_goto(0, 0);
        sprintf(g_displayBuffer, "Opto MAIN:");
        LCD_writeText(g_displayBuffer);

        LCD_goto(0, 1);
        if(optoInput > PREAMP_TOTAL_INPUTS)
          LCD_writeText("Always on");
        else if(optoInput >= PREAMP_TOTAL_INPUTS)
          LCD_writeText("Always off");
        else
          LCD_writeText(preamp->config->inputNames[optoInput]);
        break;
      }

      case PREAMP_MODE_VOLUME_FIXED: {
        LCD_goto(0, 0);
        sprintf(g_displayBuffer, "%s vol:", preamp->config->inputNames[preamp->learnFixedVolume]);
        LCD_writeText(g_displayBuffer);

        LCD_goto(0, 1);
        if(preamp->config->fixedVolume[preamp->learnFixedVolume] == 0xff)
          LCD_writeText("Fixed vol. off");
        else {
          // calculate gain in dB
          int16_t gain = 315 - (5 * (255 - preamp->config->fixedVolume[preamp->learnFixedVolume]));

          // dB gain
          sprintf(g_displayBuffer, "Fixed: %d dB", gain / 10);
          LCD_writeText(g_displayBuffer);
        }

        break;
      }

      case PREAMP_MODE_VOLUME_INPUT: {
        LCD_goto(0, 0);
        sprintf(g_displayBuffer, "%s vol:", preamp->config->inputNames[preamp->config->input]);
        LCD_writeText(g_displayBuffer);

        // calculate gain in dB
        int16_t gain = preamp->config->volume[preamp->config->input] * 5;

        // dB gain
        sprintf(g_displayBuffer, "Adj: %d dB", gain / 10);
        LCD_goto(0, 1);
        LCD_writeText(g_displayBuffer);

        break;
      }

      case PREAMP_MODE_STANDBY: {
        // nothing
        break;
      }

      default: {
        // volume
        LCD_goto(0, 0);
        LCD_writeText("Volume: ");

        if(preamp->config->mute) {
          LCD_writeText("MUTE");
        } else {
          switch(preamp->config->masterVolume) {
            case PREAMP_GAIN_MIN: {
              LCD_writeText("MIN");
              break;
            }

            case PREAMP_GAIN_MAX: {
              LCD_writeText("MAX");
              break;
            }

            default: {
              if(preamp->attenuation)
                LCD_writeText("ATT");
              else {
                // is volume fixed for this channel?
                BOOL fixed = preamp->config->fixedVolume[preamp->config->input] != 0xff;

                // volume gain
                int16_t gain = 0;

                // calculate gain in dB
                if(fixed)
                  gain = 315 - (5 * (255 - preamp->config->fixedVolume[preamp->config->input]));
                else
                  gain = 315 - (5 * (255 - preamp->config->masterVolume));

                // fixed indicator
                if(fixed)
                  LCD_writeText("#");

                // dB gain
                sprintf(g_displayBuffer, "%d dB", gain / 10);
                LCD_writeText(g_displayBuffer);
              }

              break;
            }
          }
        }

        // signal input
        LCD_goto(0, 1);
        sprintf(g_displayBuffer, "Input:  %s", preamp->config->inputNames[preamp->config->input]);
        LCD_writeText(g_displayBuffer);
        break;
      }
    }

    preamp->mustUpdateDisplay = FALSE;
  }
}

void Preamp_updateStatus(void)
{
  struct RemoteCode* remoteCode = Remote_pop();

  // increase timeout for all remote codes
  for(uint8_t i = REMOTECODE_FIRST; i <= REMOTECODE_LAST; i++) {
    if(preamp->mustIgnoreRemote[i] != ignore[i]) {
      preamp->mustIgnoreRemote[i]++;
    }
  }

  if(remoteCode) {
    uint16_t command = U16(remoteCode->address, remoteCode->command);
    if(preamp->mode == PREAMP_MODE_LEARN_REMOTE) {
      BOOL previouslyUsed = FALSE;

      // check whether the code has been used before
      for(uint8_t i = 0; i < preamp->learnRemoteCode; i++) {
        if(command == preamp->config->remoteCodes[i]) {
          previouslyUsed = TRUE;
          break;
        }
      }

      // if not, lets save it
      if(!previouslyUsed) {
        preamp->config->remoteCodes[preamp->learnRemoteCode] = command;

        if(preamp->learnRemoteCode == REMOTECODE_LAST) {
          preamp->mode = PREAMP_MODE_NORMAL;
          preamp->mustSaveConfig = TRUE;
        } else {
          preamp->learnRemoteCode++;
        }
      }
    } else {
      for(uint8_t i = REMOTECODE_FIRST; i <= REMOTECODE_LAST; i++) {
        if(preamp->config->remoteCodes[i] == command) {
          if(preamp->mustIgnoreRemote[i] != ignore[i]) {
            break;
          }

          switch(i) {
            case REMOTECODE_VOLUME_UP: {
              Preamp_onVolumeChange(ENCODER_RIGHT);
              break;
            }

            case REMOTECODE_VOLUME_DOWN: {
              Preamp_onVolumeChange(ENCODER_LEFT);
              break;
            }

            case REMOTECODE_MUTE: {
              Preamp_onMuteChange(BUTTON_PUSHED);
              break;
            }

            case REMOTECODE_BACKLIGHT_UP: {
              Preamp_setBacklight(+PREAMP_BACKLIGHT_STEP);
              break;
            }

            case REMOTECODE_BACKLIGHT_DOWN: {
              Preamp_setBacklight(-PREAMP_BACKLIGHT_STEP);
              break;
            }

            case REMOTECODE_SOURCE_1:
            case REMOTECODE_SOURCE_2:
            case REMOTECODE_SOURCE_3:
            case REMOTECODE_SOURCE_4:
            case REMOTECODE_SOURCE_5:
            case REMOTECODE_SOURCE_6: {
              Preamp_setInput(i - REMOTECODE_SOURCE_1);
              break;
            }

            case REMOTECODE_TOGGLE_OPTO_AUX: {
              Opto_toggle(PREAMP_OPTO_MAIN);
              break;
            }

            case REMOTECODE_STANDBY_MODE: {
              if(preamp->mode == PREAMP_MODE_STANDBY)
                Preamp_wakeup();
              else
                Preamp_sleep();

              break;
            }

            case REMOTECODE_ATTENUATE: {
              if(preamp->attenuation)
                preamp->attenuation = 0;
              else
                preamp->attenuation = PREAMP_GAIN_STEP * 30;

              preamp->mustUpdateDevice = TRUE;
              break;
            }

            default: break;
          }

          // task should ignore next codes if delay is required
          preamp->mustIgnoreRemote[i] = 0;

          // nothing else to do
          break;
        }
      }
    }

    preamp->mustUpdateDisplay = TRUE;
  }
}

void Preamp_onSourceChange(enum Encoderstate_t state)
{
  switch(preamp->mode) {
    case PREAMP_MODE_LEARN_INPUT: {
      // choose next input
      int8_t input = preamp->learnInput + (state == ENCODER_LEFT ? -1 : +1);
      int8_t limit = PREAMP_TOTAL_INPUTS - 1;

      // circular rotation
      if(input < 0)
        input = limit;
      if(input > limit)
        input = 0;

      preamp->learnInputCursorPosition = 0;
      preamp->learnInput = input;
      preamp->mustUpdateDisplay = TRUE;
      break;
    }

    case PREAMP_MODE_NORMAL: {
      // choose next input
      int8_t input = preamp->config->input + (state == ENCODER_LEFT ? -1 : +1);
      int8_t limit = PREAMP_TOTAL_INPUTS - 1;

      // circular rotation
      if(input < 0)
        input = limit;
      if(input > limit)
        input = 0;

      Preamp_setInput(input);
      break;
    }


    case PREAMP_MODE_STANDBY: {
      Preamp_wakeup();
      break;
    }

    default: break;
  }
}

void Preamp_onVolumeChange(enum Encoderstate_t state)
{
  switch(preamp->mode) {
    case PREAMP_MODE_LEARN_INPUT: {
      // load old character
      uint8_t character = preamp->config->inputNames[preamp->learnInput][preamp->learnInputCursorPosition];

      // choose new character
      for(uint8_t i = 0; i < PREAMP_CHARSET_LENGTH; i++) {
        if((char)character == charset[i]) {
          if(state == ENCODER_LEFT)
            character = i == 0 ? charset[PREAMP_CHARSET_LENGTH - 1] : charset[i - 1];
          else if(state == ENCODER_RIGHT)
            character = i == PREAMP_CHARSET_LENGTH - 1 ? charset[0] : charset[i + 1];

          break;
        }
      }

      // save new character
      preamp->config->inputNames[preamp->learnInput][preamp->learnInputCursorPosition] = character;
      break;
    }

    case PREAMP_MODE_LEARN_OPTO: {
      int8_t optoInput = preamp->config->optoMain;

      switch(state) {
        case ENCODER_LEFT: {
          optoInput = MAX(0, optoInput - 1);
          break;
        }

        case ENCODER_RIGHT: {
          optoInput = MIN(PREAMP_TOTAL_INPUTS + 1, optoInput + 1);
          break;
        }

        default: break;
      }

      preamp->config->optoMain = optoInput;
      break;
    }

    case PREAMP_MODE_VOLUME_FIXED : {
      uint8_t fixedVolume = preamp->config->fixedVolume[preamp->learnFixedVolume];

      switch(state) {
        case ENCODER_LEFT: {
          if(fixedVolume <= PREAMP_GAIN_MIN)
             fixedVolume = 0xff;
          else if(fixedVolume == 0xff)
            fixedVolume = PREAMP_GAIN_MAX;
          else
            fixedVolume = MAX(PREAMP_GAIN_MIN, fixedVolume - PREAMP_GAIN_STEP);

          break;
        }

        case ENCODER_RIGHT: {
          if(fixedVolume >= PREAMP_GAIN_MAX)
            fixedVolume = 0xff;
          else if(fixedVolume == 0xff)
            fixedVolume = PREAMP_GAIN_MIN;
          else
            fixedVolume = MIN(PREAMP_GAIN_MAX, fixedVolume + PREAMP_GAIN_STEP);

          break;
        }

        default: break;
      }

      preamp->config->fixedVolume[preamp->learnFixedVolume] = fixedVolume;
      break;
    }

    case PREAMP_MODE_VOLUME_INPUT: {
      switch(state) {
        case ENCODER_LEFT: {
          preamp->config->volume[preamp->config->input] = MAX(-PREAMP_GAIN_STEP * PREAMP_INPUT_VOL_RANGE, preamp->config->volume[preamp->config->input] - PREAMP_GAIN_STEP);
          break;
        }

        case ENCODER_RIGHT: {
          preamp->config->volume[preamp->config->input] = MIN(+PREAMP_GAIN_STEP * PREAMP_INPUT_VOL_RANGE, preamp->config->volume[preamp->config->input] + PREAMP_GAIN_STEP);
          break;
        }

        default: break;
      }

      break;
    }

    case PREAMP_MODE_STANDBY: {
      Preamp_wakeup();
      break;
    }

    default: {
      // is volume fixed for this channel?
      BOOL fixed = preamp->config->fixedVolume[preamp->config->input] != 0xff;

      if(!fixed) {
        switch(state) {
          case ENCODER_LEFT: {
            preamp->config->masterVolume = MAX(PREAMP_GAIN_MIN, preamp->config->masterVolume - PREAMP_GAIN_STEP);
            break;
          }

          case ENCODER_RIGHT: {
            preamp->config->masterVolume = MIN(PREAMP_GAIN_MAX, preamp->config->masterVolume + PREAMP_GAIN_STEP);
            break;
          }

          default: break;
        }
      }

      if(preamp->config->mute)
        preamp->config->mute = FALSE;

      if(preamp->attenuation)
        preamp->attenuation = 0;

      break;
    }
  }

  preamp->mustSaveConfig = TRUE;
  preamp->mustUpdateDevice = TRUE;
  preamp->mustUpdateDisplay = TRUE;

  // reset ticks of the update config task
  preamp->updateConfigTask->ticks = 0;
}

void Preamp_onMuteChange(enum Buttonstate_t state)
{
  switch(preamp->mode) {
    case PREAMP_MODE_LEARN_INPUT: {
      if(++preamp->learnInputCursorPosition > PREAMP_INPUT_LENGTH - 2)
        preamp->learnInputCursorPosition = 0;

      break;
    }

    case PREAMP_MODE_VOLUME_FIXED: {
      if(++preamp->learnFixedVolume >= PREAMP_TOTAL_INPUTS)
        preamp->learnFixedVolume = 0;

      break;
    }

    default: {
      preamp->config->mute = !preamp->config->mute;
      break;
    }
  }

  preamp->mustSaveConfig = TRUE;
  preamp->mustUpdateDevice = TRUE;
  preamp->mustUpdateDisplay = TRUE;
}

void Preamp_onModeChange(enum Buttonstate_t state)
{
  if(++preamp->mode > PREAMP_MODE_LAST)
    preamp->mode = PREAMP_MODE_FIRST;

  preamp->learnInputCursorPosition = 0;
  preamp->learnRemoteCode = REMOTECODE_FIRST;
  preamp->mustUpdateDisplay = TRUE;
}

void Preamp_setBacklight(int8_t step)
{
  preamp->config->backlight = MAX(0, MIN(preamp->config->backlight + step, UCHAR_MAX));

  preamp->mustSaveConfig = TRUE;
  preamp->mustUpdateDisplay = TRUE;
}

void Preamp_setInput(int8_t input)
{
  uint8_t previousInput = preamp->config->input;
  preamp->config->input = MAX(0, MIN(PREAMP_TOTAL_INPUTS - 1, input));

  if(previousInput != preamp->config->input) {
    Relays_close(previousInput);
    preamp->mustUpdateRelay = TRUE;
    preamp->mustUpdateDisplay = TRUE;
    preamp->mustSaveConfig = TRUE;
    preamp->mustUpdateDevice = TRUE;
  }

  // reset ticks of the update relay and update config tasks
  preamp->updateConfigTask->ticks = 0;
  preamp->updateRelayTask->ticks = 0;
}

void Preamp_wakeup()
{
  // wake up
  preamp->mode = PREAMP_MODE_NORMAL;

  // save it
  preamp->config->sleep = FALSE;

  // wake AUX opto
  Opto_on(PREAMP_OPTO_AUX);

  // load configuration back
  preamp->mustUpdateDisplay = TRUE;
  preamp->mustUpdateDevice = TRUE;
  preamp->mustUpdateRelay = TRUE;
  preamp->mustSaveConfig = TRUE;
}

void Preamp_sleep()
{
  // standy mode
  preamp->mode = PREAMP_MODE_STANDBY;

  // save it
  preamp->config->sleep = TRUE;

  // shut down input
  Relays_close(preamp->config->input);

  // shut down AUX opto
  Opto_off(PREAMP_OPTO_AUX);

  // update display and device
  preamp->mustUpdateDisplay = TRUE;
  preamp->mustUpdateDevice = TRUE;
  preamp->mustSaveConfig = TRUE;
}
