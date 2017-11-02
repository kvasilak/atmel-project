/* 
* Cio.cpp
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/

#include <avr/io.h>
#include <avr/power.h> //perihperal power control
#include <avr/sleep.h>
#include "Cio.h"
#include "CLeds.h"
#include "CSerial.h"
#include "CTimer.h"
#include "CADC.h"

//for ignition interrupt
#include <avr/interrupt.h>

//for sleep obviously
#include <avr/sleep.h>

volatile bool Cio::IgnitionChanged = false;
volatile bool Cio::ButtonChanged = false;
volatile uint8_t ispa;

// default constructor
Cio::Cio() :
Awake(false),
ButtonWake(false),
FillPressed(false),
DumpPressed(false),
BlinkTravelEn(false),
LeftSpeed(0),
RightSpeed(0),
SpeedTimeRight(0),
SpeedTimeLeft(0),
FillLeft(false),
FillRight(false),
DumpLeft(false),
DumpRight(false)
{
} //Cio

//setup IO lines
//Read using  PINx
//Write using PORTx
void Cio::Direction()
{      
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
		
	#define IGNITION_ON_BIT			2
	#define IGNITION_ON_PORT		PINA
		
	#define SOLENOID_RD_BIT			4
	#define SOLENOID_RD_PORT		PORTA
		
	#define SOLENOID_RU_BIT			5
	#define SOLENOID_RU_PORT		PORTA
		
	#define SOLENOID_LD_BIT			6
	#define SOLENOID_LD_PORT		PORTA
		
	#define SOLENOID_LU_BIT			7
	#define SOLENOID_LU_PORT		PORTA
		
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
		
	#define REMOTE_LD_BIT		1/*0*/
	#define REMOTE_LD_PORT		PINB
		
	#define REMOTE_LU_BIT		4/*1*/
	#define REMOTE_LU_PORT		PINB
		
	#define REMOTE_RD_BIT		0/*2*/
	#define REMOTE_RD_PORT		PINB
		
	#define REMOTE_CAMP_BIT		7/*3*/
	#define REMOTE_CAMP_PORT	PINB
		
	#define REMOTE_RU_BIT		2/*4*/
	#define REMOTE_RU_PORT		PINB
		
	#define REMOTE_TRAVEL_BIT	5
	#define REMOTE_TRAVEL_PORT	PINB
		
	#define REMOTE_DOWN_BIT		6
	#define REMOTE_DOWN_PORT	PINB
		
	#define REMOTE_UP_BIT		3/*7*/
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
		
	#define COMPRESSOR_ON_BIT	7
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
}

void Cio::Pullups()
{
		
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
}



void Cio::Init()
{
	Direction();
	Pullups();
	
	//use to debounce switch inputs
	PushTravel.Attach(IO_PORTD, PORTD3);
	PushCalibrate.Attach(IO_PORTD, PORTD4);
	PushCamp.Attach(IO_PORTD, PORTD5);
	ButtonDown.Attach(IO_PORTD, PORTD6);
	ButtonUp.Attach(IO_PORTD, PORTD7);
	

	//setup initial values
	CampChanged();
	TravelChanged();
	RockerChanged();
	SteeringRemoteChanged();
	OutSideRemoteChanged();
	CalibrateChanged();
	UpDownChanged();
	
    ResetButtons();

	EnableIgnGPIOInterrupt();
	
	Time = CTimer::GetTick();
    
    Blink = CTimer::GetTick();
    
    sleep_enable();
}

void Cio::TravelBlink(bool blink)
{
    if(BlinkTravelEn)
    {
        if(blink)
        {
            CLeds::is().TravelOff();
        }
        else
        {
            CLeds::is().TravelOn();
        }
    }    
}

 void Cio::LeftUpBlink(bool blink)
 {
      if(FillLeft)
      {
          if(LeftSpeed >1)
          {
               CLeds::is().LeftFillOn();
          }
          else
          {
              if(blink)
              {
                  CLeds::is().LeftFillOff();
              }
              else
              {
                  CLeds::is().LeftFillOn();
              }
          }
      }
 }
 
 void Cio::LeftDownBlink(bool blink)
 {
      if(DumpLeft)
      {
          if(LeftSpeed  >1)
          {
              CLeds::is().LeftDumpOn();
          }
          else
          {
              if(blink)
              {
                  CLeds::is().LeftDumpOff();
              }
              else
              {
                  CLeds::is().LeftDumpOn();
              }
          }
      }
 }
 
 void Cio::RightUpBlink(bool blink)
 {
      if(FillRight)
      {         
          if(RightSpeed >1)
          {
              CLeds::is().RightFillOn();
          }
          else
          {
              if(blink)
              {
                  CLeds::is().RightFillOff();
              }
              else
              {
                  CLeds::is().RightFillOn();
              }
          }
      }
 }
 
 void Cio::RightDownBlink(bool blink)
{

    if(DumpRight)
    {
        if(RightSpeed >1)
        {
            CLeds::is().RightDumpOn();
        }
        else
        {
            if(blink)
            {
                CLeds::is().RightDumpOff();
            }
            else
            {
                CLeds::is().RightDumpOn();
            }
        }
    }
}

