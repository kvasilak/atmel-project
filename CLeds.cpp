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

void CLeds::AllOff()
{
	RightFillOff();
	RightDumpOff();
	ActiveOff();
	LeftFillOff();
	LeftDumpOff();
	CampOff();
	TravelOff();
}

void CLeds::RightFillOn()
{
	LEDout0 |= Led2On;
	
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::RightFillOff()
{
	LEDout0 &= Led2Off;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::RightDumpOn()
{
	LEDout1 |= Led1On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::RightDumpOff()
{
	LEDout1 &= Led1Off;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::ActiveOn()
{
	LEDout1 |= Led3On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::ActiveOff()
{
	LEDout1 &= Led3Off;
	WriteReg(LEDReg1, LEDout1);
}


void CLeds::LeftFillOn()
{
	LEDout0 |= Led3On;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::LeftFillOff()
{
	LEDout0 &= Led3Off;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::LeftDumpOn()
{
	LEDout1 |= Led2On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::LeftDumpOff()
{
	LEDout1 &= Led2Off;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::CampOn()
{
	LEDout1 |= Led0On;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::CampOff()
{
	LEDout1 &= Led0Off;
	WriteReg(LEDReg1, LEDout1);
}

void CLeds::TravelOn()
{
	LEDout0 |= Led1On;
	WriteReg(LEDReg0, LEDout0);
}

void CLeds::TravelOff()
{
	LEDout0 &= Led1Off;
	WriteReg(LEDReg0, LEDout0);
}