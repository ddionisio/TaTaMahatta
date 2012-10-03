#include "inputX.h"
#include "inputX_i.h"

JoystickEnumArray	g_joystickEnums;		//all the joystick enumeration
JoystickList			g_joysticks;			//all the joysticks

PROTECTED BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
	DIDEVICEINSTANCE newEnum;

	memcpy(&newEnum, pdidInstance, sizeof(DIDEVICEINSTANCE));

	//just add it to the enumeration list
	g_joystickEnums.push_back(newEnum);

	return DIENUM_CONTINUE;
}

/////////////////////////////////////
// Name:	INPXJoystickEnum
// Purpose:	enumerate all the attached
//			joysticks
// Output:	stuff
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXJoystickEnum()
{
	g_joystickEnums.clear();

	//get all the attached joysticks
	if(_INPXCheckError(g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
		NULL, DIEDFL_ATTACHEDONLY ), true, "INPXJoystickEnum"))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;										 
}

/////////////////////////////////////
// Name:	INPXJoystickGetNumEnum
// Purpose:	get the number of enumerated
//			joysticks
// Output:	none
// Return:	number of joysticks
/////////////////////////////////////
PUBLIC int INPXJoystickGetNumEnum()
{
	return g_joystickEnums.size();
}

/////////////////////////////////////
// Name:	INPXJoystickGetNameEnum
// Purpose:	get the joystick name from
//			enumeration
// Output:	strOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickGetNameEnum(int ind, char *strOut)
{
	if(INPXJoystickGetNumEnum() > 0 && ind >= 0)
		strcpy(strOut, g_joystickEnums[ind].tszProductName);
	else
		strOut[0] = 0;
}

/////////////////////////////////////
// Name:	INPXJoystickGetIndEnum
// Purpose:	get the index from enumeration
//			that matches the given joystick
//			name
// Output:	none
// Return:	index in enumeration, -1 if not found
/////////////////////////////////////
PUBLIC int INPXJoystickGetIndEnum(const char *name)
{
	for(int i = 0; i < g_joystickEnums.size(); i++)
	{
		if(strcmp(g_joystickEnums[i].tszProductName, name) == 0)
			return i;
	}

	return -1; //not found
}

PRIVATE void _JoystickDestroy(hJOYSTICK *jStick)
{
	if(*jStick)
	{
		if((*jStick)->device)
		{
			(*jStick)->device->Unacquire();
			(*jStick)->device->Release();
		}

		INP_FREE(*jStick);
	}
}

PROTECTED void INPXJoystickDestroyAll()
{
	hJOYSTICK jStick;

	for(JoystickList::iterator i = g_joysticks.begin(); i != g_joysticks.end(); i++)
	{
		jStick = *i;

		_JoystickDestroy(&jStick);
	}

	g_joysticks.clear();
}

/////////////////////////////////////
// Name:	INPXJoystickDestroy
// Purpose:	destroys the joystick device
// Output:	joystick destroyed (virtually)
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickDestroy(hJOYSTICK *jStick)
{
	if(*jStick)
	{
		if((*jStick)->ref <= 0)
		{
			//just remove the pointer of object from list
			g_joysticks.remove(*jStick);

			_JoystickDestroy(jStick);
		}
		else
			(*jStick)->ref--;
	}

	jStick = 0;
}

/////////////////////////////////////
// Name:	INPXJoystickGetIndEnum
// Purpose:	get the index enum. of given
//			joystick.
// Output:	none
// Return:	enumeration index
/////////////////////////////////////
PUBLIC int INPXJoystickGetIndEnum(hJOYSTICK jStick)
{
	return jStick->enumInd;
}

PROTECTED hJOYSTICK _INPXJoystickSearch(int enumInd)
{
	hJOYSTICK theJoy = 0;

	for(JoystickList::iterator i = g_joysticks.begin(); i != g_joysticks.end(); i++)
	{
		theJoy = *i;

		if(theJoy)
		{
			if(strcmp(g_joystickEnums[enumInd].tszProductName,
				g_joystickEnums[theJoy->enumInd].tszProductName) == 0)
				break;
		}
	}

	return theJoy;
}

/////////////////////////////////////
// Name:	INPXJoystickAddRef
// Purpose:	add a reference
// Output:	ref increment
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickAddRef(hJOYSTICK jStick)
{
	jStick->ref++;
}