//update switch states and debounce
void Cio::Run()
{
    static bool blinkon = true;
       
    //check if we should be moving
    //if filling or dumping IsMoving() should be true
    CalcSpeed();

    if(CTimer::IsTimedOut(250, Blink))
    {       
        
        TravelBlink(blinkon);
        LeftUpBlink(blinkon);
        LeftDownBlink(blinkon);
        RightUpBlink(blinkon);
        RightDownBlink(blinkon);
        
        blinkon = !blinkon;
        
        Blink = CTimer::GetTick();
    }
    
	//debounce inputs
	ButtonDown.Update();
	ButtonUp.Update();
	PushTravel.Update();
	PushCamp.Update();
	PushCalibrate.Update();
    
    
}

//PCInt2 on pin 35
void Cio::EnableIgnGPIOInterrupt()
{
	//PCI0
	PCMSK0 = _BV(PCINT2);  //pcint2
	
	//PCI1
	//PB0 PCint 8, Remote LD
	//PB1 PCint 9, Remote LU
	//PB2 PCint 10, Remote RD
	//PB4 PCint 12, Remote RU
	//PCMSK1 = 0x17;
	PCMSK1 = _BV(PCINT8) | _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT12);

	//PCI3
	//PD3 PCint 27, Travel
	//PD5 PCint 29, Camp
	//PD6 PCint 30, Down
	//PD7 PCint 31, Up
	//PCMSK3 = 0xE8;
	PCMSK3 = _BV(PCINT27) | _BV(PCINT29) | _BV(PCINT30) | _BV(PCINT31) ;

	//Enable interrupts
	//PCICR = 0x0B;
	PCICR = _BV(PCIE0) | _BV(PCIE1) | _BV(PCIE3) ;
}

//now a latching pushbutton not a rocker
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
	
	//pins 0,1,2,4
	uint8_t port = PINB & 0x17;
	
	bool changed = OldPort != port;
	
	OldPort = port;
	
	return changed;
}

bool Cio::SteeringRemoteChanged()
{
	static uint8_t OldPort = false;
	
	//pins 3, 6 //3,5,6,7
	uint8_t port = PINB & 0x24;//E8;

	bool changed = OldPort != port;

	OldPort = port;

	return changed;
}

//return true if the steering remote up or down button has changed
bool Cio::UpDownChanged()
{
	static bool oldup = false;
	static bool olddown = false;
	
	bool up = REMOTE_UP_PORT & _BV(REMOTE_UP_BIT);
	bool down = REMOTE_DOWN_PORT & _BV(REMOTE_DOWN_BIT);
	
	bool changed = (oldup != up) || (olddown != down);
	
	if(changed)
	{
		oldup = up;
		olddown = down;
	}
	
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
//from not pressed to pressed
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
			CSerial::is() << "fill off\n";
			LeftFillOff();
			RightFillOff();
			
			FillPressed = false;
		}
		else
		{
			//turn off and unlatch the dump button
			if(DumpPressed)
			{
				CSerial::is() << "dump off\n";
				
				LeftDumpOff();
				RightDumpOff();
				
				DumpPressed = false;
			}
			
			CSerial::is() << "fill on\n";
			
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
			CSerial::is() << "dump off\n";
			LeftDumpOff();
			RightDumpOff();
			
			DumpPressed = false;
		}
		else
		{
			//turn off and unlatch the fill button
			if(FillPressed)
			{
				CSerial::is() << "fill off\n";
				
				LeftFillOff();
				RightFillOff();
				
				FillPressed = false;
			}
			
			CSerial::is() << "dump on\n";
			
			if(CADC::is().LeftHeightOK())
			{
				LeftDumpOn();
			}
		
			if(CADC::is().RightHeightOK())
			{
				RightDumpOn();
			}
			
			DumpPressed = true;
		}
	}

}

