/* 
* Cio.h
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/


#ifndef __CIO_H__
#define __CIO_H__

#include "Debounced.h"

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
	void RightFillOn();
	void RightFillOff();
	void RightDumpOn();
	void RightDumpOff();
	void LeftFillOn();
	void LeftFillOff();
	void LeftDumpOn();
	void LeftDumpOff();

	//whats changed
	bool RockerChanged();
	bool OutSideRemoteChanged();
	bool SteeringRemoteChanged();
	bool CampChanged();
	bool TravelChanged();

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
private:
	Cio();
	Cio& operator=( const Cio &c );

	Debounced RockerUp;
	Debounced RockerDown;
	Debounced PushCamp;
	Debounced PushTravel;
	
	void Direction();
	void Pullups();

}; //Cio

#endif //__CIO_H__
