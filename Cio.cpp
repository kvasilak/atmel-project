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
#include "CTimer.h"

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
		
		#define IGNITION_ON_BIT				2
		#define IGNITION_ON_PORT		PINA
		
		#define SOLENOID_RD_BIT			4
		#define SOLENOID_RD_PORT		PINA
		
		#define SOLENOID_RU_BIT			5
		#define SOLENOID_RU_PORT		PINA
		
		#define SOLENOID_LD_BIT			6
		#define SOLENOID_LD_PORT		PINA
		
		#define SOLENOID_LU_BIT			7
		#define SOLENOIF_LU_PORT		PINA
		
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
		
		#define REMOTE_LD_BIT		0
		#define REMOTE_LD_PORT		PINB
		
		#define REMOTE_LU_BIT		1
		#define REMOTE_PORT			PINB
		
		#define REMOTE_RD_BIT		2
		#define REMOTE_TD_PORT		PINB
		
		#define REMOTE_CAMP_BIT		3
		#define REMOTE_CAMP_PORT	PINB
		
		#define REMOTE_RU_BIT		4
		#define REMOTE_RU_PORT		PINB
		
		#define REMOTE_TRAVEL_BIT	5
		#define REMOTE_TRAVEL_PORT	PINB
		
		#define REMOTE_DOWN_BIT		6
		#define REMOTE_DOWN_PORT	PINB
		
		#define REMOTE_UP_BIT		7
		#define REMOTE_UP_PORT		PINB
		
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
		
		#define COMPRESSOR_ON_BIT	8
		#define COMPRESSOR_ON_PORT	PORTC
		
		DDRD = 0x06;
		//Port D
		//0		RX		0	Debug serial
		//1		TX		1	Debug serial
		//2		out		0	Power up
		//3		in		0	Travel
		
		//4		in		0	Calibrate
		//5		in		0	Camp
		//6		in		0	Down
		//7		in		0	up
		
		#define POWER_ON_BIT			2
		#define POWER_ON_PORT			PORTD
		
		#define TRAVEL_BUTTON_BIT		3
		#define TRAVEL_BUTTON_PORT		PIND
		
		#define CALIBRATE_BUTTON_BIT	4
		#define CALIBRATE_BUTTON_PORT	PIND
		
		#define CAMP_BUTTON_BIT			5
		#define CAMP_BUTTON_PORT		PIND
		
		#define DOWN_BUTTON_BIT			7
		#define DOWN_BUTTON_PORT		PIND
		
		#define UP_BUTTON_BIT			6
		#define UP_BUTTON_PORT			PIND
		
		
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
		
		PORTD = 0xF8;
		//Port D
		//0		RX		0	Debug serial
		//1		TX		0	Debug serial
		//2		out		0	Power up
		//3		in		1	Travel
		
		//4		in		1	Calibrate
		//5		in		1	Camp
		//6		in		1	Down
		//7		in		1	up
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
	//ButtonUp.Attach(IO_PORTD, PORTD5);
	//PushTravel.Attach(IO_PORTD, PORTD6);
	//PushCamp.Attach(IO_PORTD, PORTD7);
//#endif

#ifdef REV3_PCB	
	PushTravel.Attach(IO_PORTD, PORTD3);
	PushCalibrate.Attach(IO_PORTD, PORTD4);
	PushCamp.Attach(IO_PORTD, PORTD5);
	ButtonDown.Attach(IO_PORTD, PORTD6);
	ButtonUp.Attach(IO_PORTD, PORTD7);
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
	
	Time = CTimer::GetTick();
}

//update switch states and debounce
void Cio::Run()
{
	//debounce inputs
	ButtonDown.Update();
	ButtonUp.Update();
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

	bool changed   = (OldRockerDown	!= ButtonDown.Level());
		 changed  |= (OldRockerUp	!= ButtonUp.Level());
	
	OldRockerDown = ButtonDown.Level();
	OldRockerUp = ButtonUp.Level();
	
	if(changed)
	{

		CSerial::is() << "PORTB; " << ButtonDown.Level() << ", " << ButtonUp.Level() <<"\n";
	}
	
	return changed;
}

bool Cio::OutSideRemoteChanged()
{
	static uint8_t OldPort = false;
	
	//pins 3,5,6,7
	uint8_t port = PINB & 0x1D;
	
	bool changed = OldPort != port;
	
	OldPort = port;
	
	return changed;
}

bool Cio::SteeringRemoteChanged()
{
	static uint8_t OldPort = false;
	
	//Pins 0,1,2,4
	uint8_t port = PINB & 0x17;

	bool changed = OldPort != port;

	OldPort = port;

	return changed;
}



