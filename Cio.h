/* 
* Cio.h
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/


#ifndef __CIO_H__
#define __CIO_H__

#include "Debounced.h"

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

//whats changed
bool ManualChanged();
bool CampChanged();
bool TravelChanged();

//What to do


private:
Cio();
Cio& operator=( const Cio &c );

Debounced RockerUp;
Debounced RockerDown;
Debounced PushCamp;
Debounced PushTravel;

}; //Cio

#endif //__CIO_H__
