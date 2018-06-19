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
waiting(false),
HeightIndex(0)
{
}

void FsmTravel::OnEntry()
{
	CLeds::is().TravelOn();
    Cio::is().BlinkTravel(true);
	LeftSide.Init(LeftRear);
	RightSide.Init(RightRear);

    LeftSide.SetSetPoint(nvm::is().GetLeftTravel());
    RightSide.SetSetPoint(nvm::is().GetRightTravel());

	LeftSide.SetLongFilter(false); 
	RightSide.SetLongFilter(false);
    
    LeftSide.AtHeight(false);
    RightSide.AtHeight(false);
	
	Starting = true;
    waiting = false;

	Start = CTimer::GetTick();
    
    FilterState = FilterStart;
    
    LeftHeight = CADC::is().GetLeftAvgHeight();
    RightHeight = CADC::is().GetRightAvgHeight();
    
    //init height arrays
    for(int i=0;i<HEIGHTSIZE; i++)
    {
        LeftHeights[i] = LeftHeight;
        RightHeights[i] = RightHeight;
    }        

    CSerial::is().Dec();
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
	CSerial::is() << "Travel Cal vals; Left, " << nvm::is().GetLeftTravel() << ", Right, " << nvm::is().GetRightTravel() << "\n";
    CSerial::is().Hex();
}

inline static float sqr(float x) 
{
    return x*x;
}

void FsmTravel::LeastSquares(int size, int16_t *x, int16_t *m, int16_t *b)
{
     float   sumx = 0.0;                      /* sum of x     */
     float   sumx2 = 0.0;                     /* sum of x**2  */
     float   sumxy = 0.0;                     /* sum of x * y */
     float   sumy = 0.0;                      /* sum of y     */
     float   sumy2 = 0.0;                     /* sum of y**2  */

     for (int i=0;i<size;i++)
     {
         sumx  += x[i];
         sumx2 += sqr(x[i]);
         sumxy += x[i] * i; //y[i];
         sumy  += i; //y[i];
         sumy2 += sqr(i); //y[i]);
     }

     float denom = (size * sumx2 - sqr(sumx));
     
     if (denom == 0) 
     {
         // singular matrix. can't solve the problem.
         *m = 0;
         *b = 0;
         //if (r) *r = 0;
         //return false;
     }

    // *m = (n * sumxy  -  sumx * sumy) / denom;
     *b = (sumy * sumx2  -  sumx * sumxy) / denom;
     
     //if (r!=NULL) 
     //{
         //*r = (sumxy - sumx * sumy / n) /    /* compute correlation coeff */
         //sqrt((sumx2 - sqr(sumx)/n) *
         //(sumy2 - sqr(sumy)/n));
     //}

     //return true;
}

void FsmTravel::UpdateHeights(void)
{   
    int16_t LM;
    int16_t RM;
    
    //add new measurements to arrays
    LeftHeights[HeightIndex] = CADC::is().GetLeftAvgHeight();
    RightHeights[HeightIndex] = CADC::is().GetRightAvgHeight();
    
    if(HeightIndex++ > HEIGHTSIZE) {HeightIndex = 0;}
        
    //Calc least squares fit to each side
    LeastSquares(HEIGHTSIZE, LeftHeights, &LM, &LeftHeight);
    LeastSquares(HEIGHTSIZE, RightHeights, &RM, &RightHeight);
}

void FsmTravel::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
        
            UpdateHeights();
            
			//run travel FSM
			LeftSide.Run(LeftHeight);
			RightSide.Run(RightHeight);
			
            
            switch(FilterState)
            {
                case FilterStart:
                    if(LeftSide.AtHeight() && RightSide.AtHeight() )
                    {
                        filterwait = CTimer::GetTick();
                        FilterState = FilterWait;
                    }
                    break;
                case FilterWait:
                     if(CTimer::IsTimedOut(filterwait, 2000))
                     {
                        CSerial::is() << " *******Setting long travel Filter ******\r\n";
                        LeftSide.SetLongFilter(true);
                        RightSide.SetLongFilter(true);
                         
                        Cio::is().BlinkTravel(false);
                        CLeds::is().TravelOn();
                     
                         FilterState = FilterLong;
                     }     
                     else
                     {
                         if((LeftSide.AtHeight() == false) || (RightSide.AtHeight() == false) )
                         {
                             //CSerial::is() << "Long Filter reset\n";
                             //restart timer
                             filterwait = CTimer::GetTick();
                             
                              FilterState = FilterStart;
                         }
                     }                
                    break;
                case FilterLong:
                     if((LeftSide.AtHeight() == false) || (RightSide.AtHeight() == false) )
                     {
                        //long filter turned off in Corner class                    
                        FilterState = FilterStart;
                     }
                break;
            }
			break;
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::SteeringEvent:
			Cio::is().SteeringRemote();
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP);
		break;
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL_CALIBRATE);
			break;
        case eEvents::ButtonWakeEvent:
        case eEvents::IgnitionOnEvent:
            if( !Cio::is().Awake)
            {
                Cio::is().Awake = true;
                Cio::is().Wakeup();
            
                //Cio::is().ButtonWake = false;
                
                CLeds::is().TravelOn();
                        
                //reset so we get back to ride height quickly
                LeftSide.AtHeight(false);
                RightSide.AtHeight(false);
        
                LeftSide.SetLongFilter(false);
                RightSide.SetLongFilter(false);
            
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