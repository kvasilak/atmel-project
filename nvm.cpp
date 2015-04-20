/* 
* nvm.cpp
*
* Created: 4/3/2015 11:39:56 PM
* Author: keith
*/


#include "nvm.h"

//EEPROM addresses
//not allowed in the header
static uint8_t  EEMEM eeMagicNumber;

static uint16_t EEMEM eeLeftTravel;
static uint16_t EEMEM eeRightTravel;

static uint16_t EEMEM eeCampX;
static uint16_t EEMEM eeCampY;
static uint16_t EEMEM eeCampZ;

static const uint8_t Majik = 0x42;

// default constructor
nvm::nvm()
{
} //nvm

void nvm::init()
{
	MagicNumber = eeprom_read_byte (&eeMagicNumber);

	LeftTravel = eeprom_read_word( &eeLeftTravel);
	RightTravel = eeprom_read_word( &eeRightTravel);

	CampX = eeprom_read_word( &eeCampX);
	CampY = eeprom_read_word( &eeCampY);
	CampZ = eeprom_read_word( &eeCampZ);
	
	if(MagicNumber != Majik)
	{
		MagicNumber		= Majik;

		LeftTravel		= 512;
		RightTravel		= 512;

		CampX			= 0xB50; //4096 * .707 ( SIN(45) )
		CampY			= 0xb50;
		CampZ			= 0x00;
		
		Save();
	}

}

void nvm::Save()
{
	eeprom_update_byte( &eeMagicNumber, MagicNumber);

	eeprom_update_word( &eeLeftTravel, LeftTravel);
	eeprom_update_word( &eeRightTravel, RightTravel);

	eeprom_update_word( &eeCampX, CampX);
	eeprom_update_word( &eeCampY, CampY);
	eeprom_update_word( &eeCampZ, CampZ);
}

