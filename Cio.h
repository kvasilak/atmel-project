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
	bool UpDownChanged();

//What to do
	void RockerSwitch();
	void OutsideRemote();
	void SteeringRemote();

	bool CampSwitches();
	bool TravelSwitches();
	
	bool IsHolding();
	
	FillStates LeftState;
	FillStates RightState;

	void PowerOn();
	void PowerOff();
	
	//void ResetButtons() 
	//{
		//FillPressed = false;
		//DumpPressed = false;
	//}

	void Sleep();
	void Wakeup();
	bool IsIgnitionOn();
	static volatile bool IgnitionChanged;
	static volatile bool ButtonChanged;
	bool Awake;
	
private:
	Cio();
	Cio& operator=( const Cio &c );

	Debounced ButtonUp;
	Debounced ButtonDown;
	Debounced PushCamp;
	Debounced PushTravel;
	Debounced PushCalibrate;
	
	void Direction();
	void Pullups();
	void EnableIgnGPIOInterrupt();

	bool FillPressed;
	bool DumpPressed;
	
	uint32_t Time;

}; //Cio

#endif //__CIO_H__
