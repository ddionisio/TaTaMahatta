#include "inputX.h"
#include "inputX_i.h"

#pragma comment(lib,"Dinput8.lib")

LPDIRECTINPUT8       g_pDInput=0;

DWORD				 g_coopFlag;

/////////////////////////////////////
// Name:	INPXInit
// Purpose:	this will initialize the
//			Direct Input Interface
// Output:	stuff
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXInit(HWND hwnd, HINSTANCE hinst, DWORD coopFlag)
{
	g_coopFlag   = coopFlag;

	//create the main input interface
	if(_INPXCheckError(DirectInput8Create(hinst, DIRECTINPUT_VERSION,
						   IID_IDirectInput8, (void**)&g_pDInput, NULL), 
		true, "Error in INPXInit")) return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	INPXDestroy
// Purpose:	this will terminate DInput
//			as well as all the other devices
// Output:	everything destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void INPXDestroy()
{
	INPXKeyboardDestroy();
	INPXJoystickDestroyAll();

	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput=0;
	}

	g_joystickEnums.clear();
	g_joystickEnums.resize(0);
}