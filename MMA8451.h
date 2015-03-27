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
	void Read(uint16_t &X, uint16_t &y, uint16_t &z);
	
	bool readRegister8(uint8_t reg, uint8_t *data);

protected:
private:
	CMMA8451();
	CMMA8451( const CMMA8451 &c );
	CMMA8451& operator=( const CMMA8451 &c );
	
	//bool readRegister8(uint8_t reg, uint8_t *data);
	void writeRegister8(uint8_t reg, uint8_t data);

}; //MMA8451

#endif //__MMA8451_H__
