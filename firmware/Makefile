CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega32
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref	
OBJ2HEX=avr-objcopy 


####### Files:

SOURCES       = 40h.c \
		adc.c \
		message.c \
		button.c

OBJECTS	      = 40h.o \
		adc.o \
		message.o \
		button.o

TARGET=40h


####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C


.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

%.elf:	$(OBJECTS)


####### Build rules

$(TARGET):	$(OBJECTS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
		$(CC) $(ALL_CFLAGS) $(OBJECTS) --output $(TARGET).elf $(LDFLAGS)	
		$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex
program: $(TARGET).hex
	avarice --erase --program --file $(TARGET).hex --jtag /dev/tty.usbserial-1B1 --jtag-bitrate 500KHz

debug: $(TARGET).hex
	avarice --jtag /dev/tty.usbserial-1B1 --jtag-bitrate 500KHz localhost:8086

erase:
	avarice --erase --jtag /dev/tty.usbserial-1B1 --jtag-bitrate 500KHz

fuses:
	avarice --write-fuses ee00ee --jtag /dev/tty.usbserial-1B1 --jtag-bitrate 500KHz

pro: $(TARGET).hex
	avarice --erase --program --file $(TARGET).hex --jtag /dev/tty.usbserial-1B1 --jtag-bitrate 500KHz --write-fuses ee00ee


####### Compile

40h.o:		40h.c
message.o:      message.c
adc.o: 		adc.c
button.o:	button.c