PROTECTED BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    hJOYSTICK jStick = (hJOYSTICK)pContext;

    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
	{
		//set range of axis
		DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = jStick->min;
        diprg.lMax              = jStick->max;
    
        // Set the range for the axis
        if(FAILED(jStick->device->SetProperty(DIPROP_RANGE, &diprg.diph)))
			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

/////////////////////////////////////
// Name:	INPXJoystickInit
// Purpose:	intializes the joystick
//			device
//			the [min,max] range is for
//			the axis usu. (-1000,1000)
// Output:	joystick initialize
// Return:	success if success
/////////////////////////////////////
PUBLIC hJOYSTICK INPXJoystickInit(HWND hwnd, int enumInd,
								  int min, int max)
{
	//make sure there are joysticks...
	if(g_joystickEnums.size() == 0)
		return 0;

	//check to see if this joystick alread exists
	hJOYSTICK newStick = _INPXJoystickSearch(enumInd);

	if(!newStick)
	{
		newStick = (hJOYSTICK)INP_MALLOC(sizeof(inputJoystick));

		if(!newStick) { ASSERT_MSG(0, "Unable to allocate new joystick!", "INPXJoystickInit"); return 0; }

		newStick->enumInd = enumInd;
		
		//create the device
		if(_INPXCheckError(g_pDInput->CreateDevice(g_joystickEnums[enumInd].guidInstance, &newStick->device, NULL),
			true, "INPXJoystickInit"))
		{ _JoystickDestroy(&newStick); return 0; }

		// A data format specifies which controls on a device we are interested in,
		// and how they should be reported. This tells DInput that we will be
		// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		if(_INPXCheckError(newStick->device->SetDataFormat(&c_dfDIJoystick2),
			true, "INPXJoystickInit"))
		{ _JoystickDestroy(&newStick); return 0; }

		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.
		if(_INPXCheckError(newStick->device->SetCooperativeLevel(hwnd, g_coopFlag), 
			true, "INPXJoystickInit"))
		{ _JoystickDestroy(&newStick); return 0; }

		//set the min/max of all axis
		newStick->min = min;
		newStick->max = max;

		newStick->device->EnumObjects(EnumObjectsCallback, (VOID*)newStick, DIDFT_ALL);

		//insert to global list
		g_joysticks.push_back(newStick);
	}
	else
		INPXJoystickAddRef(newStick);

	return newStick;
}

