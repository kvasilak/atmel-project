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
	
	uint16_t GetRightHeight(void);
	uint16_t GetLeftHeight(void);
	uint8_t GetDimmer(void);
	bool LeftHeightOK(void);
	bool RightHeightOK(void);
    
    int32_t GetLeftAvgHeight(void);
    int32_t GetRightAvgHeight(void);

private:
CADC();
	CADC& operator=( const CADC &c );
    
    int32_t LeftHeightAverages[10];
    int LeftHeightAverageCount;
    
    int32_t RightHeightAverages[10];
    int RightHeightAverageCount;

}; //CADC

#endif //__CADC_H__
