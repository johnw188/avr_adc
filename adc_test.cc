//======================================================================================
/** \file adc_test.cc
 *      This file contains a test program which checks that the A/D converter on the
 *      AVR processor is working correctly. 
 *
 *  Revisions:
 *    \li  00-00-00  The Earth cooled, and then there was breakfast
 *    \li  01-01-00  Confusion
 *    \li  04-10-08  Man writes adc_test.cc
 *    \li  04-14-08  Man completes code/comments. There is much rejoycing
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
                                            // User written headers included with " "
#include "rs232.h"                          // Include header for serial port class
#include "avr_adc.h"                        // Include header for the A/D class

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal on an ATmega8 
 */
#define BAUD_DIV        52                  // For testing an ATmega128


//--------------------------------------------------------------------------------------
/** The main function is the "entry point" of every C program, the one which runs first
 *  (after standard setup code has finished). For mechatronics programs, main() runs an
 *  infinite loop and never exits. 
 */

int main ()
    {
    volatile unsigned long dummy;           // Used as a not-smart delay loop counter
    unsigned int conversion;                // Data from the A/D

    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);

    // Create an ADC (Analog to Digital Converter) object. This object must be given a
    // pointer to the serial port object so that it can print debugging information
    avr_adc my_adc (&the_serial_port);

    // Say hello
    the_serial_port << "\r\nAnalog to Digital Test Program v0.002\r\n";

    // Run the main scheduling loop, in which the action to run is done repeatedly.
    // In the future, we'll run tasks here; for now, just do things in a simple loop
    while (true)
        {
        // The dummy counter is used to slow down the rate at which stuff is printed
        // on the terminal
        if (++dummy >= 1000000L)
            {
            dummy = 0;


	    // Calls the overloaded << operator to print diagnostic information about
	    // the A/D conversion ports
            the_serial_port << "A/D status:\n\r" << my_adc << endl;
            }
        }

    return (0);
    }
