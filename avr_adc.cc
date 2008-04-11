//======================================================================================
/** \file  avr_adc.h
 *  This file needs a comment which describes what's in here. 
 *
 *  Revisions:
 *    \li  00-00-00  The Big Bang occurred, followed by the invention of waffles
 *    \li  Program is somewhat broke, but base logic is implemented
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

#include "rs232.h"                          // Include header for serial port class
#include "avr_adc.h"                        // Include header for the A/D class


#define ADC_RETRIES      10000              // Retries before giving up on conversion


//-------------------------------------------------------------------------------------
/** This union holds the two bytes which are read from the A/D converter's result
 *  registers after a conversion has been completed. They're referenced also as one
 *  16-bit integer which can be returned as the result of a conversion.
 */

typedef union ADC_result
    {
    int word;                               // The whole 16-bit number
    char bytes[2];                          // The bytes in the number
    };


//-------------------------------------------------------------------------------------
/** This constructor sets up an A/D converter. You'll need to write a comment which 
 *  describes how it does that once you've written the constructor. 
 *  @param p_serial_port A pointer to the serial port which writes debugging info. 
 */

avr_adc::avr_adc (base_text_serial* p_serial_port)
    {
    ptr_to_serial = p_serial_port;          // Store the serial port pointer locally

    // Note that ptr_to_serial is a pointer; the "*" is needed to indicate "the serial
    // port which is pointed to by the pointer" 
    *ptr_to_serial << "Setting up AVR A/D converter" << endl;

    // Initializes A/D converter to channel 0
    ADMUX = 0b00000000;

    // Turns on A/D converter without interrupts or anything, with prescaler set to 16
    ADCSRA = 0b10000100;
    }


//-------------------------------------------------------------------------------------
/** This method takes one A/D reading from the given channel, and...?
 *  \param  channel The A/D channel which is being read must be from 0 to 7
 *  \return The result of the A/D conversion, or 0xFFFF if there was a timeout
 */

unsigned int avr_adc::read_once (unsigned char channel)
    {
    *ptr_to_serial << "Running avr_adc::read_once()" << endl;
    if(channel > 7){
	    *ptr_to_serial << "Channel must be between 0 and 7";
    }
    else{
    ADMUX |= channel;
    }

    // reads from the selected channel, into ADCH and ADCL
    ADCSRA |= 0b01000000;

    int while_counter = 0;
    
    while(ADCSRA & 0b01000000){
	    while_counter++;
    }
    
    *ptr_to_serial << while_counter;
    while_counter = 0;
    ADC_result result;

    result.bytes[0] = ADCL;
    result.bytes[1] = ADCH;

    *ptr_to_serial << hex << "\n\rADCL: " << result.bytes[0] << "\n\rADCH: " << result.bytes[1] << endl;

    return result.word;
    }


//--------------------------------------------------------------------------------------
/** This overloaded operator allows information about or from an A/D converter to be 
 *  printed on a serial device such as a regular serial port or radio module in text 
 *  mode, which is extremely convenient for debugging. 
 *  @param serial A reference to the serial-type object to which to print
 *  @param stamp A reference to the time stamp to be displayed
 */

base_text_serial& operator<< (base_text_serial& serial, avr_adc& my_adc)
    {
    unsigned int channel0, channel1, channel2, channel3;
    channel0 = my_adc.read_once(0);
    channel1 = my_adc.read_once(1);
    channel2 = my_adc.read_once(2);
    channel3 = my_adc.read_once(3);
    serial << "A/D registers of interest: \n\rADMUX: " << ADMUX << "\n\rADCSRA: " << ADCSRA <<
	    "\n\rCurrent value of channels:\n\rChannel 0: " << channel0 << "\n\rChannel 1: " << channel1 <<
	    "\n\rChannel 2: " << channel2 << "\n\rChannel 3: " << channel3 << endl;

    return (serial);
    }
