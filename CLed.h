/* 
* CLed.h
*
* Created: 1/13/2015 10:33:49 PM
* Author: keith
*/


#ifndef __CLED_H__
#define __CLED_H__

#include <inttypes.h>

class CLed
{
//variables
public:
protected:
private:

//functions
public:
	CLed();
	~CLed();
	void Set(bool RU,bool RD, bool Active, bool LU, bool LD, bool Camp, bool TravelWarn, bool TravelOK, uint8_t brightness);
	
protected:
private:
	CLed( const CLed &c );
	CLed& operator=( const CLed &c );

}; //CLed

#endif //__CLED_H__
