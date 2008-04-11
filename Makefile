#-----------------------------------------------------------------------------
# File:    Makefile for an AVR project.  
#
# Version:  4-11-2004  JRR  Original file
#           6-19-2006  JRR  Modified to use AVR-JTAG-ICE for debugging
#
# Relies   The avr-gcc compiler and avr-libc library
# on:      The avrdude downloader, if downloading without debugging
#          AVR-Insight and avarice, if debugging with the JTAG port
#          Doxygen, for automatic documentation generation
#
# Copyright 2006-2007 by JR Ridgely.  This makefile is intended for use in
# educational courses only, but its use is not restricted thereto. It is 
# released under the terms of the Lesser GNU Public License with no warranty 
# whatsoever. Anyone who uses this file agrees to take full responsibility
# for any and all consequences of that use. 
#-----------------------------------------------------------------------------
# Stuff the user is expected to change is in this section

# The name of the program you're building, and the list of object files
TARGET = adc_test
OBJS = $(TARGET).o base_text_serial.o rs232.o avr_adc.o

# This specifies the type of CPU; both 'CHIP' and 'MCU' must be set
#CHIP = 2313
#MCU = at90s2313
#CHIP = m8535
#MCU = atmega8535
#CHIP = m8
#MCU = atmega8
#CHIP = m32
#MCU = atmega32
#CHIP = m324p
#MCU = atmega324p
#CHIP = m644
#MCU = atmega644
CHIP=m128
MCU=atmega128

# Port to which downloader cable is attached, and type of downloader
PORT = /dev/parport0             # Port used by avrdude downloader program
HWARE = bsd                      # Type of cable ('bsd' is parport cable)
JPORT = /dev/ttyUSB0             # Port to which JTAG-ICE is connected
JHWARE = jtagice                 # Type of JTAG-ICE interface
IPPORT = :4242                   # IP port on which Avarice and Insight talk

# These codes are used to switch on debugging modes if they're being used:
# -DSTL_DEBUG_SERIAL_PORT   For general debugging through a serial line
# -DSTL_DEBUG_9XSTREAM      For general debugging over a 9XStream radio modem
# -DAOWI_DEBUG_9XSTREAM	    For debugging 1-wire interface with a 9XStream
# DSTL_TRACE_9XSTREAM       For state transition tracing over a 9XStream
DEBUG_CODES = 

# End of stuff which the user is expected to change
#-----------------------------------------------------------------------------

# Definitions of which compiler to use, which options, etc. 
CC = avr-gcc                     # Name of compiler program
STD = _GNU_SOURCE                # Standard compiler options
HDRS =                           # Not used
DEBUGL = DEBUG_LEVEL=0           # Option for debugging level
OPTIM = -O2                      # Optimization level for compiler (0, n, s)
LINKMODE = -g                    # Linker mode string
AVARICE = avarice                # Name of JTAG interface program
DEBUGPROG = /usr/avr/bin/avr-insight   # Name of debugger, avr-gdb or avr-insight
DBCMFL = gdb.commands            # File containing debugger startup commands

#-----------------------------------------------------------------------------
# Inference rules show how to process each kind of file.

# How to compile a .c file into a .o file
.c.o:
	$(CC) -c -g $(OPTIM) -mmcu=$(MCU) -D$(MCU) $(DEBUG_CODES) $<

# How to compile a .cc file into a .o file
.cc.o:
	$(CC) -c -g $(OPTIM) -mmcu=$(MCU) -D$(MCU) $(DEBUG_CODES) $<

#-----------------------------------------------------------------------------
# Make the main file of this project.  This target is invoked when the user
# just types 'make' as opposed to 'make <something>.'  This should be the
# first target in the makefile.

all:  $(TARGET).lst

#-----------------------------------------------------------------------------
# How to make a listing file from the .elf file
$(TARGET).lst: $(TARGET).elf
	avr-objdump -h -S $(TARGET).elf > $(TARGET).lst

#-----------------------------------------------------------------------------
# How to update the copy of the program which has been installed by JTAG.
# The TARGET.u2d file is used to determine if there is a version on the PC
# which has been compiled more recently than the last download.
$(TARGET).u2d: $(TARGET).lst
	$(AVARICE) -e -p -f $(TARGET).elf -j $(JPORT); touch $(TARGET).u2d

#-----------------------------------------------------------------------------
# This rule controls the building of the target program from source files.

