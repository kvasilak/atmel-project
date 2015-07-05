/* 
* Cio.cpp
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/

#include <avr/io.h>
#include <avr/power.h> //perihperal power control
//#include <util/delay.h>
#include "Cio.h"
#include "CLeds.h"
#include "CSerial.h"

//for ignition interrupt
#include <avr/interrupt.h>

//for sleep obviously
#include <avr/sleep.h>

volatile bool Cio::IgnitionChanged = false;


// default constructor
Cio::Cio()
{
} //Cio

//setup IO lines
//Read using  PINx
//Write using PORTx
void Cio::Direction()
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
}

void Cio::Pullups()
{
		//port A
		PORTA = 0xf0;
		
		//0		ADC		Left Height
		//1		ADC		Right height
		//2		Out		power up
		//3		OUt		Compressor
		//4		in		Remote RU
		//5		in		Remote RD
		//6		in		Remote LU
		//7		in		Remote LD
		
		
		PORTB = 0x0F;
		
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
		
		PORTD = 0x00;
		//Port D
		//0		RX		Debug serial
		//1		TX		Debug serial
		//2		in		Calibrate
		//3		in		Ignition On
		//4		in		switch Down
		//5		in		switch Up
		//6		in		switch Travel
		//7		in		switch Camp
}



void Cio::Init()
{
	Direction();
	Pullups();
	
	//use to debounce switch inputs
	PushCalibrate.Attach(IO_PORTD, PORTD2);
	RockerDown.Attach(IO_PORTD, PORTD4);
	RockerUp.Attach(IO_PORTD, PORTD5);
	PushTravel.Attach(IO_PORTD, PORTD6);
	PushCamp.Attach(IO_PORTD, PORTD7);
	

	//setup initial values
	CampChanged();
	TravelChanged();
	RockerChanged();
	SteeringRemoteChanged();
	OutSideRemoteChanged();
	CalibrateChanged();
	
	ResetButtons();

	EnableIgnOnInterrupt();
}

//update switch states and debounce
void Cio::Run()
{
	//debounce inputs
	RockerDown.Update();
	RockerUp.Update();
	PushTravel.Update();
	PushCamp.Update();
	PushCalibrate.Update();
}

void Cio::EnableIgnOnInterrupt()
{
	EICRA = 4; //(1 << ISC00);    // set INT1 to trigger on both edges
	EIMSK  = 2;//|= (1 << INT2);     
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
 	
	bool Changed	= OldCamp			!= (bool)PushCamp;		//port D bit 7
	Changed			|= OldInsideCamp	!= camp;  //port B bit 3
	 
	OldCamp		= (bool)PushCamp;
	OldInsideCamp  = camp;

	return Changed;
}

//returns true if the travel mode buttons have changed state
//camp switch
//camp on steering remote
bool Cio::TravelChanged()
{
	static bool OldTravel				= false;
	static bool OldInsideTravel			= false;	
	
	bool trav = (PINB & _BV(PORTB3));

	bool Changed	= OldTravel				!= (bool)PushTravel;
		 Changed	|= OldInsideTravel		!= trav; 
	
	OldTravel		= (bool)PushTravel;
	OldInsideTravel = trav;
	
	return Changed;
}

bool Cio::CalibrateChanged()
{
	static bool OldCal	= false;
	bool changed = false;
	
	//only consider a press a change a release isn't a change
	if(PushCalibrate > 0)
	{
		changed = OldCal != (bool)PushCalibrate;
	}
	OldCal = (bool)PushCalibrate;
	
	return changed;
}

void Cio::RockerSwitch()
{
	if(RockerUp.Level())
	{
		CSerial::is() << "RockerUp.Level()\n";
		
		if(FillPressed)
		{
			LeftFillOff();
			RightFillOff();
			
			FillPressed = false;
		}
		else
		{
			//turn off and unlatch the dump button
			if(DumpPressed)
			{
				LeftDumpOff();
				RightDumpOff();
				
				DumpPressed = false;
			}
			
			LeftFillOn();
			RightFillOn();
			
			FillPressed = true;
		}
	}
	
	if(RockerDown.Level())
	{
		CSerial::is() <<  "RockerDown.Level()\n";
		
		//toggle on button press
		if(DumpPressed)
		{
			LeftDumpOff();
			RightDumpOff();
			
			DumpPressed = false;
		}
		else
		{
			//turn off and unlatch the fill button
			if(FillPressed)
			{
				LeftFillOff();
				RightFillOff();
				
				FillPressed = false;
			}
			
			LeftDumpOn();
			RightDumpOn();
			
			DumpPressed = true;
		}
	}

}

void Cio::OutsideRemote()
{
	bool RemoteLeftUp = PINA & _BV(4);		//left down
	bool RemoteLeftDown =PINA & _BV(6);		//right down
	bool RemoteRightUp = PINA & _BV(5);		//left up
	bool RemoteRightDown =PINA & _BV(7);	//right down
	
	FillPressed = false;
	DumpPressed = false;
	
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
		if(FillPressed)
		{
			LeftFillOff();
			RightFillOff();
			
			FillPressed = false;
			
		}
		else
		{
			//turn off and unlatch the dump button
			if(DumpPressed)
			{
				LeftDumpOff();
				RightDumpOff();
				
				DumpPressed = false;
			}
			
			LeftFillOn();
			RightFillOn();
			
			FillPressed = true;
			DumpPressed = false;
		}
		
	}

	
	if(RemoteDown)
	{
		if(DumpPressed)
		{
			LeftDumpOff();
			RightDumpOff();
			
			DumpPressed = false;
			
		}
		else
		{
			//turn off and unlatch the fill button
			if(FillPressed)
			{
				LeftFillOff();
				RightFillOff();
				
				FillPressed = false;
			}
			
			LeftDumpOn();
			RightDumpOn();
			
			DumpPressed = true;
		}
	}

}


void Cio::CampSwitches()
{
	bool SteeringCamp = PINB & _BV(1);
	
	if(!PushCamp.Level() || SteeringCamp)
	{
		CLeds::is().CampOn();
		CLeds::is().TravelOKOff();
	}

}

void Cio::TravelSwitches()
{
	bool SteeringTravel = PINB & _BV(3);
	
	if(!PushTravel.Level() || SteeringTravel)
	{
		CLeds::is().CampOff();
		CLeds::is().TravelOKOn();
	}

}

void Cio::AllOff()
{
	RightFillOff();
	RightDumpOff();
	LeftFillOff();
	LeftDumpOff();
}

void Cio::Right(eValveStates s)
{
	switch(s)
	{
		case eValveStates::Dump:
			RightFillOff();
			RightDumpOn();
		break;
		case eValveStates::Fill:
			RightDumpOff();
			RightFillOn();
		break;
		case eValveStates::Hold:
			RightFillOff();
			RightDumpOff();
		break;
		default:
			RightFillOff();
			RightDumpOff();
	}
}

void Cio::Left(eValveStates s)
{
	switch(s)
	{
		case eValveStates::Dump:
			LeftFillOff();
			LeftDumpOn();
		break;
		case eValveStates::Fill:
			LeftFillOn();
			LeftDumpOff();
		break;

		case eValveStates::Hold:
			LeftFillOff();
			LeftDumpOff();
		break;
		default:
			LeftFillOff();
			LeftDumpOff();
	}
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
	//PORTA |= _BV(3);
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

void Cio::PowerOn()
{
	//Turn on 5v, PA2
	PORTA |= _BV(PA2);
}

void Cio::PowerOff()
{
	//Turn off 5v, PA2
	PORTA &= ~_BV(PA2);
}

void Cio::CompressorOn()
{
	//Turn on Air compressor enable, PA3
	PORTA |= _BV(PA3);
}

void Cio::CompressorOff()
{
	PORTA &= ~_BV(PA3);
}

//put the system into sleep to conserve power
void Cio::Sleep()
{
	//Turn off all LEDS
	CLeds::is().AllOff();
	
	//turn off all valves
	AllOff();
	
	CompressorOff();
	
	PowerOff();
	
	//turn off unnecessary peripherals
    //power_all_disable();

	
	//Set CPU to sleep, will wake up on an ignition IRQ
 	cli();
 	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 	
 	// sleep_mode() has a possible race condition
 	sleep_enable();
 	sei();
 	sleep_cpu();
 	sleep_disable();
}

void Cio::Wakeup()
{
	//int i=0;
	PowerOn();
	CompressorOn();
	
// 	for(i=0; i<2; i++)
// 	{
// 		CLeds::is().Init();
// 	}
// 	
	CLeds::is().Init();
	CLeds::is().Init();
	CLeds::is().Init();
	CLeds::is().Init();
	CLeds::is().Dim(100);
	
	CLeds::is().ActiveOn();
	
	
}

 bool Cio::IsIgnitionOn()
 {
 	return (PIND & _BV(3)) & _BV(3);
 }

// clock interrupt - clear flag immediately to resume count
//PD2 (INT0/I26)
ISR(INT1_vect)
{
	//Cio::IgnitionChanged = true;
}