void Cio::OutsideRemote()
{
	bool RemoteLeftDown  = REMOTE_LD_PORT & _BV(REMOTE_LD_BIT);
	bool RemoteLeftUp    = REMOTE_LU_PORT & _BV(REMOTE_LU_BIT);		
    bool RemoteRightDown = REMOTE_RD_PORT & _BV(REMOTE_RD_BIT);
	bool RemoteRightUp   = REMOTE_RU_PORT & _BV(REMOTE_RU_BIT);		
	
	//FillPressed = false;
	//DumpPressed = false;
	
	if(RemoteLeftUp)
	{
		LeftFillOn();
		FillPressed = true;
	}
	else
	{
		LeftFillOff();
		FillPressed = false;
	}
		
	if(RemoteRightUp)
	{
		RightFillOn();
		FillPressed = true;
	}
	else
	{
		RightFillOff();
		FillPressed = false;
	}
		
	if(RemoteLeftDown)
	{
		if(CADC::is().LeftHeightOK())
		{
			LeftDumpOn();
			DumpPressed = true;
		}
	}
	else
	{
		if(CADC::is().RightHeightOK())
		{
			LeftDumpOff();
			DumpPressed = false;
		}
	}
		
	if(RemoteRightDown)
	{
		if(CADC::is().RightHeightOK())
		{
			RightDumpOn();
			DumpPressed = true;
		}
	}
	else
	{
		RightDumpOff();
		DumpPressed = false;
	}
	
}

void Cio::SteeringRemote()
{
	bool RemoteUp = REMOTE_UP_PORT & _BV(REMOTE_UP_BIT);
	bool RemoteDown = REMOTE_DOWN_PORT & _BV(REMOTE_DOWN_BIT);
	
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
			
			if(CADC::is().LeftHeightOK())
			{
				LeftDumpOn();
			}
			
			if(CADC::is().RightHeightOK())
			{
				RightDumpOn();
			}
			
			DumpPressed = true;
		}
	}

}


bool Cio::CampSwitches()
{
	bool SteeringCamp = REMOTE_CAMP_PORT & _BV(REMOTE_CAMP_BIT);
	bool pressed =  false;
	
	if(!PushCamp.Level() || SteeringCamp)
	{
		pressed  = true;
	}

	return pressed;
}

bool Cio::TravelSwitches()
{
	bool SteeringTravel = REMOTE_TRAVEL_PORT & _BV(REMOTE_TRAVEL_BIT);
	bool pressed = false;
	
	if(!PushTravel.Level() || SteeringTravel)
	{
		pressed = true;
	}

	return pressed;
}

bool Cio::CalibrateSwitch()
{

	bool pressed = false;
	
	if(!PushCalibrate.Level() )
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
    static eValveStates last = eValveStates::None;

    if(s != last)
    {
        last = s;
        
        CSerial::is() << "*Right ";
        
	    switch(s)
	    {
		    case eValveStates::Dump:
            CSerial::is() << " Dump\n";
			    RightFillOff();
			    RightDumpOn();
		    break;
		    case eValveStates::Fill:
            CSerial::is() << " Fill\n";
			    RightDumpOff();
			    RightFillOn();
		    break;
		    case eValveStates::Hold:
            CSerial::is() << " Hold\n";
			    RightFillOff();
			    RightDumpOff();
		    break;
		    default:
			    RightFillOff();
			    RightDumpOff();
	    }
    }    
}

