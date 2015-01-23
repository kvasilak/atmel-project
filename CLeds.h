/* 
* CLeds.h
*
* Created: 1/22/2015 10:44:30 PM
* Author: keith
*/


#ifndef __CLEDS_H__
#define __CLEDS_H__

#include <avr/io.h>			//I/O
#include <inttypes.h>
#include <avr/pgmspace.h>		//PGM space read
#include "CLedDriver.h"

class CLeds :public CLedDriver
{
public:
	//Make me a Singelton
	static CLeds &is()
	{
		static CLeds led;
		return led;
	}
	CLeds();

	
	void Init();
	
	//LEDOUT0
	//LED0
	void RightUpOn();
	void RightUpOff();
	
	//LED1
	void RightDownOn();
	void RightDownOff();
	
	//LED2
	void ActiveOn();
	void ActiveOff();
	
	//LED3
	void LeftUpOn();
	void LeftUpOff();
	
	
	
	//LEDOUT1
	//LED0
	void LeftDownOn();
	void LeftDownOff();
	
	//LED1
	void CampOn();
	void CampOff();
	
	//LED2
	void TravelWarnOn();
	void TravelWarnOff();
	
	//LED3
	void TravelOKOn();
	void TravelOKOff();
	
	CLeds& operator=( const CLeds &c );
	
	private:
	uint8_t LEDout0;
	uint8_t LEDout1;

};

#endif //__CLEDS_H__
