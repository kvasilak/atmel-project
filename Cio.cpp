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
volatile uint8_t ispa;

#define REV3_PCB


// default constructor
Cio::Cio() :
Awake(false),
FillPressed(false),
DumpPressed(false)
{
} //Cio

//setup IO lines
//Read using  PINx
//Write using PORTx
void Cio::Direction()
{
	#ifdef REV2_PCB
		//port A
		DDRA = 0x0C;
		
		//0		ADC		0	Left Height
		//1		ADC		0	Right height
		//2		Out		1	power up
		//3		OUt		1	Compressor
		
		//4		in		0	Remote RU
		//5		in		0	Remote RD
		//6		in		0	Remote LU
		//7		in		0	Remote LD
		
		
		DDRB = 0xF0;
		
		//port B
		//0		in		0	Remote up
		//1		in		0	Remote Down
		//2		in		0	Remote Travel
		//3		in		0	Remote Camp
		
		//4		Out		1	Right Up
		//5		Out		1	Right Down
		//6		Out		1	Left up
		//7		Out		1	Left Down
		
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
		//0		RX		0	Debug serial
		//1		TX		1	Debug serial
		//2		in		0	Calibrate
		//3		in		0	Ignition On
		
		//4		in		0	switch Down
		//5		in		0	switch Up
		//6		in		0	switch Travel
		//7		in		0	switch Camp
		#endif
		
		#ifdef REV3_PCB
		//port A
		DDRA = 0xF0;
		
		//0		ADC		0	Left Height
		//1		ADC		0	Right height
		//2		In		0	Ignition on
		//3		ADC		0	Dimming
		
		//4		out		1	RD 
		//5		out 	1	RU
		//6		out		1	LD
		//7		out		1	LU
		
		
		DDRB = 0x00;
		
		//port B
		//0		in		0	Remote LD
		//1		in		0	Remote LU
		//2		in		0	Remote RD
		//3		in		0	Remote Camp
		
		//4		in 		0	Remote RU
		//5		in 		0	Remote travel
		//6		in 		0	Remote down
		//7		in		0	Remote up
		
		DDRC = 0x80;
		//Port C
		//0		SCL
		//1		SDA
		//2		JTAG
		//3		JTAG
		//4		JTAG
		//5		JTAG
		//6		in		0 spare
		//7		out		1 Compressor on
		
		DDRD = 0x02;
		//Port D
		//0		RX		0	Debug serial
		//1		TX		1	Debug serial
		//2		out		0	Power up
		//3		in		0	Travel
		
		//4		in		0	Calibrate
		//5		in		0	Camp
		//6		in		0	Down
		//7		in		0	up
		#endif
}

void Cio::Pullups()
{
	#ifdef REV2_PCB
		//port A
		PORTA = 0xf0;
		
		//0		ADC		0	Left Height
		//1		ADC		0	Right height
		//2		Out		0	power up
		//3		OUt		0	Compressor
		
		//4		in		1	Remote RU
		//5		in		1	Remote RD
		//6		in		1	Remote LU
		//7		in		1	Remote LD
		
		
		PORTB = 0x0F;
		
		//port B
		//0		in		1	Remote up
		//1		in		1	Remote Down
		//2		in		1	Remote Travel
		//3		in		1	Remote Camp
		
		//4		Out		0	Right Up
		//5		Out		0	Right Down
		//6		Out		0	Left up
		//7		Out		0	Left Down
		
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
		//0		RX		0	Debug serial
		//1		TX		0	Debug serial
		//2		in		0	Calibrate
		//3		in		0	Ignition On
		
		//4		in		0	switch Down
		//5		in		0	switch Up
		//6		in		0	switch Travel
		//7		in		0	switch Camp
		#endif
		
#ifdef REV3_PCB
		//port A
		PORTA = 0x00;
		
		//0		ADC		0	Left Height
		//1		ADC		0	Right height
		//2		In		0	Ignition on
		//3		ADC		0	Dimming
		
		//4		out		0	RD
		//5		out 	0	RU
		//6		out		0	LD
		//7		out		0	LU
		
		
		PORTB = 0xFF;
		
		//port B
		//0		in		1	Remote LD
		//1		in		1	Remote LU
		//2		in		1	Remote RD
		//3		in		1	Remote Camp
		
		//4		in 		1	Remote RU
		//5		in 		1	Remote travel
		//6		in 		1	Remote down
		//7		in		1	Remote up
		
		//PORTC = 0x00;
		//Port C
		//0		SCL
		//1		SDA
		//2		JTAG
		//3		JTAG
		//4		JTAG
		//5		JTAG
		//6		in		0 spare
		//7		out		0 Compressor on
		
		PORTD = 0x00;
		//Port D
		//0		RX		0	Debug serial
		//1		TX		0	Debug serial
		//2		out		0	Power up
		//3		in		0	Travel
		
		//4		in		0	Calibrate
		//5		in		0	Camp
		//6		in		0	Down
		//7		in		0	up
#endif
}



