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
	bool SteeringRemoteChanged(void);
	bool CampChanged(void);
	bool TravelChanged(void);
	bool CalibrateChanged(void);
	bool UpDownChanged(void);

//What to do
	void RockerSwitch(void);
	void OutsideRemote(void);
	void SteeringRemote(void);

	bool CampSwitches(void);
	bool TravelSwitches(void);
	bool CalibrateSwitch(void);
	
	bool IsHolding(void);
	
	FillStates LeftState;
	FillStates RightState;

	void PowerOn(void);
	void PowerOff(void);
	void UpdateButtons(void);
	
	void ResetButtons(void) 
	{
		FillPressed = false;
		DumpPressed = false;
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

	Debounced ButtonUp;
	Debounced ButtonDown;
	Debounced PushCamp;
	Debounced PushTravel;
	Debounced PushCalibrate;
	
	void Direction(void);
	void Pullups(void);
	void EnableIgnGPIOInterrupt(void);

	bool FillPressed;
	bool DumpPressed;
	
	uint32_t Time;
    bool BlinkTravelEn;
    uint32_t Blink;

}; //Cio

#endif //__CIO_H__
