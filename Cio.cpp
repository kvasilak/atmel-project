/* 
* Cio.cpp
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/

#include <avr/io.h>
#include "Cio.h"
#include "CLeds.h"

// default constructor
Cio::Cio()
{
} //Cio

//setup IO lines
//Read using  PINx
//Write using PORTx
void Cio::Init()
{
	//port A
	DDRA = 0x0C;
	
	//0		ADC		Left Height
	//1		ADC		Right height
	//2		Out		power up
	//3		OUt		Compressor
	//4		in		Remote RU
	//5		in		Remote RD
	//6		in		Remote LU
	//7		in		Remote LD
	
	
	DDRB = 0xF0;
	
	//port B
	//0		in		Remote up
	//1		in		Remote Down
	//2		in		Remote Travel
	//3		in		Remote Camp
	//4		Out		Right Up
	//5		Out		Right Down
	//6		Out		Left up
	//7		Out		Left Down
	
	//DDRC = 0x;
	//Port C
	//0		SCL		
	//1		SDA		
	//2		JTAG		
	//3		JTAG		
	//4		JTAG		
	//5		JTAG		
	//6		in		Outside valid
	//7		in		Steering Valid
	
	DDRD = 0x02;
	//Port D
	//0		RX		Debug serial
	//1		TX		Debug serial
	//2		in		Calibrate
	//3		in		Ignition On
	//4		in		switch Down
	//5		in		switch Up
	//6		in		switch Travel
	//7		in		switch Camp
	
	//use to debounce switch inputs
	RockerDown.Attach(IO_PORTD, PORTD4);
	RockerUp.Attach(IO_PORTD, PORTD5);
	PushTravel.Attach(IO_PORTD, PORTD6);
	PushCamp.Attach(IO_PORTD, PORTD7);
	
	Run();
	
	//setup initial values
	RockerSwitch();
	OutsideRemote();
	SteeringRemote();
	CampChanged();
	TravelChanged();
}

//update switch states and debounce
void Cio::Run()
{
	//debounce inputs
	RockerDown.Update();
	RockerUp.Update();
	PushTravel.Update();
	PushCamp.Update();
}

bool Cio::RockerChanged()
{
	static bool OldRockerDown = false;
	static bool OldRockerUp = false;

	bool changed   = (OldRockerDown	!= (bool)RockerDown);
		 changed  |= (OldRockerUp	!= (bool)RockerUp);
	
	OldRockerDown = (bool)RockerDown;
	OldRockerUp = (bool)RockerUp;
	
	return changed;
}

bool Cio::OutSideRemoteChanged()
{
	static uint8_t OldPortA = false;
	uint8_t porta = PINA & 0xF0;
	
	bool changed = OldPortA != porta;
	
	OldPortA = porta;
	
	return changed;
}

bool Cio::SteeringRemoteChanged()
{
	static uint8_t OldPortB = false;
	uint8_t portb = PINB & 0x05;

	bool changed = OldPortB != portb;

	OldPortB = portb;

	return changed;
}



//returns true if any camp mode button has changed state
//camp switch
//camp on steering remote
bool Cio::CampChanged()
{
	static bool OldCamp					= false;
	static bool OldInsideCamp			= false;
	 
	bool camp = (PINB & _BV(PORTB1));
 	
	bool CampChanged	= OldCamp			!= (bool)PushCamp;		//port D bit 7
	CampChanged			|= OldInsideCamp	!= camp;  //port B bit 3
	 
	OldCamp		= (bool)PushCamp;
	OldInsideCamp  = camp;
	
	return CampChanged;
}

//returns true if the travel mode buttons have changed state
//camp switch
//camp on steering remote
bool Cio::TravelChanged()
{
	static bool OldTravel				= false;
	static bool OldInsideTravel			= false;	
	
	bool trav = (PINB & _BV(PORTB3));

	bool TravelChanged	= OldTravel				!= (bool)PushTravel;		 //port D bit 6
	TravelChanged		|= OldInsideTravel		!= trav; //port B bit 2
	
	OldTravel		= (bool)PushTravel;
	OldInsideTravel = trav;
	
	return TravelChanged;
}

void Cio::RockerSwitch()
{
	if(RockerUp.Level())
	{
		LeftFillOn();
		RightFillOn();
	}
	else if(RockerDown.Level())
	{
		LeftDumpOn();
		RightDumpOn();
	}
	else
	{
		LeftDumpOff();
		RightDumpOff();
		
		LeftFillOff();
		RightFillOff();
	}
}

void Cio::OutsideRemote()
{
	bool RemoteLeftUp = PINA & _BV(4);		//left down
	bool RemoteLeftDown =PINA & _BV(6);		//right down
	bool RemoteRightUp = PINA & _BV(5);		//left up
	bool RemoteRightDown =PINA & _BV(7);	//right down
	
	if(RemoteLeftUp)
		LeftFillOn();
	else
		LeftFillOff();
		
	if(RemoteRightUp)
		RightFillOn();
	else
		RightFillOff();
		
	if(RemoteLeftDown)
		LeftDumpOn();
	else
		LeftDumpOff();
		
	if(RemoteRightDown)
		RightDumpOn();
	else
		RightDumpOff();
	
}

void Cio::SteeringRemote()
{
	bool RemoteUp = PINB & _BV(2);
	bool RemoteDown = PINB & _BV(0);
	
	if(RemoteUp)
	{
		LeftFillOn();
		RightFillOn();
	}
	else
	{
		LeftFillOff();
		RightFillOff();
	}
	
	if(RemoteDown)
	{
		LeftDumpOn();
		RightDumpOn();
	}
	else
	{
		LeftDumpOff();
		RightDumpOff();
	}
}



void Cio::CampSwitches()
{
	if(PushCamp.Level())
	{
		
	}
	else
	{
		
	}
}

void Cio::TravelSwitches()
{
	if(PushTravel.Level())
	{
		
	}
	else
	{
		
		
	}
}

void Cio::AllOff()
{
	RightFillOff();
	RightDumpOff();
	LeftFillOff();
	LeftDumpOff();
}

void Cio::RightFillOn()
{
	PORTB |= _BV(4);
	CLeds::is().RightFillOn();
}

void Cio::RightFillOff()
{
	PORTB &= ~_BV(4);
	CLeds::is().RightFillOff();
}

void Cio::RightDumpOn()
{
	PORTB |= _BV(5);
	CLeds::is().RightDumpOn();
}

void Cio::RightDumpOff()
{
	PORTB &= ~_BV(5);
	CLeds::is().RightDumpOff();
}


void Cio::LeftFillOn()
{
	PORTB |= _BV(6);
	CLeds::is().LeftFillOn();
}

void Cio::LeftFillOff()
{
	PORTB &= ~_BV(6);
	CLeds::is().LeftFillOff();
}

void Cio::LeftDumpOn()
{
	PORTB |= _BV(7);
	CLeds::is().LeftDumpOn();
}

void Cio::LeftDumpOff()
{
	PORTB &= ~_BV(7);
	CLeds::is().LeftDumpOff();
}


bool Cio::IsHolding()
{
	bool hold = true;
	
	if(RockerUp.Level() || RockerDown.Level())
	{
		hold = false;
	}
	
	return hold;
}