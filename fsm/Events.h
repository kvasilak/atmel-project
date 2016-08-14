#ifndef EVENTS_H
#define EVENTS_H

enum class eEvents
{
	NoEvent,
	TimerEvent ,
	RockerEvent,
	OutSideEvent,
	SteeringEvent,
	TravelEvent,
	CampEvent,
	CalibrateEvent,
	IgnitionOnEvent,
	IgnitionOffEvent,
	ButtonWakeEvent
};


#endif // EVENTS_H