/* 
 *  Copyright (C) 2006, Brian Crabtree and Joe Lake, monome.org
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
 *  $Id: 40h.c,v. 1.1.1.1 2006/05/02 1:01:22
 */

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
//#include <avr/signal.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "message.h"
#include "adc.h"
#include "button.h"

typedef struct {
	enum port_num { A, B, C, D } port;
	uint8 pin;
} io_pin_t;

inline void output_pin(io_pin_t iopin, bool state)    
{
    if (state) {
        switch (iopin.port) {
        case A:
            PORTA |= (1 << iopin.pin);
            break;
        case B:
            PORTB |= (1 << iopin.pin);
            break;
        case C:
            PORTC |= (1 << iopin.pin);
            break;
        case D:
            PORTD |= (1 << iopin.pin);
            break;
        }
    } else {
        switch (iopin.port) {
        case A:
            PORTA &= ~(1 << iopin.pin);
            break;
        case B:
            PORTB &= ~(1 << iopin.pin);
            break;
        case C:
            PORTC &= ~(1 << iopin.pin);
            break;
        case D:
            PORTD &= ~(1 << iopin.pin);
            break;
        }
    } 
}

bool input_pin(io_pin_t pin)
{
    switch (pin.port) {
    case A:
        return (PINA & (1 << pin.pin));                    
    case B:
        return (PINB & (1 << pin.pin));
    case C:
        return (PINC & (1 << pin.pin));
    case D:
        return (PIND & (1 << pin.pin));
    }

    return 0;
}

// #define DEBUG if you're going to debug over JTAG because we'll get caught in one of those
// while loops otherwise.  I think the JTAG interface must share a pin with the SPI, but I
// haven't confirmed this.  JAL 5/1/2006

#ifndef DEBUG
inline void spi_led(uint8 msb, uint8 lsb)
{
    PORTB &= ~(1 << PB4);
    SPDR = msb;
    while (!(SPSR & (1 << SPIF)));
    SPDR = lsb;
    while (!(SPSR & (1 << SPIF)));
    PORTB |= (1 << PB4);
}
#else
inline void spi_led { ; }
#endif