//returns true if any camp mode button has changed state
//camp switch
//camp on steering remote
bool Cio::CampChanged()
{
	static bool OldCamp					= false;
	static bool OldInsideCamp			= false;
	 
	bool camp = (REMOTE_CAMP_PORT & _BV(REMOTE_CAMP_BIT));
 	
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
	
	bool trav = (REMOTE_TRAVEL_PORT & _BV(REMOTE_TRAVEL_BIT));

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
	if(PushCalibrate == 0)
	{
		changed = OldCal != (bool)PushCalibrate;
	}
	OldCal = (bool)PushCalibrate;
	
	return changed;

}

void Cio::RockerSwitch()
{
	if(!ButtonUp.Level())
	{
		CSerial::is() << "ButtonUp\n";
		
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
	
	if(!ButtonDown.Level())
	{
		CSerial::is() <<  "Button Down\n";

		
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
	bool RemoteLeftDown  = PINB & _BV(0);
	bool RemoteLeftUp    = PINB & _BV(1);		
    bool RemoteRightDown = PINB & _BV(2);
	bool RemoteRightUp   = PINB & _BV(4);		
	
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
	bool RemoteUp = PINB & _BV(7);
	bool RemoteDown = PINB & _BV(6);
	
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


bool Cio::CampSwitches()
{
	//bool SteeringCamp = PINB & _BV(3);
	bool pressed =  false;
	
	if(!PushCamp.Level() )//|| SteeringCamp)
	{
		pressed  = true;
	}

	return pressed;
}

bool Cio::TravelSwitches()
{
	//bool SteeringTravel = REMOTE_TRAVEL_PORT & _BV(REMOTE_TRAVEL_BIT);
	bool pressed = false;
	
	if(!PushTravel.Level() )//|| SteeringTravel)
	{
		pressed = true;
	}

	return pressed;
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
	SOLENOID_RU_PORT |= _BV(SOLENOID_RU_BIT);
	CLeds::is().RightFillOn();
}

void Cio::RightFillOff()
{
	SOLENOID_RU_PORT &= ~_BV(SOLENOID_RU_BIT);
	CLeds::is().RightFillOff();
}

void Cio::RightDumpOn()
{
	SOLENOID_RD_PORT |= _BV(SOLENOID_RD_BIT);
	CLeds::is().RightDumpOn();
}

void Cio::RightDumpOff()
{
	SOLENOID_RD_PORT &= ~_BV(SOLENOID_RD_BIT);
	CLeds::is().RightDumpOff();
}


void Cio::LeftFillOn()
{
	SOLENOIF_LU_PORT |= _BV(SOLENOID_LU_BIT);
	CLeds::is().LeftFillOn();
}

void Cio::LeftFillOff()
{
	SOLENOIF_LU_PORT &= ~_BV(SOLENOID_LU_BIT);
	CLeds::is().LeftFillOff();
}

void Cio::LeftDumpOn()
{
	SOLENOID_LD_PORT |= _BV(SOLENOID_LD_BIT);
	CLeds::is().LeftDumpOn();
}

void Cio::LeftDumpOff()
{
	SOLENOID_LD_PORT &= ~_BV(SOLENOID_LD_BIT);
	CLeds::is().LeftDumpOff();
}


bool Cio::IsHolding()
{
	bool hold = true;
	
	if(ButtonUp.Level() || ButtonDown.Level())
	{
		hold = false;
	}
	
	return hold;
}

void Cio::PowerOn()
{
	//Turn on 5v, PA2
	POWER_ON_PORT |= _BV(POWER_ON_BIT);
}

void Cio::PowerOff()
{
	//Turn off 5v, PA2
	POWER_ON_PORT &= ~_BV(POWER_ON_BIT);
}

void Cio::CompressorOn()
{
	//Turn on Air compressor enable, PA3
	COMPRESSOR_ON_PORT |= _BV(COMPRESSOR_ON_BIT);
}

void Cio::CompressorOff()
{
	COMPRESSOR_ON_PORT &= ~_BV(COMPRESSOR_ON_BIT);
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
	CLeds::is().Dim(2);
	
	CLeds::is().ActiveOn();
	
	Awake = true;
	
}

 bool Cio::IsIgnitionOn()
 {
	 uint8_t port = IGNITION_ON_PORT;
	 
 	return (port & _BV(IGNITION_ON_BIT)) == _BV(IGNITION_ON_BIT);
 }

// clock interrupt - clear flag immediately to resume count
//PD2 (INT0/I26)
ISR(INT1_vect)
{
	//Cio::IgnitionChanged = true;
}

