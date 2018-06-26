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

#include <stdlib.h>
#include <string.h>

FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL),
LeftSide(LeftRear),
RightSide(RightRear),
Starting(true),
Start(0),
filterwait(0),
waiting(false),
HeightIndex(0),
ShortFilterLen(10),
LomgFilterLen(HEIGHTSIZE)
{
}
void FsmTravel::SetState(FilterStates_e s)
{
     static const char *StateStrs[] = {FILTER_STATES_LIST(STRINGIFY)};

     CSerial::is() << "Travel State,";
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

    LeftSide.SetSetPoint(nvm::is().GetLeftTravel());
    RightSide.SetSetPoint(nvm::is().GetRightTravel());

	LeftSide.SetLongFilter(false); 
	RightSide.SetLongFilter(false);
    
    LeftSide.NotAtHeight();
    RightSide.NotAtHeight();
	
	Starting = true;
    waiting = false;

	Start = CTimer::GetTick();
    
    SetState(FilterInit);
    
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

inline static double sqr(double x) 
{
    return x*x;
}

void FsmTravel::LeastSquares(int size, int16_t *y, int16_t *m, int16_t *b)
{
     double   sumx = 0.0;                      /* sum of x     */
     double   sumx2 = 0.0;                     /* sum of x**2  */
     double   sumxy = 0.0;                     /* sum of x * y */
     double   sumy = 0.0;                      /* sum of y     */
     double   sumy2 = 0.0;                     /* sum of y**2  */

    int32_t hi;
     int32_t ii;
     int i;
         
     for (i=0;i<size;i++)
     {
         //handle buffer wrap
         //HEIGHTSIZE == 100
         //HeightIndex == 5 ( it wrapped )
         //size = 10
                
         // HeightIndex - size +i < 0
         //i == 0; ii = 100 - 10 + 5 + 0  = 95
         //i == 1; ii = 100 - 10 + 5 + 1  = 96
         //i == 2; ii = 100 - 10 + 5 + 2  = 97
         //i == 3; ii = 100 - 10 + 5 + 3  = 98
         //i == 4; ii = 100 - 10 + 5 + 4  = 99
         
         // HeightIndex - size +i >= 0
         //i == 5; ii = 5 -10 + 5  = 0
         //i == 6; ii = 5 -10 + 6  = 1
         //i == 6; ii = 5 -10 + 7  = 2
         //i == 6; ii = 5 -10 + 8  = 3
         //i == 6; ii = 5 -10 + 9  = 4
         
         //HeightIndex == 95 ( it wrapped )
         //size = 100
                
         // HeightIndex - size +i < 0
         //i == 0; ii = 100 - 100 + 95 + 0  = 95
         //i == 1; ii = 100 - 100 + 95 + 1  = 96
         //i == 2; ii = 100 - 100 + 95 + 2  = 97
         //i == 3; ii = 100 - 100 + 95 + 3  = 98
         //i == 4; ii = 100 - 100 + 95 + 4  = 99
         
         // HeightIndex - size +i >= 0
         //i == 5; ii = 95 -100 + 5  = 0
         //i == 6; ii = 95 -100 + 6  = 1
         //i == 6; ii = 95 -100 + 7  = 2
         //i == 6; ii = 95 -100 + 8  = 3
         //i == 6; ii = 95 -100 + 9  = 4
         
         
         
        
        //if(size == HEIGHTSIZE)
        //{
            //ii = i;
        //}
        //else
        //{            
            hi = HeightIndex;
            ii = hi - size +i;
         
             if( ii < 0)
             {
                  CSerial::is() << "*\n";
                 ii = (HEIGHTSIZE - size) + hi + i;
             }
  //    }
         
        sumx  += i; //x[i];
        sumx2 += sqr(i); //x[i]);
        sumxy += i  * y[ii];
        sumy  += y[ii];
        sumy2 += sqr(y[ii]);
        
        //int f_part = (int)(sumxy);
        //int i_part = (int)((sumxy - (float)f_part) * 1000);
        
         CSerial::is().Dec();
         CSerial::is() << i << ", " << ii << ", " << hi << "\n "; // << (int16_t)(sumxy)  <<"\n";
     }

     double denom = (size * sumx2 - sqr(sumx));
     
     if (denom == 0) 
     {
         // singular matrix. can't solve the problem.
         *m = 0;
         *b = 0;
         //if (r) *r = 0;
         //return false;
     }
     
     double fm = (size * sumxy  -  sumx * sumy) / denom;
     double fb = (sumy * sumx2  -  sumx * sumxy) / denom;

     *m = fm;
     *b = fb;
     
     //if (r!=NULL) 
     //{
         //*r = (sumxy - sumx * sumy / n) /    /* compute correlation coeff */
         //sqrt((sumx2 - sqr(sumx)/n) *
         //(sumy2 - sqr(sumy)/n));
     //}

     //return true;
}

void FsmTravel::UpdateHeights(int length)
{   
    int16_t LM;
    //int16_t RM;
    
    int hl = 500; //CADC::is().GetLeftAvgHeight();
    int hr = CADC::is().GetRightAvgHeight();
    
    //add new measurements to arrays
    LeftHeights[HeightIndex] = hl;
    RightHeights[HeightIndex] = hr;

    //Calc least squares fit to each side
    LeastSquares(length, LeftHeights, &LM, &hl);
    //LeastSquares(length, RightHeights, &RM, &hr);
    
    if(++HeightIndex > HEIGHTSIZE) 
    {
        HeightIndex = 0;
    }
        
    LeftHeight = hl;
    RightHeight = hr;
    
    int lh = hl;
    int rh = hr;
    int lch = nvm::is().GetLeftTravel();
    int rch = nvm::is().GetRightTravel();
        
    int lerr = lh - lch;
    int rerr = rh - rch;
    
    CSerial::is().Dec();
    CSerial::is() << (int16_t)LeftHeight << ", " << (int16_t)RightHeight << ": " << "err " << lerr << ", " << rerr << "\n";
    CSerial::is().Hex();

}

void FsmTravel::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
            switch(FilterState)
            {
                //Load the filter w ShortFilterLen samples
                case FilterInit:
                    if(HeightIndex < 100) //ShortFilterLen)
                    {
                        //Heights will be zero 
                        UpdateHeights(0);
                    }
                    else
                    {
                        SetState(FilterStart); 
                    }
                    break;
                case FilterStart:
                    if(LeftSide.IsAtHeight(LeftHeight) && RightSide.IsAtHeight(RightHeight) )
                    {
                        filterwait = CTimer::GetTick();
                        SetState(FilterWait);
                    }
                    else
                    {
                        UpdateHeights(100);
                    }
                    break;
                case FilterWait:
                     if(CTimer::IsTimedOut(filterwait, 1000))
                     {
                        CSerial::is() << " *******Setting long travel Filter ******\r\n";
                        LeftSide.SetLongFilter(true);
                        RightSide.SetLongFilter(true);
                         
                        Cio::is().BlinkTravel(false);
                        CLeds::is().TravelOn();
                     
                         SetState(FilterLong);
                     }     
                     else
                     {
                         UpdateHeights(50); //ShortFilterLen);
                         
                         if((LeftSide.IsAtHeight(LeftHeight) == false) || (RightSide.IsAtHeight(RightHeight) == false) )
                         {
                             //restart timer
                             filterwait = CTimer::GetTick();
                             
                              //SetState(FilterStart);
                         }
                     }                
                    break;
                case FilterLong:
                    UpdateHeights(100);
                break;
            }
            
            //run travel FSM
            LeftSide.Run(LeftHeight);
            RightSide.Run(RightHeight);
            
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
                LeftSide.NotAtHeight();
                RightSide.NotAtHeight();
        
                //LeftSide.SetLongFilter(false);
                //RightSide.SetLongFilter(false);
                
                SetState(FilterInit);
            
                Starting = true;
                CSerial::is() << " FsmTravel::Ignition On\r\n";
            }                        
            break;

			//let the manual height state handle it
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