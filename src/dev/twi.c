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

#include "twi.h"

uint8_t TWI_poll(uint8_t u)
{
    TWCR = u | _BV(TWINT) | _BV(TWEN);

    if(u == _BV(TWSTO))
        return 0;

    while(!(TWCR & _BV(TWINT)));
    return TWSR & 0xf8;
}

void TWI_init(void)
{
    TWBR = (uint8_t)(F_CPU / TWI_FREQ - 16) / TWI_PRE / 2;
    TWSR = (0 << TWPS0);

    DDRC &= ~(_BV(SCL_BIT) | _BV(SDA_BIT));
    SCL_PORT |= _BV(SCL_BIT);
    SDA_PORT |= _BV(SDA_BIT);
}

uint8_t TWI_start(uint8_t address)
{
    int u = TWI_poll(_BV(TWSTA));
    if(u != TW_START && u != TW_REP_START)
        return 0;

    TWDR = address | TW_WRITE;
    if (TWI_poll(0) == TW_MT_SLA_ACK)
        return 1;

    return 0;
}

uint8_t TWI_stop(void)
{
    return TWI_poll(_BV(TWSTO));
}

uint8_t TWI_transmit(uint8_t cmd)
{
    TWDR = cmd;
    return TWI_poll(0);
}

uint8_t TWI_receive(uint8_t* data, uint8_t ack)
{
    if(ack && TWI_poll(_BV(TWEA)) != TW_MR_DATA_ACK)
        return 0;
    else if(TWI_poll(0) != TW_MR_DATA_NACK)
        return 0;

    *data = TWDR;
    return 1;
}
