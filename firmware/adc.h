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
 *  $Id: adc.h,v. 1.1.1.1 2006/05/02 1:01:22
 */

#ifndef __ADC_H__
#define __ADC_H__

#include "types.h"

#define kAdcFilterNumAdcs         4
#define kAdcFilterNumBuckets      16
#define kAdcFilterRightShiftValue 4

typedef struct _adc_filter {
    uint16 bucket[kAdcFilterNumBuckets];  // ring buffer for storing adc frames
    uint16 accum;                         // sum of all adc frames in bucket
    uint8  index;                         // write index into bucket
    uint16 value;                         // average of all values in bucket
    uint16 last_value;                    // last value (dirty only gets set if value != last_value)
    uint8  dirty;                         // flag to indicate whether or not the adc value has changed and should be output
} t_adc_filter;

void initAdcs(void);   
void disableAdcs(void);
void hardInitAdcs(void);
void initAdcFilters(void);
void adcAddNextValue(uint8 adc, uint16 value);
void enableAdc(uint8 adc);
void disableAdc(uint8 adc);


extern t_adc_filter gAdcFilters[kAdcFilterNumAdcs];
extern uint8 gAdcEnableState;

#endif