$(TARGET).elf:  $(OBJS)
	$(CC) $(OBJS) $(LINKMODE) -mmcu=$(MCU) -o $(TARGET).elf
	avr-objdump -h -S $(TARGET).elf > $(TARGET).lst
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex

#-----------------------------------------------------------------------------
# 'make install' will make the project, then download the program through a 
# downloader cable without running the debugger.  This can be used with cheap
# versions of the processor, such as the ATtinyXX chips.

install:  $(TARGET).elf
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset \
        -Uflash:w:$(TARGET).hex

#-----------------------------------------------------------------------------
# 'make xtal' will set the oscillator to run off the crystal rather than the
# internal oscillator, which is just kind of lame

xtal:
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset \
		-Ulfuse:w:0xEF:m 

#-----------------------------------------------------------------------------
# 'make mega128' will set up an ATmega128 in non-103 mode, crystal osc., with
# the JTAG interface enabled

mega128:
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset -Uefuse:w:0xFF:m 
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset -Uhfuse:w:0x99:m 
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset -Ulfuse:w:0xEF:m 

#-----------------------------------------------------------------------------
# 'make verify' will check if the program in the chip matches that which has
#  been most recently compiled

verify:
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset \
        -Uflash:v:$(TARGET).hex 

#-----------------------------------------------------------------------------
# 'make terminal' will just open up an avrdude terminal
terminal:
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset -t 

#-----------------------------------------------------------------------------
# 'make reset' will read a byte of lock bits, ignore it, and reset the chip
# This can be used to un-freeze the chip after a 'make freeze'
reset:
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E noreset \
		-Ulfuse:r:/dev/null:r

#-----------------------------------------------------------------------------
# 'make freeze' will read a byte of lock bits and leave the chip in reset
freeze:
	avrdude -p $(CHIP) -P $(PORT) -c $(HWARE) -V -E reset \
		-Ulfuse:r:/dev/null:r

#-----------------------------------------------------------------------------
# 'make debug' will make the project, then download it to the processor using
# the JTAG interface and open up a debugging session.

debug:  $(TARGET).u2d
	sh -c '$(AVARICE) -e -p -f $(TARGET).elf -j $(JPORT) $(IPPORT) &'
	sleep 1
	$(INSIGHT) $(TARGET).elf

#-----------------------------------------------------------------------------
# 'make doc' will use Doxygen to create documentation for the project.

doc:  $(TARGET).elf
	doxygen doxygen.conf

#-----------------------------------------------------------------------------
# 'make run' will make the project, download it using the JTAG interface, and
# start it running.  No debugging will be done. 

run:  $(TARGET).elf
	$(AVARICE) -e -p -f $(TARGET).elf -j $(JPORT)

#-----------------------------------------------------------------------------
# 'make avarice' will run the JTAG interface program only.  Use it when you
# already have an avr-insight window open and want to download a newly
# compiled version of your program, then debug it using that window.  This 
# verifies the copy of the program in the chip, and if it's good, runs the 
# avarice program in the background, waiting for avr-insight to talk to it. 

avarice:  $(TARGET).u2d
	$(AVARICE) --capture -B 500000 -j $(JPORT) $(IPPORT)

#-----------------------------------------------------------------------------
# 'make insight' will start the avr-insight graphical debugger.  It should be
# called after using 'make avarice' to start the JTAG interface program. 

insight:  $(TARGET).elf
	dchroot -c ia32 -d \
	  'export DISPLAY=:0.1; $(DEBUGPROG) --command=$(DBCMFL) $(TARGET).elf &'

#-----------------------------------------------------------------------------
# 'make clean' will erase the compiled files, listing files, etc. so you can
# restart the building process from a clean slate.

clean:
	rm -f *.o $(TARGET).hex $(TARGET).lst $(TARGET).elf $(TARGET).u2d
	rm -fr html

#-----------------------------------------------------------------------------
# 'make help' will show a list of things this makefile can do

help:
	@echo 'make          - Build program file ready to download'
	@echo 'make install  - Build program and download with parallel ISP cable'
	@echo 'make run      - Build program and download with JTAG-ICE module'
	@echo 'make doc      - Generate documentation with Doxygen'
	@echo 'make clean    - Remove compiled files; use before archiving files'
	@echo 'make verify   - Check program on chip is up to date with parallel cable'
	@echo 'make freeze   - Stop processor with parallel cable RESET line'
	@echo 'make reset    - Reset processor with parallel cable RESET line'
	@echo '                (Other less used targets are in the Makefile)'
