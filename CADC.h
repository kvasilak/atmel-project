/* 
* CADC.h
*
* Created: 1/23/2015 6:37:30 PM
* Author: keith
*/


#ifndef __CADC_H__
#define __CADC_H__
#include <inttypes.h>

class CADC
{

public:
	static CADC &is()
	{
		static CADC a;
		
		return a;
	}

	void Init();
	uint16_t Read(uint8_t ch);

private:
CADC();
	CADC& operator=( const CADC &c );

}; //CADC

#endif //__CADC_H__
