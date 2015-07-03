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
	Hold
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

	void Init();
	void Run();
	
	void AllOff();
	void Right(eValveStates s);
	void Left(eValveStates s);
	
	void RightFillOn();
	void RightFillOff();
	void RightDumpOn();
	void RightDumpOff();
	void LeftFillOn();
	void LeftFillOff();
	void LeftDumpOn();
	void LeftDumpOff();
	
	void CompressorOn();
	void CompressorOff();

	//whats changed
	bool RockerChanged();
	bool OutSideRemoteChanged();
	bool SteeringRemoteChanged();
	bool CampChanged();
	bool TravelChanged();
	bool CalibrateChanged();

//What to do
	void RockerSwitch();
	void OutsideRemote();
	void SteeringRemote();

	void CampSwitches();
	void TravelSwitches();
	
	bool IsHolding();
	
	FillStates LeftState;
	FillStates RightState;

	void PowerOn();
	void PowerOff();
	
	void FillReset() 
	{
		FillPressed = false;
		DumpPressed = false;
	}

	void Sleep();
	void Wakeup();
	bool IsIgnitionOn();
	static volatile bool IgnitionChanged;
	
private:
	Cio();
	Cio& operator=( const Cio &c );

	Debounced RockerUp;
	Debounced RockerDown;
	Debounced PushCamp;
	Debounced PushTravel;
	Debounced PushCalibrate;
	
	void Direction();
	void Pullups();
	void EnableIgnOnInterrupt();

	bool FillPressed;
	bool DumpPressed;

}; //Cio

#endif //__CIO_H__