void Cio::Init()
{
	Direction();
	Pullups();
	
	//use to debounce switch inputs
//#ifdef REV2_PCB
	//PushCalibrate.Attach(IO_PORTD, PORTD2);
	//RockerDown.Attach(IO_PORTD, PORTD4);
	//RockerUp.Attach(IO_PORTD, PORTD5);
	//PushTravel.Attach(IO_PORTD, PORTD6);
	//PushCamp.Attach(IO_PORTD, PORTD7);
//#endif

#ifdef REV3_PCB	
	PushTravel.Attach(IO_PORTD, PORTD3);
	PushCalibrate.Attach(IO_PORTD, PORTD4);
	PushCamp.Attach(IO_PORTD, PORTD5);
	RockerDown.Attach(IO_PORTD, PORTD6);
	RockerUp.Attach(IO_PORTD, PORTD7);
#endif
	

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
	//static uint8_t OldPort = false;
	//uint8_t port = PINB & 0x71;
	
	//bool changed = OldPort != port;
	
	//OldPort = port;
	
	return false;//changed;
}

bool Cio::SteeringRemoteChanged()
{
	//static uint8_t OldPort = false;
	//uint8_t port = PINB & 0xE8;

	//bool changed = OldPort != port;

	//OldPort = port;

	return false; //changed;
}



//returns true if any camp mode button has changed state
//camp switch
//camp on steering remote
bool Cio::CampChanged()
{
	//static bool OldCamp					= false;
	//static bool OldInsideCamp			= false;
	 //
	//bool camp = (PINB & _BV(PORTB3));
 	//
	//bool Changed	= OldCamp			!= (bool)PushCamp;		//port D bit 7
	//Changed			|= OldInsideCamp	!= camp;  //port B bit 3
	 //
	//OldCamp		= (bool)PushCamp;
	//OldInsideCamp  = camp;
//
	//return Changed;
	return false;
}

//returns true if the travel mode buttons have changed state
//camp switch
//camp on steering remote
bool Cio::TravelChanged()
{
	//static bool OldTravel				= false;
	//static bool OldInsideTravel			= false;	
	//
	//bool trav = (PINB & _BV(PORTB5));
//
	//bool Changed	= OldTravel				!= (bool)PushTravel;
		 //Changed	|= OldInsideTravel		!= trav; 
	//
	//OldTravel		= (bool)PushTravel;
	//OldInsideTravel = trav;
	//
	//return Changed;
	
	return false;
}

bool Cio::CalibrateChanged()
{
	//static bool OldCal	= false;
	//bool changed = false;
	//
	////only consider a press a change a release isn't a change
	//if(PushCalibrate > 0)
	//{
		//changed = OldCal != (bool)PushCalibrate;
	//}
	//OldCal = (bool)PushCalibrate;
	//
	//return changed;
	
	return false;
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
	//bool RemoteLeftDown  = PINB & _BV(0);
	//bool RemoteLeftUp    = PINB & _BV(1);		
    //bool RemoteRightDown = PINB & _BV(2);
	//bool RemoteRightUp   = PINB & _BV(4);		
	//
	//FillPressed = false;
	//DumpPressed = false;
	//
	//if(RemoteLeftUp)
		//LeftFillOn();
	//else
		//LeftFillOff();
		//
	//if(RemoteRightUp)
		//RightFillOn();
	//else
		//RightFillOff();
		//
	//if(RemoteLeftDown)
		//LeftDumpOn();
	//else
		//LeftDumpOff();
		//
	//if(RemoteRightDown)
		//RightDumpOn();
	//else
		//RightDumpOff();
	
}