/////////////////////////////////////
// Name:	INPXJoystickUpdate
// Purpose:	update the given joystick
// Output:	joystick states filled
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXJoystickUpdate(hJOYSTICK jStick)
{
	if(!jStick)
		return RETCODE_FAILURE;

	int i;

	//reset released btns.
	for(i = 0; i < eGamePadNum; i++)
	{
		if(jStick->arrowStates[i] == INP_RELEASED)
			jStick->arrowStates[i] = INP_UP;
	}

	for(i = 0; i < MAXJOYSTICKBTN; i++)
	{
		if(jStick->btnStates[i] == INP_RELEASED)
			jStick->btnStates[i] = INP_UP;
	}

	memset(&jStick->state, 0 , sizeof(jStick->state));
	
	HRESULT hr;

	hr = jStick->device->Poll();

	if(FAILED(hr))
	{
		// DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = jStick->device->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = jStick->device->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return RETCODE_SUCCESS; 
	}

	// Get the input's device state
    if(FAILED(hr = jStick->device->GetDeviceState(sizeof(DIJOYSTATE2), &jStick->state)))
	{ _INPXCheckError(hr, true, "INPXJoystickUpdate"); return RETCODE_FAILURE; }

	//check the arrow status

	//left/right
	if(jStick->state.lX == jStick->min)
		jStick->arrowStates[eGamePadLeft] = INP_DOWN;
	else if(jStick->state.lX == jStick->max)
		jStick->arrowStates[eGamePadRight] = INP_DOWN;
	else
	{
		if(jStick->arrowStates[eGamePadLeft] == INP_DOWN)
			jStick->arrowStates[eGamePadLeft] = INP_RELEASED;
		else if(jStick->arrowStates[eGamePadRight] == INP_DOWN)
			jStick->arrowStates[eGamePadRight] = INP_RELEASED;
	}

	//up/down
	if(jStick->state.lY == jStick->min)
		jStick->arrowStates[eGamePadUp] = INP_DOWN;
	else if(jStick->state.lY == jStick->max)
		jStick->arrowStates[eGamePadDown] = INP_DOWN;
	else
	{
		if(jStick->arrowStates[eGamePadUp] == INP_DOWN)
			jStick->arrowStates[eGamePadUp] = INP_RELEASED;
		else if(jStick->arrowStates[eGamePadDown] == INP_DOWN)
			jStick->arrowStates[eGamePadDown] = INP_RELEASED;
	}

	//check the buttons
	for(i = 0; i < MAXJOYSTICKBTN; i++ )
    {
        if(jStick->state.rgbButtons[i] & INP_DOWN)
			jStick->btnStates[i] = INP_DOWN;
		else if(jStick->btnStates[i] == INP_DOWN)
			jStick->btnStates[i] = INP_RELEASED;
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	INPXJoystickAnyBtnPressed
// Purpose:	determine if any btn. is
//			pressed
// Output:	none
// Return:	true if any is pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyBtnPressed(hJOYSTICK jStick)
{
	if(!jStick)
		return false;

	for( int i = 0; i < MAXJOYSTICKBTN; i++ )
	{
		if(jStick->btnStates[i] == INP_DOWN)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	INPXJoystickAnyBtnReleased
// Purpose:	determine if any btn. was
//			released
// Output:	none
// Return:	true if any is pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyBtnReleased(hJOYSTICK jStick)
{
	if(!jStick)
		return false;

	for( int i = 0; i < MAXJOYSTICKBTN; i++ )
	{
		if(jStick->btnStates[i] == INP_RELEASED)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	INPXJoystickIsBtnPressed
// Purpose:	checks to see if given
//			btn. is pressed
// Output:	none
// Return:	true if btn. is pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsBtnPressed(hJOYSTICK jStick, BYTE btn)
{
	if(!jStick)
		return false;

	return (jStick->btnStates[btn] == INP_DOWN);
}

/////////////////////////////////////
// Name:	INPXJoystickIsBtnReleased
// Purpose:	checks to see if given
//			btn. was released
// Output:	none
// Return:	true if btn. was released
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsBtnReleased(hJOYSTICK jStick, BYTE btn)
{
	if(!jStick)
		return false;

	return (jStick->btnStates[btn] == INP_RELEASED);
}

/////////////////////////////////////
// Name:	INPXJoystickAnyArrowPressed
// Purpose:	checks to see if any arrow
//			is pressed
// Output:	none
// Return:	true if pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyArrowPressed(hJOYSTICK jStick)
{
	if(!jStick)
		return false;

	for(int i = 0; i < eGamePadNum; i++)
	{
		if(jStick->arrowStates[i] == INP_DOWN)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	INPXJoystickAnyArrowReleased
// Purpose:	checks to see if any arrow
//			is released
// Output:	none
// Return:	true if pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyArrowReleased(hJOYSTICK jStick)
{
	if(!jStick)
		return false;

	for(int i = 0; i < eGamePadNum; i++)
	{
		if(jStick->arrowStates[i] == INP_RELEASED)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	INPXJoystickIsArrowPressed
// Purpose:	checks to see if given arrow
//			is pressed
// Output:	none
// Return:	true if pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsArrowPressed(hJOYSTICK jStick, eGamePadArrows arrow)
{
	if(!jStick)
		return false;

	return (jStick->arrowStates[arrow] == INP_DOWN);
}

/////////////////////////////////////
// Name:	INPXJoystickIsArrowReleased
// Purpose:	checks to see if given arrow
//			is pressed
// Output:	none
// Return:	true if released
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsArrowReleased(hJOYSTICK jStick, eGamePadArrows arrow)
{
	if(!jStick)
		return false;

	return (jStick->arrowStates[arrow] == INP_RELEASED);
}

PUBLIC DIJOYSTATE2 INPXJoystickGetState(hJOYSTICK jStick)
{
	if(!jStick)
	{
		DIJOYSTATE2 ass={0};
		return ass;
	}

	return jStick->state;
}

/////////////////////////////////////
// Name:	INPXJoystickClear
// Purpose:	clear the joystick buffer
// Output:	buffer cleared
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickClear(hJOYSTICK jStick)
{
	if(!jStick)
		return;

	memset(jStick->arrowStates, 0, sizeof(jStick->arrowStates));
	memset(jStick->btnStates, 0, sizeof(jStick->btnStates));
}