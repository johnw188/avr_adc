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

/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _RS232_H_
#define _RS232_H_

#include "base_text_serial.h"               // Pull in the base class header file


//-------------------------------------------------------------------------------------
// This section contains macros to define bitmasks which have different names on 
// different processor versions. 

#ifdef __AVR_AT90S2313__                    // For the AT90S2313 processor
    #define UDRE_MASK (1 << UDRE)           // Mask for transmitter empty
    #define RXC_MASK (1 << RXC)             // Mask for receive complete
#endif
#ifdef __AVR_ATmega8__                      // For the ATMega8 processor
    #define UDRE_MASK (1 << UDRE)           // Mask for transmitter empty
    #define RXC_MASK (1 << RXC)             // Mask for receive complete
#endif
#ifdef __AVR_ATmega8535__                   // For the old ATMega8535 processor
    #define UDRE_MASK (1 << UDRE)           // Mask for transmitter empty
    #define RXC_MASK (1 << RXC)             // Mask for receive complete
#endif // __AVR_ATmega8535__
#ifdef __AVR_ATmega32__                     // For the ATMega32 processor
    #define UDRE_MASK (1 << UDRE)           // Mask for transmitter empty
    #define RXC_MASK (1 << RXC)             // Mask for receive complete
#endif // __AVR_ATmega32__
#if (defined __AVR_ATmega644__ || defined __AVR_ATmega324P__)
    #define UDRE_MASK (1 << UDRE0)          // Mask for transmitter empty
    #define RXC_MASK (1 << RXC0)            // Mask for receive complete
#endif // ...324 or 644...
#ifdef __AVR_ATmega128__                    // For the big ATMega128 processor
    #define UDRE_MASK (1 << UDRE0)          // Mask for transmitter empty
    #define RXC_MASK (1 << RXC0)            // Mask for receive complete
#endif // __AVR_ATmega128__


/** The number of tries to wait for the transmitter buffer to become empty */
#define UART_TX_TOUT        20000


//-------------------------------------------------------------------------------------
/** This class controls a UART (Universal Asynchronous Receiver Transmitter), a common 
 *  serial interface. It talks to old-style RS232 serial ports (through a voltage
 *  converter chip such as a MAX232) or through a USB to serial converter such as a
 *  FT232RL chip. The UART is also sometimes used to communicate directly with other
 *  microcontrollers, sensors, or wireless modems. 
 */

class rs232 : public base_text_serial
    {
    // Private data and methods are accessible only from within this class and 
    // cannot be accessed from outside -- even from descendents of this class
    private:

    // Protected data and methods are accessible from this class and its descendents
    // only
    protected:
        /// This is a pointer to the data register used by the UART
        volatile unsigned char* p_UDR;

        /// This is a pointer to the status register used by the UART
        volatile unsigned char* p_USR;

        /// This is a pointer to the control register used by the UART
        volatile unsigned char* p_UCR;

    // Public methods can be called from anywhere in the program where there is a 
    // pointer or reference to an object of this class
    public:
        // The constructor sets up the UART, saving its baud divisor and location
        rs232 (unsigned char, unsigned char = 0);
        bool ready_to_send (void);          // Check if the port is ready to transmit
        bool putchar (char);                // Write one character to serial port
        void puts (char const*);            // Write a string constant to serial port
        bool check_for_char (void);         // Check if a character is in the buffer
        char getchar (void);                // Get a character; wait if none is ready
    };

#endif  // _RS232_H_
