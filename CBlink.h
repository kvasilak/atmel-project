/* 
* CBlink.h
*
* Created: 11/4/2014 9:25:23 PM
* Author: keith
*/


#ifndef __CBLINK_H__
#define __CBLINK_H__


class CBlink
{
//variables
public:
protected:
private:

//functions
public:
        //Constructor with the last parameter set to false as default.
        CBlink(volatile uint8_t* port, uint8_t pin, bool initiallyLit=false);
        
        bool IsLedLit() { return m_isLedLit; }
        void SetLitState(bool state);
        void ToggleLitState();
protected:
private:
        bool m_isLedLit;
        
        volatile uint8_t* m_port;
        volatile uint8_t* m_ddr_port;
        uint8_t  m_pin;

}; //CBlink

#endif //__CBLINK_H__
