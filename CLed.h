/* 
* CLed.h
*
* Created: 1/13/2015 10:33:49 PM
* Author: keith
*/


#ifndef __CLED_H__
#define __CLED_H__

#include <avr/io.h>			//I/O
#include <inttypes.h>
#include <avr/pgmspace.h>		//PGM space read

class CLed
{

//functions
public:
	static CLed &is()
	{
		static CLed led;
			
		return led;
	}
	
	void Init();
	
	void RightUpOn();
	void RightUpOff();
	void LeftUpOn();
	void LeftUpOff();
	void RightDownOn();
	void RightDownOff();
	void LeftDownOn();
	void LeftDownOff();
	
	void GlobalBrightness(uint8_t Brightness);
	
	void Set(bool RU,bool RD, bool Active, bool LU, bool LD, bool Camp, bool TravelWarn, bool TravelOK, uint8_t brightness);
	void Set1(bool RU,bool RD, bool Active, bool LU, bool LD, bool Camp, bool TravelWarn, bool TravelOK, uint8_t brightness);
	
protected:
private:
	CLed();
	
	//CLed( const CLed &c );
	CLed& operator=( const CLed &c );
	
	void WriteReg(uint8_t LEDReg, uint8_t data);
	
	uint8_t LEDReg1;
	uint8_t WhatsOn;
	

}; //CLed

#endif //__CLED_H__
