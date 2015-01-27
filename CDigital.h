/* 
* CDigital.h
*
* Digital IO functions
*
* Created: 1/23/2015 6:37:52 PM
* Author: keith
*/


#ifndef __CDIGITAL_H__
#define __CDIGITAL_H__


class CDigital
{
/*outputs
PA2	power up
PA3	compressor

PB4 right fill
PB5 right dump
PB6 left fill
PB7 left dump

inputs
switches
PD2 calibrate
PD3 ignition
PD4 down
PD5 up
PD6 travel
PD7 camp


Steering remote
PB0 up
PB1 down
PB2 travel
PB3 camp
PC7 valid

outside remote
PA4 right up
PA5 right down
PA6 Left up
PA7 left down
PC6 valid




*/

//functions
public:
	CDigital();
	~CDigital();
protected:
private:
	CDigital( const CDigital &c );
	CDigital& operator=( const CDigital &c );

}; //CDigital

#endif //__CDIGITAL_H__
