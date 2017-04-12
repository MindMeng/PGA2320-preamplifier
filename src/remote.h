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

#ifndef REMOTE_H_
#define REMOTE_H_

#include "pch.h"
#include "const.h"

#define REMOTE_TICK_MILLIS (F_CPU / 64ULL / 1000ULL)
#define REMOTE_BUFFER_SIZE 70
#define REMOTE_TICK_RESOLUTION 16
#define REMOTE_MAX_PROTOCOLS 12

enum RemoteCode_t {
  REMOTECODE_FIRST = 0,

  REMOTECODE_VOLUME_UP = REMOTECODE_FIRST,
  REMOTECODE_VOLUME_DOWN,
  REMOTECODE_MUTE,
  REMOTECODE_BACKLIGHT_UP,
  REMOTECODE_BACKLIGHT_DOWN,
  REMOTECODE_SOURCE_1,
  REMOTECODE_SOURCE_2,
  REMOTECODE_SOURCE_3,
  REMOTECODE_SOURCE_4,
  REMOTECODE_SOURCE_5,
  REMOTECODE_SOURCE_6,
  REMOTECODE_TOGGLE_OPTO_AUX,
  REMOTECODE_STANDBY_MODE,
  REMOTECODE_ATTENUATE,

  REMOTECODE_LAST = REMOTECODE_ATTENUATE,
  REMOTECODE_TOTAL = REMOTECODE_LAST + 1
};

enum RemoteEdge_t {
  REMOTE_EDGE_NONE = 0,
  REMOTE_EDGE_RISING,
  REMOTE_EDGE_FALLING
};

enum RemoteProtocol_t {
  REMOTEPROTOCOL_NEC = 0, // NEC Protocol
  REMOTEPROTOCOL_PNR,     // Pioneer protocol
  REMOTEPROTOCOL_SAT,     // X-Sat/Mitsubishi Protocol
  REMOTEPROTOCOL_RCA,     // RCA Protocol
  REMOTEPROTOCOL_SIR,     // Sony SIRC Protocol
  REMOTEPROTOCOL_RC6,     // Philips RC-6
  REMOTEPROTOCOL_RC5,     // Philips RC-5
  REMOTEPROTOCOL_NRC,     // Nokia NRC17 Protocol
  REMOTEPROTOCOL_VCR,     // Sharp Protocol
  REMOTEPROTOCOL_JVC      // JVC Protocol
};

struct RemoteCode {
  uint8_t command;
  uint8_t address;
  BOOL toggled;
};

struct RemoteSample {
  enum RemoteEdge_t edge;
  uint16_t delay;
};

struct RemoteProtocol {
  enum RemoteProtocol_t protocol;
  uint16_t agc;
  uint16_t jitter;
  BOOL (*parser)(void);
};

struct Remote {
  struct Task* watchdog;
  enum RemoteProtocol_t protocol;

  struct RemoteProtocol parsers[REMOTE_MAX_PROTOCOLS];
  uint8_t protocols;

  struct RemoteSample buffer[REMOTE_BUFFER_SIZE];
  uint8_t samples;
  BOOL sampling;

  struct RemoteCode lastCode;
  struct RemoteCode previousCode;
  BOOL available;

  uint16_t ticks;
};

// Public
void Remote_init(void);
struct Remote* Remote_get(void);
struct RemoteCode* Remote_pop(void);
void Remote_define(enum RemoteProtocol_t protocol, uint16_t agc, uint16_t jitter, BOOL (*parser)(void));

// Private
void Remote_parse(void);
void Remote_watchdog(void);
uint16_t Remote_swap(uint16_t value, uint8_t bits);
struct RemoteSample* Remote_sample(uint8_t sample);
enum RemoteEdge_t Remote_edge(uint8_t edge);
uint16_t Remote_delayToNextEdge(uint8_t edge);
uint16_t Remote_delayBetweenEdges(uint8_t edge, uint8_t limit);
BOOL Remote_check(uint16_t delay, uint16_t expected, uint16_t jitter);

// Parsers
BOOL Remote_parseNEC(void);
BOOL Remote_parsePNR(void);
BOOL Remote_parseSAT(void);
BOOL Remote_parseRCA(void);
BOOL Remote_parseSIR(void);
BOOL Remote_parseRC6(void);
BOOL Remote_parseRC5(void);
BOOL Remote_parseNRC(void);
BOOL Remote_parseVCR(void);
BOOL Remote_parseJVC(void);

#endif
