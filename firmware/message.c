/* 
 *  Copyright (C) 2006, Joe Lake, monome.org
 * 
 *  This file is part of 40h.
 *
 *  40h is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  40h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with 40h; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  $Id: message.c,v. 1.1.1.1 2006/05/02 1:01:22
 */

#include "message.h"

void messagePackButtonPress(t_message *message, uint8 state, uint8 x, uint8 y)
{
    message->data0 = (kMessageTypeButtonPress << 4) | (state ? on : off);
    message->data1 = (x << 4) | y;
}

void messagePackAdcVal(t_message *message, uint8 port, uint16 val)
{
    message->data0 = (kMessageTypeAdcVal << 4) | ((port << 2) & 0x0C) | ((val >> 8) & 0x03);
    message->data1 = (uint8)val;
}

void messagePackLedStateChange(t_message *message, uint8 state, uint8 x, uint8 y)
{
    message->data0 = (kMessageTypeLedStateChange << 4) | state; 
    message->data1 = (x << 4) | y;
}

void messagePackLedIntensity(t_message *message, uint8 intensity)
{
    message->data0 = (kMessageTypeLedIntensity << 4);
    message->data1 = intensity;
}

void messagePackLedTest(t_message *message, uint8 state)
{
    message->data0 = (kMessageTypeLedTest << 4);
    message->data1 = state ? on : off;
}

void messagePackAdcEnable(t_message *message, uint8 adc, uint8 state)
{
    message->data0 = kMessageTypeAdcEnable << 4;
    message->data1 = (adc << 4) | (state ? on : off);
}

void messagePackShutdown(t_message *message, uint8 state)
{
    message->data0 = kMessageTypeShutdown << 4;
    message->data1 = state ? on : off;
}

void messagePackLedRow(t_message *message, uint8 rowIndex, uint8 state)
{
    message->data0 = (kMessageTypeLedSetRow << 4) | (rowIndex & 0xF);
    message->data1 = state;
}

void messagePackLedCol(t_message *message, uint8 columnIndex, uint8 state)
{
    message->data0 = (kMessageTypeLedSetColumn << 4) | (columnIndex & 0xF);
    message->data1 = state;
}
