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

class CLedDriver
{

//functions
public:
	static CLedDriver &is()
	{
		static CLedDriver led;
			
		return led;
	}
	
	void Init();
	
	void On(uint8_t led, bool on);
	void Brightness(uint8_t led, uint8_t brightness);
	void Dim(uint8_t brightness);
	void Blink(uint8_t rate);
	
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
	CLedDriver();
	
	//CLed( const CLed &c );
	CLedDriver& operator=( const CLedDriver &c );
	
	void WriteReg(uint8_t LEDReg, uint8_t data);
	
	uint8_t LEDReg1;
	uint8_t WhatsOn;
	

}; //CLed

#endif //__CLED_H__
