/* 
* CLed.cpp
*
* Created: 1/13/2015 10:33:49 PM
* Author: keith
*/

#include <stdint.h>
#include "CLedDriver.h"
#include "i2cmaster.h"
#include "CADC.h"
#include "CSerial.h"

// default constructor
CLedDriver::CLedDriver():
LEDReg0(0x0C),
LEDReg1(0x0D)
{
	// initialize I2C library
	i2c_init();
}

void CLedDriver::Init()
{
	  if(i2c_start(0x40)==0) //LED controller address
      {      
	   if(i2c_write(0x80)==0)  //Control reg autoincrement starting at mode0
       {
	    if(i2c_write(0x80)==0)  //mode1 auto inc
        {
	     if(i2c_write(0x00)==0)  //mode2
         {
	      if(i2c_write(0xFF)==0)       //PWM0
          {
	       if(i2c_write(0xFF)==0)       //PWM1
           {
	        if(i2c_write(0xFF)==0)       //PWM2
            {
	         if(i2c_write(0xFF)==0)       //PWM3
             {
	          if(i2c_write(0xFF)==0)       //PWM4
              {
	           if(i2c_write(0xFF)==0)       //PWM5
               {
	            if(i2c_write(0xFF)==0)       //PWM6
                {
	             if(i2c_write(0xFF)==0)       //PWM7
                 {
	              if(i2c_write(0x00)==0)    //GRPPWM
                  {
	               if(i2c_write(0x00)==0)    //GRPFREQ
                   {
	                if(i2c_write(0X00)==0)    //LEDOUT0
                    {
	                 if(i2c_write(0x00)==0)    //LEDOUT1
                     {
                        CSerial::is() << "** CLED Driver Init Succeeded\n";
                     }                         
                     else {CSerial::is() << "** CLED Driver Init failed 1\n"; } 
                    }
                    else {CSerial::is() << "** CLED Driver I2C Init failed 2!!\n"; } 
                   }
                   else {CSerial::is() << "** CLED Driver I2C Init failed 3!!\n"; } 
                  }
                  else {CSerial::is() << "** CLED Driver I2C Init failed 4!!\n"; } 
                 }
                 else {CSerial::is() << "** CLED Driver I2C Init failed 5!!\n"; } 
                }
                else {CSerial::is() << "** CLED Driver I2C Init failed 6!!\n"; } 
               }
               else {CSerial::is() << "** CLED Driver I2C Init failed 7!!\n"; } 
              }
              else {CSerial::is() << "** CLED Driver I2C Init failed 8!!\n"; } 
             }
             else {CSerial::is() << "** CLED Driver I2C Init failed 9 !!\n"; } 
            }
            else {CSerial::is() << "** CLED Driver I2C Init failed 0!!\n"; } 
           }
           else {CSerial::is() << "** CLED Driver I2C Init failed a!!\n"; } 
          }
          else {CSerial::is() << "** CLED Driver I2C Init failed b !!\n"; } 
         }
         else {CSerial::is() << "** CLED Driver I2C Init failed c!!\n"; } 
        }
        else {CSerial::is() << "** CLED Driver I2C Init failed d!!\n"; } 
       }                                                                
       else {CSerial::is() << "** CLED Driver I2C Init failed e!!\n"; }                                                                                                                                                                    
      }
      else {CSerial::is() << "** CLED Driver I2C Init failed f!!\n"; }                                       
	  
	  i2c_stop();
}

//brightness, 0 to 255
void CLedDriver::Dim(uint8_t Brightness)
{
	WriteReg(0X0A, Brightness);
}

void CLedDriver::WriteReg(uint8_t Reg, uint8_t data)
{
	if(i2c_start(0x40)==0)
	   if(i2c_write(Reg)==0) //Control reg single register
	      if(i2c_write(data)==0)
		  {
			  i2c_stop();
		  }
}