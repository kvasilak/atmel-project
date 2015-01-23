/* 
* CLeds.cpp
*
* Created: 1/22/2015 10:44:30 PM
* Author: keith
*/


#include "CLeds.h"

// 4 LEDs for either bank ( LEDOUT0 or LEDOUT1 )
static const uint8_t Led0On		= uint8_t(0x03);	//00000011
static const uint8_t Led0Off	= uint8_t(~0x03);	//00000011
static const uint8_t Led1On		= uint8_t(0x0C);	//00001100
static const uint8_t Led1Off	= uint8_t(~0x0C);	//00001100
static const uint8_t Led2On		= uint8_t(0x30);	//00110000
static const uint8_t Led2Off	= uint8_t(~0x30);	//00110000
static const uint8_t Led3On		= uint8_t(0xC0);	//11000000
static const uint8_t Led3Off	= uint8_t(~0xC0);	//11000000



// default constructor
CLeds::CLeds():
LEDout0(0),
LEDout1(0)
{
} //CLeds


void CLeds::Init()
{
	CLedDriver::Init();
}

void CLeds::RightUpOn()
{
	LEDout0 |= Led0On;
	
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::RightUpOff()
{
	LEDout0 &= Led0Off;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::RightDownOn()
{
	LEDout0 |= Led1On;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::RightDownOff()
{
	LEDout0 &= Led1Off;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::ActiveOn()
{
	LEDout0 |= Led2On;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::ActiveOff()
{
	LEDout0 &= Led2Off;
	WriteReg(LEDReg0, LEDout0);
}


void CLeds::LeftUpOn()
{
	LEDout0 |= Led3On;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::LeftUpOff()
{
	LEDout0 &= Led3Off;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::LeftDownOn()
{
	LEDout1 |= Led0On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::LeftDownOff()
{
	LEDout1 &= Led0Off;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::CampOn()
{
	LEDout1 |= Led1On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::CampOff()
{
	LEDout1 &= Led1Off;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::TravelWarnOn()
{
	LEDout1 |= Led2On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::TravelWarnOff()
{
	LEDout1 &= Led2Off;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::TravelOKOn()
{
	LEDout1 |= Led3On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::TravelOKOff()
{
	LEDout1 &= Led3Off;
	WriteReg(LEDReg1, LEDout1);
}