void Cio::SteeringRemote()
{
	//bool RemoteUp = PINB & _BV(7);
	//bool RemoteDown = PINB & _BV(6);
	//
	//if(RemoteUp)
	//{
		//if(FillPressed)
		//{
			//LeftFillOff();
			//RightFillOff();
			//
			//FillPressed = false;
			//
		//}
		//else
		//{
			////turn off and unlatch the dump button
			//if(DumpPressed)
			//{
				//LeftDumpOff();
				//RightDumpOff();
				//
				//DumpPressed = false;
			//}
			//
			//LeftFillOn();
			//RightFillOn();
			//
			//FillPressed = true;
			//DumpPressed = false;
		//}
		//
	//}

	
	//if(RemoteDown)
	//{
		//if(DumpPressed)
		//{
			//LeftDumpOff();
			//RightDumpOff();
			//
			//DumpPressed = false;
			//
		//}
		//else
		//{
			////turn off and unlatch the fill button
			//if(FillPressed)
			//{
				//LeftFillOff();
				//RightFillOff();
				//
				//FillPressed = false;
			//}
			//
			//LeftDumpOn();
			//RightDumpOn();
			//
			//DumpPressed = true;
		//}
	//}

}


void Cio::CampSwitches()
{
	//bool SteeringCamp = PIND & _BV(5);
	//
	//if(!PushCamp.Level() || SteeringCamp)
	//{
		//CLeds::is().CampOn();
		//CLeds::is().TravelOKOff();
	//}

}

void Cio::TravelSwitches()
{
	//bool SteeringTravel = PIND & _BV(3);
	//
	//if(!PushTravel.Level() || SteeringTravel)
	//{
		//CLeds::is().CampOff();
		//CLeds::is().TravelOKOn();
	//}

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
	PORTA |= _BV(5);
	CLeds::is().RightFillOn();
}

void Cio::RightFillOff()
{
	PORTA &= ~_BV(5);
	CLeds::is().RightFillOff();
}

void Cio::RightDumpOn()
{
	PORTA |= _BV(4);
	CLeds::is().RightDumpOn();
}

void Cio::RightDumpOff()
{
	PORTA &= ~_BV(4);
	CLeds::is().RightDumpOff();
}


void Cio::LeftFillOn()
{
	PORTA |= _BV(7);
	CLeds::is().LeftFillOn();
}

void Cio::LeftFillOff()
{
	PORTA &= ~_BV(7);
	CLeds::is().LeftFillOff();
}

void Cio::LeftDumpOn()
{
	PORTA |= _BV(6);
	//PORTA |= _BV(3);
	CLeds::is().LeftDumpOn();
}

void Cio::LeftDumpOff()
{
	PORTA &= ~_BV(6);
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
	PORTD |= _BV(PD2);
}

void Cio::PowerOff()
{
	//Turn off 5v, PA2
	PORTD &= ~_BV(PD2);
}

void Cio::CompressorOn()
{
	//Turn on Air compressor enable, PA3
	PORTC |= _BV(PC7);
}

void Cio::CompressorOff()
{
	PORTC &= ~_BV(PC7);
}

//put the system into sleep to conserve power
void Cio::Sleep()
{
	//Turn off all LEDS
	CLeds::is().AllOff();
	
	Awake = false;
	
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
	
	Awake = true;
	
}

 bool Cio::IsIgnitionOn()
 {
 	return (PINA & _BV(2)) & _BV(2);
 }

// clock interrupt - clear flag immediately to resume count
//PD2 (INT0/I26)
ISR(INT1_vect)
{
	//Cio::IgnitionChanged = true;
}

