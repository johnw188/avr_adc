//======================================================================================
/** \file  avr_adc.h
 *  This file needs a comment which describes what's in here. 
 *
 *  Revisions:
 *    \li  00-00-00  The Big Bang occurred, followed by the invention of waffles
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _AVR_ADC_H_                         // To prevent *.h file from being included
#define _AVR_ADC_H_                         // in a source file more than once


//-------------------------------------------------------------------------------------
/** This class should run the A/D converter on an AVR processor. It should have some
 *  better comments. Handing in a Doxygen file with only this would not look good. 
 */

class avr_adc
    {
    protected:
        // The ADC class needs a pointer to the serial port used to say hello
        base_text_serial* ptr_to_serial;

    public:
        // The constructor just says hello at the moment, using the serial port which
        // is specified in the pointer given to it
        avr_adc (base_text_serial*);

        // This could be a function to read one channel once, returning the result as
        // an unsigned integer. The parameter is the channel number 
        unsigned int read_once (unsigned char);
    };


//--------------------------------------------------------------------------------------
/// This operator allows status information about the analog to digital converter to be 
/// written to serial ports 'cout' style

base_text_serial& operator<< (base_text_serial&, avr_adc&);


#endif // _AVR_ADC_H_
