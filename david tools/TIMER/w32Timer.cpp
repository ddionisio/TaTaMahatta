#include "timer.h"

#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( default : 4201 )
#pragma comment( lib, "winmm.lib" )

win32Time g_time={0};		//global timer used for time based movement
extern double    g_timeElapse=0;	//global time elapse

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimeUpdate()
{
	g_timeElapse = TimeGetTime(&g_time);
	TimeReset(&g_time);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TimeInit(win32Time *timer, double delay)
{
	if(!QueryPerformanceFrequency(( LARGE_INTEGER* )&timer->freq ))
	{
		//use conventional timer
		timer->bUsePC = false;
		timer->timeStart.mmTimer = timeGetTime();
	}
	else
	{
		//use Performance counter
		timer->bUsePC = true;

		//initialize timer start
		QueryPerformanceCounter(( LARGE_INTEGER* )&timer->timeStart.pcTimer);

		//get the time resolution
		timer->resolution = ( float )( 1.0/( double )timer->freq )*1000.0f;
	}

	timer->pauseCount = 0;
	timer->pauseTime = 0;

	timer->elapseCount = 0;
	timer->delay = delay;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC double TimeGetTime(win32Time *timer)
{
	if(timer->pauseCount > 0)
		return timer->pauseTime;

	__int64 timeElapsed;

	if(timer->bUsePC)
	{
		QueryPerformanceCounter(( LARGE_INTEGER* )&timeElapsed );
		timeElapsed -= timer->timeStart.pcTimer;
		return timeElapsed*timer->resolution;
	}
	
	timeElapsed = timeGetTime()-timer->timeStart.mmTimer;
	return ( double )timeElapsed;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimeReset(win32Time *timer)
{
	if(timer->bUsePC)
		QueryPerformanceCounter(( LARGE_INTEGER* )&timer->timeStart.pcTimer );
	else
		timer->timeStart.mmTimer = timeGetTime();

	timer->pauseTime = TimeGetTime(timer);

	timer->elapseCount = 0;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimePause(win32Time *timer)
{
	if(timer->pauseCount == 0)
		timer->pauseTime = TimeGetTime(timer);

	timer->pauseCount++;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimeUnpause(win32Time *timer)
{
	assert( timer->pauseCount > 0 );
	timer->pauseCount--;

	if(timer->pauseCount == 0)
	{
		if(timer->bUsePC)
		{
			__int64 time;
			QueryPerformanceCounter(( LARGE_INTEGER* )&time );
			timer->timeStart.pcTimer = time - ( __int64 )(timer->pauseTime/timer->resolution);
		}
		else
			timer->timeStart.mmTimer = (unsigned int)(timeGetTime() - timer->pauseTime);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC bool TimeElapse(win32Time *timer)
{
	if(timer->pauseCount == 0)
	{
		double time = TimeGetTime(timer);

		if(time > timer->delay)
		{
			unsigned int c = timer->elapseCount++;
			TimeReset(timer);
			timer->elapseCount = c;

			return true;
		}
	}

	return false;
}

PUBLIC bool TimeElapse(win32Time *timer, double *tOut)
{
	if(timer->pauseCount == 0)
	{
		*tOut = TimeGetTime(timer);

		if(*tOut > timer->delay)
		{
			unsigned int c = timer->elapseCount++;
			TimeReset(timer);
			timer->elapseCount = c;

			return true;
		}
	}

	return false;
}

//
// Clock
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void ClockInit(Clock *clock)
{
	if(clock)
	{
		clock->hour = clock->min = clock->sec = 0;
		TimeInit(&clock->SecDelay, 1000);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void ClockUpdate(Clock *clock)
{
	if(clock)
	{
		if(TimeElapse(&clock->SecDelay))
			clock->sec++;

		if(clock->sec == 60)
		{
			clock->sec = 0;
			clock->min++;
		}

		if(clock->min == 60)
		{
			clock->min = 0;
			clock->hour++;
		}
	}
}