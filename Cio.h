/* 
* Cio.h
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/


#ifndef __CIO_H__
#define __CIO_H__

#include "Debounced.h"

enum class eValveStates
{
	Fill,
	Dump,
	Hold,
    None
};

enum FillStates
{
	SolenoidFilling,
	SolenoidHolding,
	SolenoidDumping
};

class Cio
{
public:
	static Cio &is()
	{
		static Cio a;
		return a;
	}

	void Init(void);
	void Run(void);
	
	void AllOff();
	void Right(eValveStates s);
	void Left(eValveStates s);
	
	void RightFillOn(void);
	void RightFillOff(void);
	void RightDumpOn(void);
	void RightDumpOff(void);
	void LeftFillOn(void);
	void LeftFillOff(void);
	void LeftDumpOn(void);
	void LeftDumpOff(void);
	
	void CompressorOn(void);
	void CompressorOff(void);

	//whats changed
	bool RockerChanged(void);
	bool OutSideRemoteChanged(void);
	bool CampChanged(void);
	bool TravelChanged(void);
	bool CalibrateChanged(void);

//What to do
	void RockerSwitch(void);
	void OutsideRemote(void);
	//void SteeringRemote(void);

	bool CampSwitches(void);
	bool TravelSwitches(void);
	bool CalibrateSwitch(void);
	
	bool IsHolding(void);
	
	FillStates LeftState;
	FillStates RightState;

	void UpdateButtons(void);
	void ResetButtons(void) 
	{
		RightFillPressed = false;
		RightDumpPressed = false;
		LeftFillPressed = false;
	    LeftDumpPressed = false;
	}

    void BlinkTravel(bool blink);
    
	void Sleep();
	void Wakeup();
	bool IsIgnitionOn(void);

	static volatile bool IgnitionChanged;
	static volatile bool ButtonChanged;
	bool Awake;
	bool ButtonWake;
	
private:
	Cio();
	Cio& operator=( const Cio &c );

	Debounced ButtonRightUp;
	Debounced ButtonRightDown;
	Debounced ButtonLeftUp;
	Debounced ButtonLeftDown;
	Debounced PushCamp;
	Debounced PushTravel;
	Debounced PushCalibrate;
	
	void Direction(void);
	void Pullups(void);
	void EnableIgnGPIOInterrupt(void);
    
    void CalcSpeed(void);
    void TravelBlink(bool blink);
    void LeftUpBlink(bool blink);
    void LeftDownBlink(bool blink);
    void RightUpBlink(bool blink);
    void RightDownBlink(bool blink);
    void CalcLeftSpeed(void);
    void CalcRightSpeed(void);

	bool RightFillPressed;
	bool RightDumpPressed;
	bool LeftFillPressed;
	bool LeftDumpPressed;
	
	uint32_t Time;
    bool BlinkTravelEn;
    uint32_t Blink;
    
    uint32_t LeftSpeed;
    uint32_t RightSpeed;
    uint32_t SpeedTimeRight;
    uint32_t SpeedTimeLeft;
    uint16_t oldleft;
    uint16_t oldright;
    
    bool FillLeft;
    bool FillRight;
    bool DumpLeft;
    bool DumpRight;

}; //Cio

#endif //__CIO_H__
