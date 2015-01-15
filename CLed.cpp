/* 
* CLed.cpp
*
* Created: 1/13/2015 10:33:49 PM
* Author: keith
*/


#include "CLed.h"
#include "i2cmaster.h"

// default constructor
CLed::CLed()
{
	i2c_init();                             // initialize I2C library
} //CLed


void CLed::Set(bool RU, bool RD, bool Active, bool LU, bool LD, bool Camp, bool TravelWarn, bool TravelOK, uint8_t brightness)
{
	uint8_t low;
	
	low  = RU << 1;
	low  |= RD << 3;
	low  |= LU << 5;
	low  |= LD << 7;
	
	
	  if(i2c_start_wait(0x40))
	   if(i2c_write(0x80)==0) //Control reg autoincrement starting at pwm0
	    if(i2c_write(0x81)==0)
	     if(i2c_write(0x03)==0)
	      if(i2c_write(brightness)==0)       //PWM0
	       if(i2c_write(brightness)==0)       //PWM1
	        if(i2c_write(brightness)==0)       //PWM2
	         if(i2c_write(brightness)==0)       //PWM3
	          if(i2c_write(brightness)==0)       //PWM4
	           if(i2c_write(brightness)==0)       //PWM5
	            if(i2c_write(brightness)==0)       //PWM6
	             if(i2c_write(brightness)==0)       //PWM7
	              if(i2c_write(0xFF)==0)       //GRPPWM
	               if(i2c_write(0x00)==0)       //GRPFREQ
	                if(i2c_write(low)==0) //0x55)==0)       //LEDOUT0
	                 if(i2c_write(0xAA)==0)       //LEDOUT1
	                  ;
	  
	  i2c_stop();
}
