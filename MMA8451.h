/* 
* MMA8451.h
*
* Created: 3/21/2015 8:42:28 PM
* Author: keith
*/


#ifndef __MMA8451_H__
#define __MMA8451_H__

#include <inttypes.h>

class CMMA8451
{

public:
	static CMMA8451 &is()
	{
		static CMMA8451 me;
	
		return me;
	}
	
	bool Init();
	void ReadXYZ();
	void ReadXYZ(int16_t &X, int16_t &y, int16_t &z);
	
	bool readRegister8(uint8_t reg, uint8_t *data);
	
	uint8_t readRegister8(uint8_t reg);
	
	void writeRegister8(uint8_t reg, uint8_t data);
	
	
	void PitchRoll();

protected:
private:
	CMMA8451();
	CMMA8451( const CMMA8451 &c );
	CMMA8451& operator=( const CMMA8451 &c );

}; //MMA8451

#endif //__MMA8451_H__
