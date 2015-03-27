/* 
* CLed.h
*
* Created: 1/13/2015 10:33:49 PM
* Author: keith
*/


#ifndef __CLEDDRIVER_H__
#define __CLEDDRIVER_H__

#include <avr/io.h>			//I/O
#include <inttypes.h>
#include <avr/pgmspace.h>		//PGM space read

class CLedDriver
{

public:
	static CLedDriver &is()
	{
		static CLedDriver me;
			
		return me;
	}
	
	void On(uint8_t led, bool on);
	void Brightness(uint8_t led, uint8_t brightness);
	void Dim(uint8_t brightness);
	void Blink(uint8_t rate);
	void Init();

protected:
	CLedDriver();
	CLedDriver& operator=( const CLedDriver &c );
	
	void WriteReg(uint8_t LEDReg, uint8_t data);
	
	uint8_t LEDReg0;
	uint8_t LEDReg1;
}; //CLedDriver

#endif //__CLEDDRIVER_H__
