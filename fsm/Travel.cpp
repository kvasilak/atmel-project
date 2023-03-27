/* 
* Travel.cpp
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/



#include "CController.h"
#include "CLeds.h"
#include "CSerial.h"

#include "Travel.h"
#include "nvm.h"
#include "CADC.h"

FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL),
LeftSide(LeftRear),
RightSide(RightRear),
Starting(true),
Start(0),
filterwait(0),
waiting(false)
{
}

void FsmTravel::SetState(FilterStates_e s)
{
     static const char *StateStrs[] = {FILTER_STATES_LIST(STRINGIFY)};

     CSerial::is() << "**Travel State,";
     CSerial::is() << StateStrs[s] ;
     CSerial::is() << "\r\n";

     FilterState = s;
}


void FsmTravel::OnEntry()
{
	CLeds::is().TravelOn();
    Cio::is().BlinkTravel(true);
	LeftSide.Init(LeftRear);
	RightSide.Init(RightRear);

	LeftSide.SetLongFilter(false, (int32_t)nvm::is().GetLeftTravel());
	RightSide.SetLongFilter(false, (int32_t)nvm::is().GetRightTravel());
    
    LeftSide.AtHeight(false);
    RightSide.AtHeight(false);
	
	Starting = true;
    waiting = false;

	Start = CTimer::GetTick();
    
    SetState(FilterStart);
	
    CSerial::is().Dec();
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
	CSerial::is() << "Travel Cal vals; Left, " << nvm::is().GetLeftTravel() << ", Right, " << nvm::is().GetRightTravel() << "\n";
    CSerial::is().Hex();
}

void FsmTravel::HandleEvent(eEvents evt)
{
     uint16_t lset;
     uint16_t rset;
     
     int32_t lh;
     int32_t rh;
     
    int32_t slh;
     int32_t srh;
     
     
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run travel FSM
            
            lset = nvm::is().GetLeftTravel();
            rset = nvm::is().GetRightTravel();
            
            lh = CADC::is().GetLeftAvgHeight();
            rh = CADC::is().GetRightAvgHeight();
            
            slh = LeftSide.slowheight;
            srh = RightSide.slowheight;
            
			LeftSide.Run(lset);
			RightSide.Run(rset);
	    
            CSerial::is().Dec();
            CSerial::is() << (int16_t)lh << ", " << (int16_t)rh << ": err " << int16_t(lh - lset) << ", " << int16_t(rh - rset);
            CSerial::is() << " ,slow: " << slh << ", " << srh << ": err " << int16_t(slh - lset) << ", " << int16_t(srh - rset) << "\n";
            CSerial::is().Hex();
		
            
            switch(FilterState)
            {
                case FilterStart:
                    if(LeftSide.AtHeight() && RightSide.AtHeight() )
                    {
                        filterwait = CTimer::GetTick();
                        SetState(FilterWait);
                    }
                    break;
                case FilterWait:
                     if(CTimer::IsTimedOut(filterwait, 2000))
                     {
                        CSerial::is() << " *******Setting long travel Filter ******\r\n";
                        LeftSide.SetLongFilter(true, (int32_t)nvm::is().GetLeftTravel());
                        RightSide.SetLongFilter(true, (int32_t)nvm::is().GetRightTravel());
                         
                        Cio::is().BlinkTravel(false);
                        CLeds::is().TravelOn();
                     
                         SetState(FilterLong);
                     }     
                     else
                     {
                         if((LeftSide.AtHeight() == false) || (RightSide.AtHeight() == false) )
                         {
                             //CSerial::is() << "Long Filter reset\n";
                             //restart timer
                             filterwait = CTimer::GetTick();
                             
                              SetState(FilterStart);
                         }
                     }                
                    break;
                case FilterLong:
                     if((LeftSide.AtHeight() == false) || (RightSide.AtHeight() == false) )
                     {
                        //long filter turned off in Corner class                    
                        SetState(FilterStart);
                     }
                break;
            }
			break;
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			if(Cio::is().CampSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_CAMP);
			}
		break;
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::CalibrateEvent:
			if(Cio::is().CalibrateSwitch())
			{
				m_SMManager.ChangeState(eStates::STATE_TRAVEL_CALIBRATE);
			}
			break;
        case eEvents::ButtonWakeEvent:
        case eEvents::IgnitionOnEvent:
            if( !Cio::is().Awake)
            {
                Cio::is().Awake = true;
                Cio::is().Wakeup();

                CLeds::is().TravelOn();
                        
                //reset so we get back to ride height quickly
                LeftSide.AtHeight(false);
                RightSide.AtHeight(false);
        
                LeftSide.SetLongFilter(false, (int32_t)nvm::is().GetLeftTravel());
                RightSide.SetLongFilter(false, (int32_t)nvm::is().GetRightTravel());
            
                Starting = true;
                CSerial::is() << " FsmTravel::Ignition On\r\n";
            }                        
            break;

			//let the manua heightstate handle it
		case eEvents::IgnitionOffEvent:
			CSerial::is() << " FsmTravel::Ignition Off\r\n";
            
            Cio::is().Sleep();

			break;
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
    Cio::is().BlinkTravel(false);
	CSerial::is() << " FsmTravel::OnExit()\r\n";
	CLeds::is().TravelOff();
}