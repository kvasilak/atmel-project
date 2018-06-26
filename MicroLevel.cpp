/*
 * Microlevel.cpp
 *
 * Created: 11/4/2014 9:24:15 PM
 *  Author: keith
 */ 

#include "common.h"
#include "CSerial.h"
#include "CController.h"

#include "CTimer.h"
#include "CBlink.h"
#include <avr/io.h>
#include "CLeds.h"
#include "CADC.h"
#include "Cio.h"
#include "MMA8451.h"
#include "nvm.h"
#include "DataVisualizer.h"

uint16_t a = 0;
uint8_t b = 0;
uint8_t txbuf[100];
uint8_t rxbuf[100];
volatile char mcusr __attribute__ ((section (".noinit")));

#define TIMESTAMP_RES 1000 /* Timestamp resolution (1 per msec) */

//error handler that is invoked when a pure virtual function is called.
extern "C" void __cxa_pure_virtual()
{
	//reset!
	while (1);
}
// "optimization enabled, check i2c operation, I.E. LEDs and accel"

void visual_stream_init(void)
{
	adv_config_start_t start;       /* Config start packet */
	adv_config_stream_t stream;     /* Stream config packet */
	adv_config_field_t field;       /* Data field info packet */
	adv_config_end_t end;           /* Config end packet */

	/* Configuration start packet */
	start.header1     = ADV_PKT_HEADER_1;
	start.header2     = ADV_PKT_HEADER_2;
	start.length      = sizeof(adv_config_start_t);
	start.type        = ADV_PKT_CONFIG_START;
	start.num_streams = 4;
	start.crc         = 0x00; /* Not used */
	start.mark        = ADV_PKT_END;

	CSerial::is().PutBuf((uint8_t *)&start, sizeof(start));

	/* Common field values for all stream config packets */
	stream.header1  = ADV_PKT_HEADER_1;
	stream.header2  = ADV_PKT_HEADER_2;
	stream.type     = ADV_PKT_CONFIG_STREAM;
	stream.tick_res = TIMESTAMP_RES;
	stream.length   = sizeof(adv_config_stream_t);
	stream.mark     = ADV_PKT_END;
	stream.crc      = 0x00; /* Not used */

	/* Common field values for all data field descriptor config packets */
	field.header1      = ADV_PKT_HEADER_1;
	field.header2      = ADV_PKT_HEADER_2;
	field.type         = ADV_PKT_CONFIG_FIELD;
	field.length       = sizeof(adv_config_field_t);
	field.field_length = 4; /* All fields are signed 4-byte integers */
	field.format       = FIELD_FORMAT_SIGNED;
	field.mark         = ADV_PKT_END;
	field.crc          = 0x00; /* Not used */

	/* Accelerometer stream */
	stream.stream_num = 1; //ACCEL_STREAM_NUM;
	stream.num_fields = 3; //NUM_ACCEL_FIELDS;
	strcpy((char *)&(stream.stream_name), "Acceleration");
	
	/* X axis data field */
	field.stream_num = 1; //ACCEL_STREAM_NUM;
	field.field_num  = 1;
	field.min        = -2000;
	field.max        = 2000;
	strcpy((char *)&(field.units), "Milli-g");
	strcpy((char *)&(field.name), "Accel X");

	CSerial::is().PutBuf((uint8_t *)&field, sizeof(field));

	/* Y axis data field (re-use previous values except name & field number) */
	field.field_num = 2;
	strcpy((char *)&(field.name), "Accel Y");

	CSerial::is().PutBuf((uint8_t *)&field, sizeof(field));

	/* Z axis data field (re-use previous values except name & field number) */
	field.field_num = 3;
	strcpy((char *)&(field.name), "Accel Z");

	CSerial::is().PutBuf((uint8_t *)&field, sizeof(field));
		
		
/////////////////////////////////////////////////////////////////////////////
	CSerial::is().PutBuf((uint8_t *)&stream, sizeof(stream));
	
		/* Configuration end packet */
		end.header1 = ADV_PKT_HEADER_1;
		end.header2 = ADV_PKT_HEADER_2;
		end.length  = sizeof(adv_config_end_t);
		end.type    = ADV_PKT_CONFIG_END;
		end.num_streams = 4;
		end.crc     = 0x00;             /* not used */
		end.mark    = ADV_PKT_END;

		CSerial::is().PutBuf((uint8_t *)&end, sizeof(end));
}

inline static float sqr(float x)
{
    return x*x;
}

void LeastSquares(int size, int16_t *y, int16_t *m, int16_t *b)
{
    double   sumx = 0.0;                      /* sum of x     */
    double   sumx2 = 0.0;                     /* sum of x**2  */
    double   sumxy = 0.0;                     /* sum of x * y */
    double   sumy = 0.0;                      /* sum of y     */
    double   sumy2 = 0.0;                     /* sum of y**2  */

    for (int i=0;i<size;i++)
    {
        sumx  += i; //x[i];
        sumx2 += sqr(i); //x[i]);
        sumxy += i /*x[i]*/ * y[i];
        sumy  += y[i];
        sumy2 += sqr(y[i]);
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

int main(void)
{
    mcusr=MCUSR;
    
    MCUSR=0;

	CController Controller;

	Cio::is().Init();
	
	Cio::is().PowerOn();
	
	CSerial::is().Init();
	
	CSerial::is() << "\n\nMicrolevel " << __DATE__ << "\nCopyright 2016 - 2018 Keith Vasilakes\n\n";
    
    CSerial::is() << "** reset Cause " << mcusr << "\n";
	
	CTimer::Init();

	CLeds::is().Init();
	
	Controller.Init();
	
	CADC::is().Init();
	
	CMMA8451::is().Init();
	
	nvm::is().init();

    //least squares fit test
    int16_t data[] = {1, 2,4,5,10,20};
    int16_t m;
    int16_t b;
        
    LeastSquares(6, data, &m, &b);

    while(1)
    {
		Controller.Run();
    }
}