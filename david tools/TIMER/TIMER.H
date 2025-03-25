#ifndef _timer_h
#define _timer_h

#include "..\common.h"

#include <winnt.h>
#include <winbase.h>

union _time {
	unsigned int mmTimer;	//multimedia var for start time
	__int64		 pcTimer;	//performance counter var for start time
};

typedef struct _win32Time {
	__int64		 freq;			//timer frequency
	float		 resolution;	//resolution

	union _time  timeStart;		//the start time (current)

	bool		 bUsePC;		//use performance counter?

	int			 pauseCount;	//pause count
	double		 pauseTime;		//when did we pause?

	double		 delay;			//delay for elapse
	unsigned int elapseCount;	//elapse counter
} win32Time, * Pwin32Time;

typedef struct _Clock {
	unsigned int	hour;
	BYTE			min;
	BYTE			sec;

	win32Time	SecDelay;		//seconds delay
} Clock;

//DO NOT MODIFY THESE VARIABLES
extern win32Time g_time;		//global timer used for time based movement
extern double    g_timeElapse;	//global time elapse

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimeUpdate();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TimeInit(win32Time *timer, double delay);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC double TimeGetTime(win32Time *timer);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimeReset(win32Time *timer);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimePause(win32Time *timer);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimeUnpause(win32Time *timer);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC bool TimeElapse(win32Time *timer);
PUBLIC bool TimeElapse(win32Time *timer, double *tOut);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC inline double TimeGetDelay(win32Time *timer)
{ return timer->delay; }

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimerFPSBegin();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TimerFPSEnd();

//
// Clock
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void ClockInit(Clock *clock);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void ClockUpdate(Clock *clock);

#endif