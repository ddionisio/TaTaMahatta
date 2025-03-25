#include "timer.h"

DWORD g_timer = 0;

DWORD g_tickerFPS = 0;

int   g_fps = 0, g_oldfps = 0;

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimerFPSBegin()
{
	g_tickerFPS = GetTickCount();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TimerFPSEnd()
{
	if(g_tickerFPS - 1000 > g_timer)
	{
		g_timer = g_tickerFPS;
	
		g_oldfps = g_fps;
		g_fps = 0;
	}
	else
		g_fps++;

	return g_oldfps;
}