void Cio::Left(eValveStates s)
{
    static eValveStates last = eValveStates::None;

    if(s != last)
    {
        last = s;
        
        CSerial::is() << "*Left ";
         
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
}

void Cio::RightFillOn()
{
	//CSerial::is() << "*RightFillOn\n";
    
    FillRight = true;
    
    SpeedTimeRight = CTimer::GetTick();
    oldright= CADC::is().GetRightAvgHeight();
    RightSpeed = 10; // anything >1
	
	SOLENOID_RU_PORT |= _BV(SOLENOID_RU_BIT);
	//CLeds::is().RightFillOn();
}

void Cio::RightFillOff()
{
	//CSerial::is() << "*RightFillOff\n";
    
    FillRight = false;
	
	SOLENOID_RU_PORT &= ~_BV(SOLENOID_RU_BIT);
	CLeds::is().RightFillOff();
}

void Cio::RightDumpOn()
{
	//CSerial::is() << "*RightDumpOn\n";
    
    DumpRight = true;
    
    SpeedTimeRight = CTimer::GetTick();
    oldright= CADC::is().GetRightAvgHeight();
    RightSpeed = 10; // anything >1
    
	SOLENOID_RD_PORT |= _BV(SOLENOID_RD_BIT);
	//CLeds::is().RightDumpOn();
}

void Cio::RightDumpOff()
{
	//CSerial::is() << "*RightDumpOff\n";
    DumpRight = false;
	
	SOLENOID_RD_PORT &= ~_BV(SOLENOID_RD_BIT);
	CLeds::is().RightDumpOff();
}


void Cio::LeftFillOn()
{
	//CSerial::is() << "*LeftFillOn\n";
    
    FillLeft = true;
    
    SpeedTimeLeft = CTimer::GetTick();
    oldleft= CADC::is().GetLeftAvgHeight();
    LeftSpeed = 10; // anything >1
	
	SOLENOID_LU_PORT |= _BV(SOLENOID_LU_BIT);
	//CLeds::is().LeftFillOn();
}

void Cio::LeftFillOff()
{
	//CSerial::is() << "*LeftFillOff\n";
    
    FillLeft = false;
	
	SOLENOID_LU_PORT &= ~_BV(SOLENOID_LU_BIT);
	CLeds::is().LeftFillOff();
}

void Cio::LeftDumpOn()
{
	//CSerial::is() << "*LeftDumpOn\n";
    
    DumpLeft = true;
    SpeedTimeLeft = CTimer::GetTick();
    oldleft= CADC::is().GetLeftAvgHeight();
    LeftSpeed = 10; // anything >1
	
	SOLENOID_LD_PORT |= _BV(SOLENOID_LD_BIT);
	//CLeds::is().LeftDumpOn();
}

void Cio::LeftDumpOff()
{
	//CSerial::is() << "*LeftDumpOff\n";
    
    DumpLeft = false;
	
	SOLENOID_LD_PORT &= ~_BV(SOLENOID_LD_BIT);
	CLeds::is().LeftDumpOff();
}


bool Cio::IsHolding()
{
	bool hold = false;
	
	CSerial::is() << "holding? " << FillPressed << ", " << DumpPressed  << "\n";
	
	if(!FillPressed && !DumpPressed)
	{
		hold = true;
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
	//minimal power savings
    power_all_disable();

	FillPressed = false;
	DumpPressed = false;
	
	//Set CPU to sleep, will wake up on an ignition IRQ
    //button press or outside remote
 	cli();
 	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
   
 	// sleep_mode() has a possible race condition
 	sleep_enable();
 	sei();
 	sleep_cpu();
     
     power_all_enable();
 	sleep_disable();
}

void Cio::Wakeup()
{
	//int i=0;
	PowerOn();
	CompressorOn();
	
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
	 uint8_t port = IGNITION_ON_PORT;
	 
 	return (port & _BV(IGNITION_ON_BIT)) == _BV(IGNITION_ON_BIT);
 }


void Cio::UpdateButtons()
{
	if(FillPressed)
	{
		LeftFillOn();
		RightFillOn();
	}
	else if(DumpPressed)
	{
		LeftDumpOff();
		RightDumpOff();
	}
}

void Cio::BlinkTravel(bool blink)
{
    if( blink == true)
    {
        if(BlinkTravelEn == false)
        {
            BlinkTravelEn = true;
             Blink = CTimer::GetTick();
        }
    }
    else
    {
        BlinkTravelEn = false;
        CLeds::is().TravelOn();
    }
   
}

void Cio::CalcSpeed()
{
    //calculate speed, counts/second
    CalcRightSpeed();
    CalcLeftSpeed();
   // CSerial::is() << "speed,  " << RightSpeed << ", " << LeftSpeed << "\n";
}

void Cio::CalcRightSpeed()
{
    uint16_t right = CADC::is().GetRightAvgHeight();
    
    if(CTimer::IsTimedOut(10000, SpeedTimeRight))
    {
        if(right > oldright)
        {
            RightSpeed = right - oldright;
        }
        else
        {
            RightSpeed = oldright - right;
        }
        SpeedTimeRight = CTimer::GetTick();
        
        oldright = right;
    }    
    
    
}

void Cio::CalcLeftSpeed(void)
{
    uint16_t left = CADC::is().GetLeftAvgHeight();
    
    if(CTimer::IsTimedOut(10000, SpeedTimeLeft))
    {
        if(left > oldleft)
        {
            LeftSpeed = left - oldleft;
        }
        else
        {
            LeftSpeed = oldleft - left;
        }
        SpeedTimeLeft = CTimer::GetTick();
        
        oldleft = left;
    }
    
    
}


// ignition changed
//PA2 pc int 2
ISR(PCINT0_vect)
{
	Cio::IgnitionChanged = true;
}

//Outside remote changed
ISR(PCINT1_vect)
{
	Cio::ButtonChanged = true;
}

//panel buttons changed
ISR(PCINT3_vect)
{
	Cio::ButtonChanged = true;
}
