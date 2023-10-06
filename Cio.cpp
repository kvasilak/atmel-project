/* 
* Cio.cpp
*
* Created: 2/15/2015 /2023
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
RightFillPressed(false),
RightDumpPressed(false),
LeftFillPressed(false),
LeftDumpPressed(false),
BlinkTravelEn(false),
LeftSpeed(0),
RightSpeed(0),
SpeedTimeRight(0),
SpeedTimeLeft(0),
FillLeft(false),
FillRight(false),
DumpLeft(false),
DumpRight(false)
//EL2CompCount(0)
{
} //Cio

//setup IO lines
//Read using  PINx
//Write using PORTx
//DDR 1 for output 0 for input
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
		
	DDRB = 0xC0; //F0;
		
	//port B
	//0		in		0	Remote LD
	//1		in		0	Remote LU
	//2		in		0	Remote RD
	//3		in		0	Remote RU
		
	//4		in 		0	Btn Left down
	//5		in 		0	Btn Left up
	//6		in 		0	NC
	//7		in		0	NC

	#define REMOTE_RD_BIT		0 
	#define REMOTE_LD_BIT		1 
	#define REMOTE_RU_BIT		2
	#define REMOTE_LU_BIT		3
		
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
		
	DDRD = 0x00;
	//Port D
	//0		RX		0	Debug serial
	//1		TX		1	Debug serial
	//2		NC

	//3		in		0	Btn Travel
		
	//4		in		0	Btn Calibrate
	//5		in		0	Btn Camp
	//6		in		0	Btn Right Down
	//7		in		0	Btn Right Up
		
	#define BUTTON_TRAVEL_BIT		3
	#define BUTTON_CALIBRATE_BIT	4
	#define BUTTON_CAMP_BIT			5
	#define BUTTON_RD_BIT			6
	#define BUTTON_RU_BIT			7

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
	//0		in		0	Remote LD
	//1		in		0	Remote LU
	//2		in		0	Remote RD
	//3		in		0	Remote RU
	
	//4		in 		0	Btn Left down
	//5		in 		0	Btn Left up
	//6		in 		0	NC
	//7		in		0	NC

	//PORTC = 0x00;
	//Port C
	//0		SCL
	//1		SDA
	//2		JTAG
	//3		JTAG
	//4		JTAG
	//5		JTAG
	//6	    out		0 spare
	//7		out		1 Compressor on
		
	PORTD = 0xF8;
	//Port D
	//0		RX		0	Debug serial
	//1		TX		1	Debug serial
	//2		NC
	//3		in		0	Btn Travel
	
	//4		in		0	Btn Calibrate
	//5		in		0	Btn Camp
	//6		in		0	Btn Right Down
	//7		in		0	Btn Right Up
}



void Cio::Init()
{
	Direction();
	Pullups();
	
	//use to debounce switch inputs
	PushTravel.Attach     (IO_PORTD, PORTD3);
	PushCalibrate.Attach  (IO_PORTD, PORTD4);
	PushCamp.Attach       (IO_PORTD, PORTD5);
	ButtonRightDown.Attach(IO_PORTD, PORTD6);
	ButtonRightUp.Attach  (IO_PORTD, PORTD7);
	ButtonLeftDown.Attach (IO_PORTB, PORTB4);
	ButtonLeftUp.Attach   (IO_PORTB, PORTB5);
	

	//setup initial values
	CampChanged();
	TravelChanged();
	RockerChanged();
	OutSideRemoteChanged();
	CalibrateChanged();
	
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
	ButtonRightDown.Update();
	ButtonRightUp.Update();
	ButtonLeftUp.Update();
	ButtonLeftDown.Update();
	PushTravel.Update();
	PushCamp.Update();
	PushCalibrate.Update();
    
    
}

//PCInt2 on pin 35
void Cio::EnableIgnGPIOInterrupt()
{
	//PCI0
	//PA2 PCint 2, Ign on
	//PB0 PCint 8, Remote LD
	PCMSK0 = _BV(PCINT2) | _BV(PCINT8);  
	
	//PCI1
	//PB1 PCint 9, Remote LU
	//PB2 PCint 10, Remote RD
	//PB3 PCint 11, Remote RU
	
	//PB4 PCint 12, Btn L Down
	//PB5 PCint 13, Btn R Up
	
	PCMSK1 = _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT11) | _BV(PCINT12) | _BV(PCINT13);

	//PCI3
	//PD3 PCint 27, Travel
	//PD4 PCint 28, Calibrate
	//PD5 PCint 29, Camp

	//PD6 PCint 30, Btn R Down
	//PD7 PCint 31, Btn R Up
	PCMSK3 = _BV(PCINT27) | _BV(PCINT28) | _BV(PCINT29) | _BV(PCINT30) | _BV(PCINT31) ;

	//Enable interrupts
	PCICR = _BV(PCIE0) | _BV(PCIE1) | _BV(PCIE3) ;
}

//now a latching pushbutton not a rocker
bool Cio::RockerChanged()
{
	static bool OldRockerRightDown = false;
	static bool OldRockerRightUp = false;
	static bool OldRockerLeftDown = false;
	static bool OldRockerLeftUp = false;

	bool changed   = (OldRockerRightDown	!= ButtonRightDown.Level());
		 changed  |= (OldRockerRightUp	!= ButtonRightUp.Level());
		 changed  |= (OldRockerLeftDown	!= ButtonLeftDown.Level());
		 changed  |= (OldRockerLeftUp	!= ButtonLeftUp.Level());
	
	OldRockerRightDown = ButtonRightDown.Level();
	OldRockerRightUp = ButtonRightUp.Level();
	OldRockerLeftDown = ButtonLeftDown.Level();
	OldRockerLeftUp = ButtonLeftUp.Level();
	
	if(changed)
	{

		CSerial::is() << "PORTB; " << ButtonRightDown.Level() << ", " << ButtonRightUp.Level() << ", " << ButtonLeftDown.Level() << ", " << ButtonLeftUp.Level() <<"\n";
	}
	
	return changed;
}

bool Cio::OutSideRemoteChanged()
{
	static uint8_t OldPort = false;
	uint8_t port = PINB & 0x0F; //17;
	bool changed = OldPort != port;
	OldPort = port;
	
	return changed;
}

//returns true if camp mode button has changed state
//camp switch
bool Cio::CampChanged()
{
	static bool OldCamp					= false;
	bool Changed	= OldCamp			!= (bool)PushCamp;	
	OldCamp		= (bool)PushCamp;

	return Changed;

}

//returns true if the travel mode buttons have changed state
//from not pressed to pressed
bool Cio::TravelChanged()
{
	static bool OldTravel = false;
	bool Changed	= OldTravel	!= (bool)PushTravel;
	OldTravel		= PushTravel.Level();

	return Changed;
}

bool Cio::CalibrateChanged()
{
	static bool OldCal	= false;
	bool changed = OldCal != PushCalibrate.Level();
	OldCal = PushCalibrate.Level();

	return changed;

}


void Cio::RockerSwitch()
{
	static bool BRUChanged = false;
	static bool BLUChanged = false;
	static bool BRDChanged = false;
	static bool BLDChanged = false;
	
	if(BRUChanged != ButtonRightUp.Level())
	{
		BRUChanged = ButtonRightUp.Level();
		
		if(!ButtonRightUp.Level())//is button pressed?
		{
			if(RightFillPressed) //filling
			{
				//button pressed again, turn off filling
				RightFillOff();
				RightFillPressed = false;
			}
			else
			{
				RightDumpOff(); //dont have dump and fill on at same time
				RightDumpPressed = false;
				RightFillOn();
				RightFillPressed = true;
			}
		}
	}
	
	if(BRDChanged != ButtonRightDown.Level())
	{
		BRDChanged = ButtonRightDown.Level();
		
		if(!ButtonRightDown.Level())
		{
			//toggle on button press
			if(RightDumpPressed)
			{
				RightDumpOff();
				RightDumpPressed = false;
			}
			else
			{
				RightFillOff();
				RightFillPressed = false;
				RightDumpOn();
				RightDumpPressed = true;
			}
		}
	}

	if(BLUChanged != ButtonLeftUp.Level())
	{
		BLUChanged = ButtonLeftUp.Level();
	
		if(!ButtonLeftUp.Level())
		{
			//toggle on button press
			if(LeftFillPressed)
			{
				LeftFillOff();
				LeftFillPressed = false;
			}
			else
			{
				LeftDumpOff();
				LeftDumpPressed = false;
				LeftFillOn();
				LeftFillPressed = true;
			}
		}
	}

	if(BLDChanged != ButtonLeftDown.Level())
	{
		BLDChanged = ButtonLeftDown.Level();
		
		if(!ButtonLeftDown.Level())
		{
			//toggle on button press
			if(LeftDumpPressed)
			{
				LeftDumpOff();
				LeftDumpPressed = false;
			}
			else
			{
				LeftFillOff();
				LeftFillPressed = false;
				LeftDumpOn();
				LeftDumpPressed = true;
			}
		}
	}
}

void Cio::OutsideRemote()
{
    static bool OldLeftUp = false;
    static bool OldRightUp = false;
    static bool OldLeftDown = false;
    static bool OldRightDown = false;
  
	bool RemoteLeftDown  = PINB & _BV(REMOTE_LD_BIT);
	bool RemoteLeftUp    = PINB & _BV(REMOTE_LU_BIT);		
    bool RemoteRightDown = PINB & _BV(REMOTE_RD_BIT);
	bool RemoteRightUp   = PINB & _BV(REMOTE_RU_BIT);	
		
	if(RemoteLeftUp != OldLeftUp)
    {
        if(RemoteLeftUp)
        {
            LeftFillOn();
            OldLeftUp = true;
        }
        else
        {
            LeftFillOff();
            OldLeftUp = false;
        }
    }    
		
    if(RemoteRightUp != OldRightUp)
    {
	    if(RemoteRightUp)
	    {
		    RightFillOn();
            OldRightUp = true;
	    }
	    else 
        {
		    RightFillOff();
            OldRightUp = false;
	    }
    }    
	
    if(RemoteLeftDown != OldLeftDown)	
    {
	    if(RemoteLeftDown)
	    {
			LeftDumpOn();
            OldLeftDown = true;
	    }
	    else
	    {
		    LeftDumpOff();
            OldLeftDown = false;
	    }
    }    
		
	if(RemoteRightDown!= OldRightDown)
	{
        if(RemoteRightDown)
        {
			RightDumpOn();
            OldRightDown = true;
	    }
	    else
	    {
		    RightDumpOff();
            OldRightDown = false;
	    }
    }        
}

bool Cio::CampSwitches()
{
	return PushCamp.Level();
}

bool Cio::TravelSwitches()
{
	return PushTravel.Level();
}

bool Cio::CalibrateSwitch()
{
	return PushCalibrate.Level();
}

void Cio::AllOff()
{
	RightFillOff();
	RightDumpOff();
	LeftFillOff();
	LeftDumpOff();
    
     RightFillPressed  = false;
     RightDumpPressed  = false;
	 LeftFillPressed   = false;
	 LeftDumpPressed   = false;
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
    FillRight = true;
    
    SpeedTimeRight = CTimer::GetTick();
    oldright= CADC::is().GetRightAvgHeight();
    RightSpeed = 10; // anything >1
	
	SOLENOID_RU_PORT |= _BV(SOLENOID_RU_BIT);
	CLeds::is().RightFillOn();
}

void Cio::RightFillOff()
{
    FillRight = false;
	
	SOLENOID_RU_PORT &= ~_BV(SOLENOID_RU_BIT);
	CLeds::is().RightFillOff();
}

void Cio::RightDumpOn()
{
    DumpRight = true;
    
    SpeedTimeRight = CTimer::GetTick();
    oldright= CADC::is().GetRightAvgHeight();
    RightSpeed = 10; // anything >1
    
	SOLENOID_RD_PORT |= _BV(SOLENOID_RD_BIT);
	CLeds::is().RightDumpOn();
}

void Cio::RightDumpOff()
{
    DumpRight = false;
	
	SOLENOID_RD_PORT &= ~_BV(SOLENOID_RD_BIT);
	CLeds::is().RightDumpOff();
}


void Cio::LeftFillOn()
{
    FillLeft = true;
    
    SpeedTimeLeft = CTimer::GetTick();
    oldleft= CADC::is().GetLeftAvgHeight();
    LeftSpeed = 10; // anything >1
	
	SOLENOID_LU_PORT |= _BV(SOLENOID_LU_BIT);
	CLeds::is().LeftFillOn();
}

void Cio::LeftFillOff()
{
    FillLeft = false;
	
	SOLENOID_LU_PORT &= ~_BV(SOLENOID_LU_BIT);
	CLeds::is().LeftFillOff();
}

void Cio::LeftDumpOn()
{
    DumpLeft = true;
    SpeedTimeLeft = CTimer::GetTick();
    oldleft= CADC::is().GetLeftAvgHeight();
    LeftSpeed = 10; // anything >1
	
	SOLENOID_LD_PORT |= _BV(SOLENOID_LD_BIT);
	CLeds::is().LeftDumpOn();
}

void Cio::LeftDumpOff()
{
    DumpLeft = false;
	
	SOLENOID_LD_PORT &= ~_BV(SOLENOID_LD_BIT);
	CLeds::is().LeftDumpOff();
}


bool Cio::IsHolding()
{
	bool hold = false;
	
	CSerial::is() << "holding? " << RightFillPressed << ", " << RightDumpPressed  << ", " << LeftFillPressed << ", " << LeftDumpPressed<< "\n";
	
	if(RightFillPressed || RightDumpPressed || LeftFillPressed || LeftDumpPressed)
	{
		hold = true;
	}
	
	return hold;
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

	//turn off unnecessary peripherals
	//minimal power savings
    power_all_disable();

	RightFillPressed = false;
	RightDumpPressed = false;
	LeftFillPressed = false;
	LeftDumpPressed = false;
	
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
	CompressorOn();
	
    CSerial::is() << "LED Init 0";
	CLeds::is().Init();
    CSerial::is() << "LED Init 1";
	CLeds::is().Init();
    CSerial::is() << "LED Init 2";
	CLeds::is().Init();
    CSerial::is() << "LED Init 3";
	CLeds::is().Init();
	
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
	if(RightFillPressed)
	{
		RightFillOn();
	}
	else if(RightDumpPressed)
	{
		RightDumpOn();
	}
	else if(LeftFillPressed)
	{
		LeftFillOn();
	}
	else if(LeftDumpPressed)
	{
		LeftDumpOn();
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
	if(PINA & _BV(IGNITION_ON_BIT))
	{
		Cio::IgnitionChanged = true;
	}

	//Left down button
	if(PINA & _BV(REMOTE_LD_BIT))
	{
		Cio::ButtonChanged = true;
	}
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
