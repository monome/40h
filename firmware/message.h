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
 *  $Id: message.h,v. 1.1.1.1 2006/05/02 1:01:22
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define OFF (0)
#define ON  (1)
#define off OFF
#define on  ON


typedef enum {
    kMessageTypeButtonPress = 0,
    kMessageTypeAdcVal,
    kMessageTypeLedStateChange,
    kMessageTypeLedIntensity,
    kMessageTypeLedTest,
    kMessageTypeAdcEnable,
    kMessageTypeShutdown,
    kMessageTypeLedSetRow,
    kMessageTypeLedSetColumn,

    kMessageNumTypes
} eMessageTypes;


typedef struct {
    uint8 data0;
    uint8 data1;
} t_message;

#define messageGetType(message)                (message.data0 >> 4)
#define messageGetButtonState(message)         (message.data0 & 0xF)
#define messageGetButtonX(message)             (message.data1 >> 4)
#define messageGetButtonY(message)             (message.data1 & 0xF)
#define messageGetAdcPort(message)             ((message.data0 >> 2) & 0x3)
#define messageGetAdcVal(message)              ((((uint16)message.data0 & 0x3) << 8) | (((uint16)message.data1)))
#define messageGetLedState(message)            (message.data0 & 0xF)
#define messageGetLedX(message)                (message.data1 >> 4)
#define messageGetLedY(message)                (message.data1 & 0xF)
#define messageGetLedIntensity(message)        (message.data1)
#define messageGetLedTestState(message)        (message.data1 & 0xF)
#define messageGetAdcEnablePort(message)       ((message.data1 & 0xF0) >> 4)
#define messageGetAdcEnableState(message)      (message.data1 & 0xF)
#define messageGetShutdownState(message)       (message.data1 & 0xF)
#define messageGetLedRowIndex(message)         (message.data0 & 0xF)
#define messageGetLedRowState(message)         (message.data1)
#define messageGetLedColumnIndex(message)      (message.data0 & 0xF)
#define messageGetLedColumnState(message)      (message.data1)

void messagePackButtonPress(t_message *message, uint8 state, uint8 x, uint8 y);
void messagePackAdcVal(t_message *message, uint8 port, uint16 val);
void messagePackLedStateChange(t_message *message, uint8 state, uint8 x, uint8 y);
void messagePackLedIntensity(t_message *message, uint8 intensity);
void messagePackLedTest(t_message *message, uint8 state);
void messagePackAdcEnable(t_message *message, uint8 adc, uint8 state);
void messagePackShutdown(t_message *message, uint8 state);
void messagePackLedRow(t_message *message, uint8 rowIndex, uint8 state);
void messagePackLedColumn(t_message *message, uint8 columnIndex, uint8 state);

#ifdef __cplusplus
}
#endif

#endif