int main(void)
{
    uint8 i1, i2, i3;
    uint8 rx0, rx1, rx_pos, rx_roll;

    uint8 led_data[8];

    t_message serial_in;
    t_message serial_out;

    io_pin_t ioRXF; ioRXF.port = B; ioRXF.pin = 1;
    io_pin_t ioRD; ioRD.port = B; ioRD.pin = 2;
    io_pin_t ioWR; ioWR.port = B; ioWR.pin = 3;

    io_pin_t ioPWREN; ioPWREN.port = C; ioPWREN.pin = 0;

    io_pin_t ioLOAD; ioLOAD.port = C; ioLOAD.pin = 6;	       // 164 SH/LD
    io_pin_t ioDATA; ioDATA.port = C; ioDATA.pin = 7;	       // 165 QH
    io_pin_t ioCLKSEL; ioCLKSEL.port = A; ioCLKSEL.pin = 6;	   // 164 CLK
    io_pin_t ioCLKIN; ioCLKIN.port = A; ioCLKIN.pin = 7;
    io_pin_t ioSET; ioSET.port = A; ioSET.pin = 5;	           // 164 A

    uint8 firstRun = true;

    DDRB &= ~(1 << ioRXF.pin); PORTB |= (1 << ioRXF.pin);
    DDRB |= (1 << ioRD.pin); PORTB |= (1 << ioRD.pin);
    DDRB |= (1 << ioWR.pin); PORTB |= (1 << ioWR.pin);

    DDRC &= ~(1 << ioPWREN.pin); PORTC |= (1 << ioPWREN.pin);

    DDRC |= (1 << ioLOAD.pin); PORTC |= (1 << ioLOAD.pin);
    DDRC &= ~(1 << ioDATA.pin); PORTC |= (1 << ioDATA.pin);
    DDRA |= (1 << ioCLKSEL.pin); PORTA |= (1 << ioCLKSEL.pin);
    DDRA |= (1 << ioCLKIN.pin); PORTA |= (1 << ioCLKIN.pin);
    DDRA |= (1 << ioSET.pin); PORTA |= (1 << ioSET.pin);

    output_pin(ioSET, 1);	             // clear out row selector
    for (i1 = 0; i1 < 8; i1++) {
        led_data[i1] = 0;
        output_pin(ioCLKSEL, 1);         // clear out row selector
        output_pin(ioCLKSEL, 0);
    }

    buttonInit();

    rx0 = 0;
    rx_pos = 0;
    rx_roll = 0;

    sei();

    // init SPI
    SPCR = (1 << SPE) | (1 << MSTR) | (SPI2X);
    DDRB |= (1 << PB5)|(1 << PB4)|(1 << PB7);
    
    spi_led(11, 7);				       // set scan limit to full range
    spi_led(10, 15);				   // set to max intensity
    for(i1 = 1; i1 < 9; i1++) {
        spi_led(i1, i1);	           // print startup pattern 
    }
    spi_led(12, 1);				       // come out of shutdown mode 
    spi_led(15, 0);				       // test mode off
   
    for(i1=0;i1<64;) {
        spi_led(10, (64-i1)/4);				   // set to max intensity
    	if(!input_pin(ioPWREN)) i1++;         // wait for USB enumeration, to prevent auto-sleep
    	_delay_ms(8);
    }

    for(i1 = 1; i1 < 9; i1++) 
        spi_led(i1, 0);	           // clear led data
 
    spi_led(10, 15);				   // set to max intensity

    // ******** main loop ********
    while (1) {
    	// read incoming serial **********************************************
    	PORTD = 0;			          // setup PORTD for input
        DDRD = 0;			          // input w/ tristate

        while(!(input_pin(ioRXF))) {
            output_pin(ioRD, 0);
            if (rx_pos == 0) {
                rx0 = PIND;
                rx_pos = 1;
            }
            else {
                uint8 msg_type, msg_data0;
                
                rx1 = PIND;
                rx_pos = 0;


                // *********** process packet here 
                serial_in.data0 = rx0;
                serial_in.data1 = rx1;
                
                msg_type = messageGetType(serial_in);

                switch (msg_type) {
                case kMessageTypeLedTest:
                    msg_data0 = messageGetLedTestState(serial_in);
                    spi_led(15, msg_data0);
                    break;

                case kMessageTypeLedIntensity:
                    msg_data0 = messageGetLedIntensity(serial_in);
                    spi_led(10, msg_data0);
                    break;

                case kMessageTypeLedStateChange:

                    i1 = messageGetLedX(serial_in);
                    i2 = messageGetLedY(serial_in);

                    if(messageGetLedState(serial_in) == 0)
                        led_data[i2] &= ~(1 << i1);
                    else
                        led_data[i2] |= (1 << i1);

                    spi_led(i2 + 1, led_data[i2]);
                    break;

                case kMessageTypeAdcEnable:
                    if (messageGetAdcEnableState(serial_in))
                        enableAdc(messageGetAdcEnablePort(serial_in));
                    else
                        disableAdc(messageGetAdcEnablePort(serial_in));
                    break;

                case kMessageTypeShutdown:
                    msg_data0 = messageGetShutdownState(serial_in);
                    spi_led(12, msg_data0);
                    break;

                case kMessageTypeLedSetRow:
                    if (firstRun == true) {
                        for (i1 = 0; i1 < 8; i1++) {
                            led_data[i1] = 0;
                            spi_led(i1 + 1, led_data[i1]);
                        }
                        
                        firstRun = false;
                    }

                    i1 = (messageGetLedRowIndex(serial_in) & 0x7); // mask this value so we don't write to an invalid address.
                                                                   // this will have to change for 100h.
                    i2 = messageGetLedRowState(serial_in);
                    
                    led_data[i1] = i2;
                    spi_led(i1 + 1, led_data[i1]);
                    break;

                case kMessageTypeLedSetColumn:
                    if (firstRun == true) {
                        for (i1 = 0; i1 < 8; i1++) {
                            led_data[i1] = 0;
                            spi_led(i1 + 1, led_data[i1]);
                        }
                        
                        firstRun = false;
                    }

                    i1 = (messageGetLedColumnIndex(serial_in) & 0x7);
                    i2 = messageGetLedColumnState(serial_in);

                    for (i3 = 0; i3 < 8; i3++) {
                        if (i2 & (1 << i3))
                            led_data[i3] |= 1 << i1;
                        else
                            led_data[i3] &= ~(1 << i1);

                        spi_led(i3 + 1, led_data[i3]);
                    }

                    break;
                }
            }

            output_pin(ioRD,1); 
	    
        }

        // check if there's a stray single packet
        if (rx_pos == 1)
            rx_roll++;
        else
            rx_roll = 0; 		// this can be moved to after the packet is processed
	
        // if single packet is "lost" trash it after a chance to get a match
        if(rx_roll > 80) {
            rx_roll = 0;
            rx_pos = 0;
        }

	
        // output serial data **********************************************
        PORTD = 0;			// setup PORTD for output
        DDRD = 0xFF;


        // button check proto

        output_pin(ioSET,0);
        for (i1 = 0; i1 < 8; i1++) {
            output_pin(ioCLKSEL, 1);
            output_pin(ioCLKSEL, 0);
            output_pin(ioSET, 1);
            
	
            button_last[i1] = button_current[i1];

            output_pin(ioLOAD, 0);		// set 165 to load
            output_pin(ioLOAD, 1);		// 165 shift

            for (i2=0; i2 < 8; i2++) {
                i3 = input_pin(ioDATA);
                i3 = (i3 == 0);
                if (i3) 
                    button_current[i1] |= (1 << i2);
                else
                    button_current[i1] &= ~(1 << i2);

                buttonCheck(i1, i2);

                if (button_event[i1] & (1 << i2)) {
                    button_event[i1] &= ~(1 << i2);

                    messagePackButtonPress(&serial_out, (button_state[i1] & (1 << i2)) ? kButtonDownEvent : kButtonUpEvent, i2, i1);
                    
                    output_pin(ioWR, 1);
                    PORTD = serial_out.data0; 
                    output_pin(ioWR, 0);
                    
                    output_pin(ioWR, 1);
                    PORTD = serial_out.data1;
                    output_pin(ioWR, 0); 
                } 
                
                output_pin(ioCLKIN, 1);
                output_pin(ioCLKIN, 0);
            }
        }

        // ---------------------------------- ADC -------------------------


        for (i1 = 0; i1 < kAdcFilterNumAdcs; i1++) { 
            if (gAdcFilters[i1].dirty == true) {
                messagePackAdcVal(&serial_out, i1, gAdcFilters[i1].value);

                output_pin(ioWR, 1);
                PORTD = serial_out.data0; 
                output_pin(ioWR, 0);
                
                output_pin(ioWR, 1);
                PORTD = serial_out.data1;
                output_pin(ioWR, 0); 
                
                gAdcFilters[i1].dirty = false;
            }
        } 
    }
    
    return 0;
}
