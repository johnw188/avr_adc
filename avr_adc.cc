//======================================================================================
/** \file  avr_adc.h
 *  This file needs a comment which describes what's in here. 
 *
 *  Revisions:
 *    \li  00-00-00  The Big Bang occurred, followed by the invention of waffles
 *    \li  04-10-08  Code is finished, except that it doesn't work
 *    \li  04-14-08  Implemented new "non-broken" functionality
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

/** These defines make it easier for us to manipulate the bits of our registers, by
 * creating two new commands - cbi for clear bit i and sbi for set bit i
 */

#define BV(bit) (1<<(bit)) // Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) // Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  // Sets the corresponding bit in register reg


//-------------------------------------------------------------------------------------
/** This union holds two bytes, making them accessable as both a two char array and
 *  a single 16 bit word
 */

typedef union ADC_result
{
	int word;                               // The whole 16-bit number
	char bytes[2];                          // The bytes in the number
};


//-------------------------------------------------------------------------------------
/** This constructor sets up an A/D converter. It does so by storing a pointer to
 *  the serial port that the user wishes to view data on in ptr_to_serial, and then
 *  
 */

avr_adc::avr_adc (base_text_serial* p_serial_port)
{
	ptr_to_serial = p_serial_port;          // Store the serial port pointer locally

	// Note that ptr_to_serial is a pointer; the "*" is needed to indicate "the serial
	// port which is pointed to by the pointer" 
	*ptr_to_serial << "Setting up AVR A/D converter" << endl;

	// Turns on A/D converter without interrupts and in single sample mode,
	// with prescaler set to 64
	ADCSRA = 0b10000110;

	// Sets ADC result to right adjust, selects AVCChannel as Vref, and selects
	// single-ended conversion on PF0
	ADMUX = 0b01000000;
}


//-------------------------------------------------------------------------------------
/** This method takes one A/D reading from the given channel, and returns it as a
 *  16 bit value.
 *  \param  channel The A/D channel which is being read must be from 0 to 7
 *  \return The result of the A/D conversion, or 0xFFFF if there was a timeout
 */

unsigned int avr_adc::read_once (unsigned char channel)
{
	ADMUX = ((ADMUX & 0b11100000) | channel);
	sbi(ADCSRA,ADSC); // start a conversion by writing a one to the ADSC bit (bit 6)

	ADC_result result;

	while(ADCSRA & 0b01000000); // wait for conversion to complete (bit 6 will change to 0)
	result.bytes[0] = ADCL;
	result.bytes[1] = ADCH;

	return result.word;
}

//--------------------------------------------------------------------------------------
/** This overloaded operator allows information about or from an A/D converter to be 
 *  printed on a serial device such as a regular serial port or radio module in text 
 *  mode, which is extremely convenient for debugging. It outputs a list of all available
 *  channels along with their respective voltages, in millivolts.
 *  @param serial A reference to the serial-type object to which to print
 *  @param stamp A reference to the time stamp to be displayed
 */

base_text_serial& operator<< (base_text_serial& serial, avr_adc& my_adc)
{
	unsigned int channel0, channel1, channel2, channel3;
	unsigned long vchannel0, vchannel1, vchannel2, vchannel3;

	// Gets values for all the available channels
	channel0 = my_adc.read_once(0);
	channel1 = my_adc.read_once(1);
	channel2 = my_adc.read_once(2);
	channel3 = my_adc.read_once(3);

	// Converts to millivolts
	vchannel0 = (unsigned long)channel0 * 5000 / 1024;
	vchannel1 = (unsigned long)channel1 * 5000 / 1024;
	vchannel2 = (unsigned long)channel2 * 5000 / 1024;
	vchannel3 = (unsigned long)channel3 * 5000 / 1024;


	// Outputs to the serial port
	serial  << 	"A/D registers of interest:" << endl << 
		"ADMUX: " << ADMUX << endl << 
		"ADCSRA: " << ADCSRA << endl << 
		"Current value of channels:" << endl <<
		"Channel 0: " << channel0 << "   in MilliVolt: " << vchannel0 << endl <<
		"Channel 1: " << channel1 << "   in MilliVolt: " << vchannel1 << endl <<
		"Channel 2: " << channel2 << "   in MilliVolt: " << vchannel2 << endl << 
		"Channel 3: " << channel3 << "   in MilliVolt: " << vchannel3 << endl << endl << endl;

	return (serial);
}
