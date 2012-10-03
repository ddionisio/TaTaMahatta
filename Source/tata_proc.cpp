#include "tata_main.h"

//
// Global for game proc table and current state
//
GAMEPROC g_gameProc[PROC_MAX]={0, IntroProc, GameProc, CreditProc};

eGameProcType g_gameCurProc=PROC_NULL;

//
// This is all the proc interface
//

/////////////////////////////////////
// Name:	ProcInit
// Purpose:	initialize given proc state
// Output:	stuff initialized
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ProcInit(eGameProcType type)
{
	return g_gameProc[type](GMSG_INIT, 0, 0);
}

/////////////////////////////////////
// Name:	ProcDestroy
// Purpose:	destroys the current proc
// Output:	stuff destroyed
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ProcDestroy()
{
	if(g_gameCurProc == PROC_NULL)
		return RETCODE_SUCCESS;

	return g_gameProc[g_gameCurProc](GMSG_DESTROY, 0, 0);
}

/////////////////////////////////////
// Name:	ProcChangeState
// Purpose:	change current game state
//			This will destroy the
//			old proc and
//			initialize the new proc
// Output:	stuff destroyed and
//			initialized
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ProcChangeState(eGameProcType type)
{
	RETCODE ret;

	//destroy previous proc
	ProcDestroy();

	//change to new proc
	g_gameCurProc = type;

	//initialize new proc
	ret = ProcInit(g_gameCurProc);

	return ret;
}

/////////////////////////////////////
// Name:	ProcUpdate
// Purpose:	update current proc
// Output:	update
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE ProcUpdate()
{
	if(g_gameCurProc == PROC_NULL)
		return RETCODE_SUCCESS;

	return g_gameProc[g_gameCurProc](GMSG_UPDATE, 0, 0);
}

/////////////////////////////////////
// Name:	ProcDisplay
// Purpose:	display current proc
// Output:	display
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE ProcDisplay()
{
	if(g_gameCurProc == PROC_NULL)
		return RETCODE_SUCCESS;

	return g_gameProc[g_gameCurProc](GMSG_DISPLAY, 0, 0);
}