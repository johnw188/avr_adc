//*************************************************************************************
/** \file base_text_serial.cc
 *        This file contains functions which allow the use of a serial port on an AVR 
 *        microcontroller.  Compatibility macros are provided to isolate the names of
 *        various registers from the many specific AVR device types.
 *
 *        This code is designed to work for low-performance applications without
 *        requiring the use of interrupts.  Interrupt based code has not yet been 
 *        completed nor tested.
 *
 *  Revised:
 *      \li 04-03-06  JRR  For updated version of compiler
 *      \li 06-10-06  JRR  Ported from C++ to C for use with some C-only projects; also
 *                         the serial_avr.h header has been stuffed with defines for
 *                         compatibility among lots of AVR variants
 *      \li 08-11-06  JRR  Some bug fixes
 *      \li 03-02-07  JRR  Ported back to C++. I've had it with the limitations of C.
 *      \li 04-16-07  JO   Added write (unsigned long)
 *      \li 07-19-07  JRR  Changed some character return values to bool, added m324p
 *      \li 12-18-07  JRR  Added write (unsigned long long) and CTS flow control
 *      \li 01-12-08  JRR  Added code for the ATmega128 using USART number 1 only
 *      \li 02-13-08  JRR  Split into base class and device specific classes; changed
 *                         from write() to overloaded << operator in the "cout" style
 */
//*************************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "base_text_serial.h"


//-------------------------------------------------------------------------------------
/** This method sets up the base serial port object. It sets the default base for
 *  the conversion of numbers to text. 
 */

base_text_serial::base_text_serial (void)
    {
    base = 10;                              // Numbers are shown as decimal by default
    }


//-------------------------------------------------------------------------------------
/** This function checks if the serial port transmitter is ready to send data.  It's 
 *  an empty base method, overridden by most serial devices. Some serial devices might
 *  always be ready to send data; those can not bother to override this method. 
 *  @return True if the serial port is ready to send, and false if not
 */

bool base_text_serial::ready_to_send (void)
    {
    return (true);                          // By default the port's always ready
    }


//-------------------------------------------------------------------------------------
/** This base method just returns zero, because it shouldn't be called. There might be
 *  classes which only send characters and don't ever receive them, and this method
 *  could be left as a placeholder for those classes. 
 *  @return A zero (null character) which means nothing
 */

char base_text_serial::getchar (void)
    {
    return ('\0');                          // Nothing to return, really
    }


//-------------------------------------------------------------------------------------
/** This method checks if there is a character in the serial port's receiver buffer.
 *  There isn't, as this base class isn't connected to a buffer; descendent classes
 *  will override this method and check a real buffer for real characters. 
 *  @return False, because no character will ever be available
 */

bool base_text_serial::check_for_char (void)
    {
    return (false);
    }


//-------------------------------------------------------------------------------------
/** This is a base method for causing immediate transmission of a buffer full of data.
 *  The base method doesn't do anything, because it will be implemented in descendent
 *  classes which have no buffers, send everything immediately by default, and don't 
 *  need to respond to calls for immediate transmission. 
 */

void base_text_serial::transmit_now (void)
    {
    }


//-------------------------------------------------------------------------------------
/** This method writes the string whose first character is pointed to by the given
 *  character pointer to the serial device. It acts in about the same way as puts(). 
 *  As with puts(), the string must have a null character (ASCII zero) at the end. 
 *  @param string Pointer to the string to be written
 */

