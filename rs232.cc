//*************************************************************************************
/** \file rs232.h
 *        This file contains functions which allow the use of a serial port on an AVR 
 *        microcontroller. 
 *
 *        This code is designed to work for low-performance applications without 
 *        requiring the use of interrupts.  Interrupt based receiving code has not 
 *        been completed or tested.
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
 *      \li 01-12-08  JRR  Added code for the ATmega128 using USART number 1 only
 *      \li 02-14-08  JRR  Split between base_text_serial and rs232 files
 */
//*************************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "rs232.h"


//-------------------------------------------------------------------------------------
/** This method sets up the AVR UART for communications.  It enables the appropriate 
 *  inputs and outputs and sets the baud rate divisor, and it saves pointers to the
 *  registers which are used to operate the serial port. Since some AVR processors
 *  have dual serial ports, this method allows one to specify a port number. 
 *  @param divisor The baud rate divisor to be used for controlling the rate of
 *      communication 
 *  @param port_number The number of the serial port, 0 or 1 (the second port numbered
 *      1 only exists on some processors). The default is port 0 
 */

rs232::rs232 (unsigned char divisor, unsigned char port_number)
    : base_text_serial ()
    {
    if (port_number == 0)
        {
        #ifdef __AVR_AT90S2313__
            p_UDR = &UDR;
            p_USR = &USR;
            p_UCR = &UCR;
            UCR = 0x18;                     // Mode N81
            UBRR = divisor;
        #endif
        #if defined __AVR_ATmega8__ || defined __AVR_ATmega8535__ \
            || defined __AVR_ATmega32__
            p_UDR = &UDR;
            p_USR = &UCSRA;
            p_UCR = &UCSRB;
            UCSRB = 0x18;
            UCSRC = 0x86;
            UBRRH = 0x00;
            UBRRL = divisor;
        #endif
        #if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__ \
            || defined __AVR_ATmega128__
            p_UDR = &UDR0;
            p_USR = &UCSR0A;
            p_UCR = &UCSR0B;
            UCSR0B = 0x18;
            UCSR0C = 0x86;
            UBRR0H = 0x00;
            UBRR0L = divisor;
        #endif
        }
    else
        {                                   // Some AVR's only have one serial port
        #if defined __AVR_AT90S2313__ || defined __AVR_ATmega8__ \
            || defined __AVR_ATmega8535__ || defined __AVR_ATmega32__
            p_UDR = NULL;
            p_USR = NULL;
            p_UCR = NULL;
        #endif
        #if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__ \
            || defined __AVR_ATmega128__
            p_UDR = &UDR1;
            p_USR = &UCSR1A;
            p_UCR = &UCSR1B;
            UCSR1B = 0x18;
            UCSR1C = 0x86;
            UBRR1H = 0x00;
            UBRR1L = divisor;
        #endif
        }

    // Read the data register to ensure that it's empty
    port_number = *p_UDR;
    port_number = *p_UDR;

//     #ifdef UART_DOUBLE_SPEED                // If double-speed macro has been defined,
//         UART_STATUS |= 0x02;                // Turn on double-speed operation
//     #endif
    }


//-------------------------------------------------------------------------------------
/** This function checks if the serial port transmitter is ready to send data.  It 
 *  tests whether transmitter buffer is empty. 
 *  @return True if the serial port is ready to send, and false if not
 */

bool rs232::ready_to_send (void)
    {
    // If transmitter buffer is full, we're not ready to send
    if (*p_USR & UDRE_MASK)
        return (true);

    return (false);
    }


//-------------------------------------------------------------------------------------
/** This method sends one character to the serial port.  It waits until the port is
 *  ready, so it can hold up the system for a while.  It times out if it waits too 
 *  long to send the character; you can check the return value to see if the character
 *  was successfully sent, or just cross your fingers and ignore the return value.
 *  Note 1:  It's possible that at slower baud rates and/or higher processor speeds, 
 *  this routine might time out even when the port is working fine.  A solution would
 *  be to change the count variable to an integer and use a larger starting number. 
 *  Note 2:  Fixed!  The count is now an integer and it works at lower baud rates.
 *  @param chout The character to be sent out
 *  @return True if everything was OK and false if there was a timeout
 */

bool rs232::putchar (char chout)
    {
    unsigned int count = 0;                 // Timeout counter

    // Now wait for the serial port transmitter buffer to be empty     
    for (count = 0; ((*p_USR & UDRE_MASK) == 0); count++)
        {
        if (count > UART_TX_TOUT)
            return (false);
        }

    // The CTS line is 0 and the transmitter buffer is empty, so send the character
    *p_UDR = chout;
    return (true);
    }


//-------------------------------------------------------------------------------------
/** This method writes all the characters in a string until it gets to the '\\0' at 
 *  the end. Warning: This function blocks until it's finished. 
 *  @param str The string to be written 
 */

void rs232::puts (char const* str)
    {
    while (*str) putchar (*str++);
    }


//-------------------------------------------------------------------------------------
/** This method gets one character from the serial port, if one is there.  If not, it
 *  waits until there is a character available.  This can sometimes take a long time
 *  (even forever), so use this function carefully.  One should almost always use
 *  check_for_char() to ensure that there's data available first. 
 *  @return The character which was found in the serial port receive buffer
 */

char rs232::getchar (void)
    {
    //  Wait until there's something in the receiver buffer
    while ((*p_USR & RXC_MASK) == 0);

    //  Return the character retreived from the buffer
    return (*p_UDR);
    }


//-------------------------------------------------------------------------------------
/** This function checks if there is a character in the serial port's receiver buffer.
 *  It returns 1 if there's a character available, and 0 if not. 
 *  @return True for character available, false for no character available
 */

bool rs232::check_for_char (void)
    {
    if (*p_USR & RXC_MASK)
        return (true);
    else
        return (false);
    }
