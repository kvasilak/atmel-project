/* 
* nvm.h
*
* Created: 4/3/2015 11:39:56 PM
* Author: keith
*/


#ifndef __NVM_H__
#define __NVM_H__

#include <avr/eeprom.h>

class nvm
{

public:
	static nvm &is()
	{
		static nvm a;
		return a;
	}
	
	void init();
	void Save();
	 	
	void SetLeftTravel(uint16_t m)  {LeftTravel = m;};
	void SetRightTravel(uint16_t m) {RightTravel = m;};
	 	
	void SetCampX(uint16_t m)	{CampX = m;};
	void SetCampY(uint16_t m)	{CampY = m;};
	void SetCampZ(uint16_t m)	{CampZ = m;};

	uint16_t GetLeftTravel()	{return LeftTravel;};
	uint16_t GetRightTravel()	{return RightTravel;};
	 	
	uint16_t GetCampX()			{return CampX;};
	uint16_t GetCampY()			{return CampY;};
	uint16_t GetCampZ()			{return CampZ;}
			 
			 
protected:
private:
	nvm();
	nvm( const nvm &c );
	nvm& operator=( const nvm &c );

	
	//Ram versions
	uint8_t  MagicNumber;
	
	uint16_t LeftTravel;
	uint16_t RightTravel;
	
	uint16_t CampX;
	uint16_t CampY;
	uint16_t CampZ;
	
}; //nvm

#endif //__NVM_H__