base_text_serial& base_text_serial::operator<< (const char* string)
    {
    while (*string) putchar (*string++);

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This method writes a boolean value to the serial port as a character, either "T"
 *  or "F" depending on the value. 
 *  @param value The boolean value to be written
 */

base_text_serial& base_text_serial::operator<< (bool value)
    {
    if (value)
        putchar ('T');
    else
        putchar ('F');

    return (*this);;   
    }


//-------------------------------------------------------------------------------------
/** This method writes a character to the serial port as a text string showing the 
 *  8-bit unsigned number in that character. If one needs to send a character directly
 *  without converting it as a number to a string, then one should use the putchar() 
 *  method instead. 
 *  @param num The 8-bit number to be sent out
 */

base_text_serial& base_text_serial::operator<< (unsigned char num)
    {
    if (base == 2)
        {
        for (unsigned char bmask = 0x80; bmask != 0; bmask >>= 1)
            {
            if (num & bmask) putchar ('1');
            else             putchar ('0');
            }
        }
    else
        {
        char out_str[9];

        utoa ((unsigned int)num, out_str, base);
        puts (out_str);
        }

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This method writes a character to the serial port as a text string showing the 
 *  8-bit signed number in that character. If one needs to send a character directly
 *  without converting it as a number to a string, then one should use the putchar() 
 *  method instead. 
 *  @param num The 8-bit number to be sent out
 */

base_text_serial& base_text_serial::operator<< (char num)
    {
    if (base == 2)
        {
        for (unsigned char bmask = 0x80; bmask != 0; bmask >>= 1)
            {
            if (num & bmask) putchar ('1');
            else             putchar ('0');
            }
        }
    else
        {
        char out_str[10];

        utoa ((unsigned int)num, out_str, base);
        puts (out_str);
        }

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This method writes an integer to the serial port as a text string showing the 
 *  16-bit unsigned number in that integer. 
 *  @param num The 16-bit number to be sent out
 */

base_text_serial& base_text_serial::operator<< (unsigned int num)
    {
    if (base == 2)
        {
        for (unsigned int bmask = 0x8000; bmask != 0; bmask >>= 1)
            {
            if (num & bmask) putchar ('1');
            else             putchar ('0');
            }
        }
    else
        {
        char out_str[17];

        utoa (num, out_str, base);
        puts (out_str);
        }

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This method writes an integer to the serial port as a text string showing the 
 *  16-bit signed number in that integer. 
 *  @param num The 16-bit number to be sent out
 */

base_text_serial& base_text_serial::operator<< (int num)
    {
    if (base == 2)
        {
        for (unsigned int bmask = 0x8000; bmask != 0; bmask >>= 1)
            {
            if (num & bmask) putchar ('1');
            else             putchar ('0');
            }
        }
    else
        {
        char out_str[17];

        utoa (num, out_str, base);
        puts (out_str);
        }

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This method writes an unsigned long integer to the serial port as a text string 
 *  showing the 32-bit unsigned number in that long integer. 
 *  @param num The 32-bit number to be sent out
 */

base_text_serial& base_text_serial::operator<< (unsigned long num)
    {
    if (base == 2)
        {
        union {
            unsigned long whole;
            unsigned char bits[4];
            } parts;
        parts.whole = num;
        *this << parts.bits[3] << parts.bits[2] << parts.bits[1] << parts.bits[0];
        }
    else
        {
        char out_str[33];
        ultoa ((long)num, out_str, base);
        puts (out_str);
        }

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This method writes a long integer to the serial port as a text string showing the 
 *  32-bit signed number in that long integer. 
 *  @param num The 32-bit number to be sent out
 */

base_text_serial& base_text_serial::operator<< (long num)
    {
    if (base == 2)
        {
        union {
            unsigned long whole;
            unsigned char bits[4];
            } parts;
        parts.whole = num;
        *this << parts.bits[3] << parts.bits[2] << parts.bits[1] << parts.bits[0];
        }
    else
        {
        char out_str[34];
        ltoa ((long)num, out_str, base);
        puts (out_str);
        }

    return (*this);
    }


//-------------------------------------------------------------------------------------
/** This overload allows manipulators to be used to change the base of displayed 
 *  numbers to binary, octal, decimal, or hexadecimal. Also, and endline is provided
 *  with the name "endl" and the code "send_now" causes immediate transmission by
 *  devices which save stuff to be transmitted in buffers. 
 *  @param new_base The new base in which numbers will be displayed
 */

base_text_serial& base_text_serial::operator<< (ser_manipulator new_base)
    {
    switch (new_base)
        {
        case (bin):
            base = 2;
            break;
        case (oct):
            base = 8;
            break;
        case (dec):
            base = 10;
            break;
        case (hex):
            base = 16;
            break;
        case (endl):
            puts ("\r\n");
            break;
        case (send_now):
            transmit_now ();
            break;
        };

    return (*this);
    }
