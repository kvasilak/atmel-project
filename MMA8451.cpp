/* 
* MMA8451.cpp
*
* Created: 3/21/2015 8:42:28 PM
* Author: keith
*/
#include <math.h>
#include "common.h"
#include "MMA8451.h"
#include "CSerial.h"
#include "i2cmaster.h"

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define MMA8451_ADDRESS                 (0x38)    // 0x3a if A input is high
/*=========================================================================*/

#define MMA8451_REG_OUT_X_MSB     0x01
#define MMA8451_REG_SYSMOD        0x0B
#define MMA8451_REG_WHOAMI        0x0D
#define MMA8451_REG_XYZ_DATA_CFG  0x0E
#define MMA8451_REG_PL_STATUS     0x10
#define MMA8451_REG_PL_CFG        0x11
#define MMA8451_REG_CTRL_REG1     0x2A
#define MMA8451_REG_CTRL_REG2     0x2B
#define MMA8451_REG_CTRL_REG4     0x2D
#define MMA8451_REG_CTRL_REG5     0x2E



#define MMA8451_PL_PUF            0
#define MMA8451_PL_PUB            1
#define MMA8451_PL_PDF            2
#define MMA8451_PL_PDB            3
#define MMA8451_PL_LRF            4  
#define MMA8451_PL_LRB            5  
#define MMA8451_PL_LLF            6  
#define MMA8451_PL_LLB            7  

// enum
// {
//   MMA8451_RANGE_8_G           = 0b10,   // +/- 8g
//   MMA8451_RANGE_4_G           = 0b01,   // +/- 4g
//   MMA8451_RANGE_2_G           = 0b00    // +/- 2g (default value)
// } mma8451_range_t;
// 

/* Used with register 0x2A (MMA8451_REG_CTRL_REG1) to set bandwidth */
//enum
// {
//   MMA8451_DATARATE_800_HZ     = 0b000, //  400Hz 
//   MMA8451_DATARATE_400_HZ     = 0b001, //  200Hz
//   MMA8451_DATARATE_200_HZ     = 0b010, //  100Hz
//   MMA8451_DATARATE_100_HZ     = 0b011, //   50Hz
//   MMA8451_DATARATE_50_HZ      = 0b100, //   25Hz
//   MMA8451_DATARATE_12_5_HZ    = 0b101, // 6.25Hz
//   MMA8451_DATARATE_6_25HZ     = 0b110, // 3.13Hz
//   MMA8451_DATARATE_1_56_HZ    = 0b111, // 1.56Hz
// } mma8451_dataRate_

#define SENSITIVITY_2G		  4096

#define PI					  3.14159

// default constructor
CMMA8451::CMMA8451()
{
} //MMA8451


bool CMMA8451::Init()
{
	bool success = false;
	
	uint8_t deviceid = 0x42;
	
	//writeRegister8(MMA8451_REG_CTRL_REG2, 0x40); // reset
	
	if(readRegister8(MMA8451_REG_WHOAMI, &deviceid))
	{
		if (deviceid == 0x1A)
		{
			// MMA8451 detected 
			CSerial::is() << "found MMA8451\n";
			
			 writeRegister8(MMA8451_REG_CTRL_REG2, 0x40); // reset

			while (readRegister8(MMA8451_REG_CTRL_REG2) & 0x40);
    
			writeRegister8(MMA8451_REG_CTRL_REG1, 1);

			success = true;
		}
		else		{			CSerial::is() << "unknown device; " << deviceid << "\n";		}	}
	else
	{
		CSerial::is() << "MMA8451 read failed\n";
	}

	
	writeRegister8(MMA8451_REG_CTRL_REG2, 0x40); // reset
	
	writeRegister8(MMA8451_REG_CTRL_REG1, 0x01); // active, max rate
	
	return success;
}

void CMMA8451::PitchRoll()
{
	int16_t x;
	int16_t y;
	int16_t z;
	
	ReadXYZ(x, y, z);
	
	float Xout_g = ((float) x) / SENSITIVITY_2G;		// Compute X-axis output value in g's
	float Yout_g = ((float) y) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
	float Zout_g = ((float) z) / SENSITIVITY_2G;		// Compute Z-axis output value in g's
			
	float Pitch = atan2 (-Xout_g, sqrt (Yout_g*Yout_g + Zout_g*Zout_g)) * 180 / PI;		// Equation 37 in the AN3461
		
	float Roll;
		
	if (Zout_g > 0)																	// Equation 38 in the AN3461
		Roll = atan2 (Yout_g, sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
	else
		Roll = atan2 (Yout_g, - sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
		
	CSerial::is() << "P, R; " << Pitch << ", " << Roll << "\n";
}

void CMMA8451::ReadXYZ()
{

	int16_t X;
	int16_t Y;
	int16_t Z;
	ReadXYZ(X,Y,Z);
	
	CSerial::is() << "XYZ, " << X <<", " << Y << ", " << Z << "\n";
}

void CMMA8451::ReadXYZ(int16_t &X, int16_t &Y, int16_t &Z)
{
		uint8_t high;
	uint8_t low;
	
	if(i2c_start(MMA8451_ADDRESS)==0)// if A is 38, its 0x3a
	{
		if(i2c_write(MMA8451_REG_OUT_X_MSB)==0) //Control reg single register
		{
			if(i2c_rep_start(MMA8451_ADDRESS + I2C_READ)==0)
			{
				high = i2c_readAck();
				low =  i2c_readAck();
				X = high << 8;
				X |= low;
				X >>= 2;

				
				high = i2c_readAck();
				low =  i2c_readAck();
				Y = high << 8;
				Y |= low;
				Y >>= 2;
								
				high = i2c_readAck();
				low =  i2c_readNak();
				i2c_stop();
				
				Z = high << 8;
				Z |= low;
				Z >>= 2;
			}
		}
	}

}

uint8_t CMMA8451::readRegister8(uint8_t reg)
{
	uint8_t d;
	
	readRegister8(reg, &d);
	
	return d;

}

bool CMMA8451::readRegister8(uint8_t reg, uint8_t *data) 
{
	bool success = false;
	
	if(i2c_start(MMA8451_ADDRESS)==0)// if A is 38, its 0x3a
	{
	   if(i2c_write(reg)==0) //Control reg single register
	   {
		if(i2c_rep_start(MMA8451_ADDRESS + I2C_READ)==0)
		{
	      *data = i2c_readNak();
			i2c_stop();
			success = true;
		  }
	   }
	}

	return success; 
}

void CMMA8451::writeRegister8(uint8_t reg, uint8_t data) 
{
	if(i2c_start(MMA8451_ADDRESS)==0)
	{
	   if(i2c_write(reg)==0) //Control reg single register
	   {
	      if(i2c_write(data)==0)
		  {
			  i2c_stop();
		  }
	   }
	}
}