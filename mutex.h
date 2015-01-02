/*
 * mutex.h
 *
 * Created: 1/1/2015 9:42:57 PM
 *  Author: keith
 
 To use this Mutex use this
 
 long tick;
 
 //create a local scope around the tick_count accessor
 {
	 Mutex m;
	 tick = tick_count;
 }
 if (tick >= next_stop)
 {
	 next_stop += 100;
	 do_stuff();
 }
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

class CMutex
{
	public:
	CMutex ()
	{
		sei();
	}
	
	~CMutex ()
	{
		cli();
	}
};

#endif /* MUTEX_H_ */