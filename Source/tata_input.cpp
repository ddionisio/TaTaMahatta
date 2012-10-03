#include "tata_main.h"

#include "tata_globals.h"

#define INPUT_CFG_SECTION "input"
#define INPUT_CFG_JOYSTICK "joystick"

int		  g_joystickEnum = 0;
hJOYSTICK g_joystick = 0;

//key strings inside config
char *g_keyboardStrBtns[INP_MAX] = {"kb_UP", "kb_DOWN", "kb_LEFT", "kb_RIGHT", "kb_A", "kb_B", "kb_C", "kb_D", "kb_E", "kb_START"};
char *g_joystickStrBtns[INP_MAX] = {0, 0, 0, 0, "jy_A", "jy_B", "jy_C", "jy_D", "jy_E", "jy_START"};

//keycodes for keyboard (temporary)
int g_tempKeyboardBtns[INP_MAX];

//keycodes for gamepad (temporary)
int g_tempJoystickBtns[INP_MAX];


//keycodes for keyboard (defaults)
int g_defaultKeyboardBtns[INP_MAX]={DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_Z, DIK_X, DIK_A, DIK_S, DIK_C, DIK_SPACE};

//keycodes for gamepad (defaults)
int g_defaultJoystickBtns[INP_MAX]={eGamePadUp, eGamePadDown, eGamePadLeft, eGamePadRight, 0, 1, 2, 3, 4, 5};


//keycodes for keyboard
int g_keyboardBtns[INP_MAX]={DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_Z, DIK_X, DIK_A, DIK_S, DIK_C, DIK_SPACE};

//keycodes for gamepad
int g_joystickBtns[INP_MAX]={eGamePadUp, eGamePadDown, eGamePadLeft, eGamePadRight, 0, 1, 2, 3, 4, 5};

//double tap stuff
struct keyTap {
	win32Time timer;		//delay for double tap
	bool	  bCheckTap;	//check for tapping
	bool	  bDTapped;		//key double tapped
	bool	  bCheck;
};

keyTap g_keyTaps[INP_MAX]={0};

/////////////////////////////////////
// Name:	InputGetJoystick
// Purpose:	only used in certain places,
//			like configuration.  DONT
//			DESTROY!!!
// Output:	none
// Return:	the joystick
/////////////////////////////////////
PUBLIC hJOYSTICK InputGetJoystick()
{
	return g_joystick;
}

/////////////////////////////////////
// Name:	InputTemp
// Purpose:	back-up current button
//			config
// Output:	input button config backup
// Return:	none
/////////////////////////////////////
PUBLIC void InputTemp()
{
	memcpy(g_tempKeyboardBtns, g_keyboardBtns, sizeof(g_keyboardBtns));
	memcpy(g_tempJoystickBtns, g_joystickBtns, sizeof(g_joystickBtns));
}

/////////////////////////////////////
// Name:	InputRestore
// Purpose:	restore input button
//			config based on stuff
//			before InputTemp() was
//			called.
// Output:	input buttons set
// Return:	none
/////////////////////////////////////
PUBLIC void InputRestore()
{
	memcpy(g_keyboardBtns, g_tempKeyboardBtns, sizeof(g_keyboardBtns));
	memcpy(g_joystickBtns, g_tempJoystickBtns, sizeof(g_joystickBtns));
}

/////////////////////////////////////
// Name:	InputDefault
// Purpose:	set to default input
// Output:	input buttons set to
//			default
// Return:	none
/////////////////////////////////////
PUBLIC void InputDefault()
{
	memcpy(g_keyboardBtns, g_defaultKeyboardBtns, sizeof(g_keyboardBtns));
	memcpy(g_joystickBtns, g_defaultJoystickBtns, sizeof(g_joystickBtns));
}

/////////////////////////////////////
// Name:	InputGetCode
// Purpose:	get input code
// Output:	none
// Return:	the input code
/////////////////////////////////////
PUBLIC int InputGetCode(bool bKeyboard, eGameInput type)
{
	if(bKeyboard)
		return g_keyboardBtns[type];

	return g_joystickBtns[type];
}

/////////////////////////////////////
// Name:	InputQuery
// Purpose:	query the given button
//			with given code(from INPX)
//			bKeyboard==true if you want
//			code for keyboard = DIK_<something>
//			code for joystick = <btn. number>
//			to set keyboard, otherwise...
// Output:	button set
// Return:	none
/////////////////////////////////////
PUBLIC void InputQuery(bool bKeyboard, eGameInput type, int code)
{
	if(bKeyboard)
		g_keyboardBtns[type] = code;
	else
	{
		switch(type)
		{
		case INP_UP:
		case INP_DOWN:
		case INP_LEFT:
		case INP_RIGHT:
			//set nothing.
			break;
		default:
			g_joystickBtns[type] = code;
			break;
		}
	}
}

/////////////////////////////////////
// Name:	InputLoad
// Purpose:	loads the input from given
//			config
// Output:	input initialized
// Return:	success if all goes well
/////////////////////////////////////
PUBLIC RETCODE InputLoad(hCFG cfg)
{
	//assuming direct input has been initialized...
	//as well as keyboard.

	//destroy previous component
	InputDestroy();

	//////////////////////////////////////////
	//initialize joystick
	INPXJoystickEnum();

	char joystickName[MAXCHARBUFF];

	if(CfgGetItemStr(cfg, INPUT_CFG_SECTION, INPUT_CFG_JOYSTICK, joystickName))
	{
		g_joystickEnum = INPXJoystickGetIndEnum(joystickName);

		if(g_joystickEnum == -1)
			g_joystickEnum = 0;
	}

	if(INPXJoystickGetNumEnum() > 0)
		g_joystick = INPXJoystickInit(g_hwnd, g_joystickEnum, JOYSTICK_MIN, JOYSTICK_MAX);
	//////////////////////////////////////////

	int i, inpCode;

	//////////////////////////////////////////
	//load the keyboard config.
	for(i = 0; i < INP_MAX; i++)
	{
		inpCode = CfgGetItemInt(cfg, INPUT_CFG_SECTION, g_keyboardStrBtns[i]);

		if(inpCode != -1)
			InputQuery(true, (eGameInput)i, inpCode);
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//load the joystick config.
	for(i = INP_A; i < INP_MAX; i++)
	{
		inpCode = CfgGetItemInt(cfg, INPUT_CFG_SECTION, g_joystickStrBtns[i]);

		if(inpCode != -1)
			InputQuery(false, (eGameInput)i, inpCode);
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//Initialize tap stuff
	double delay = CfgGetItemFloat(cfg, INPUT_CFG_SECTION, "tapDelay");

	for(i = 0; i < INP_MAX; i++)
	{
		TimeInit(&g_keyTaps[i].timer, delay);
		g_keyTaps[i].bCheckTap = false;
		g_keyTaps[i].bDTapped = false;
		g_keyTaps[i].bCheck = false;
	}
	//////////////////////////////////////////

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	InputSave
// Purpose:	save the input config.
// Output:	cfg set
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE InputSave(hCFG cfg)
{
	//////////////////////////////////////////
	//save the joystick name
	if(g_joystickEnum != -1)
	{
		char joystickName[MAXCHARBUFF];

		INPXJoystickGetNameEnum(g_joystickEnum, joystickName);
		
		CfgAddItemStr(cfg, INPUT_CFG_SECTION, INPUT_CFG_JOYSTICK, joystickName);
	}
	//////////////////////////////////////////

	int i;

	//////////////////////////////////////////
	//save the keyboard config.
	for(i = 0; i < INP_MAX; i++)
		CfgAddItemInt(cfg, INPUT_CFG_SECTION, g_keyboardStrBtns[i], g_keyboardBtns[i]);
	//////////////////////////////////////////

	//////////////////////////////////////////
	//save the joystick config.
	for(i = INP_A; i < INP_MAX; i++)
		CfgAddItemInt(cfg, INPUT_CFG_SECTION, g_joystickStrBtns[i], g_joystickBtns[i]);
	//////////////////////////////////////////

	CfgFileSave(cfg);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	InputIsDown
// Purpose:	check for given input state
// Output:	none
// Return:	true if given input is down
/////////////////////////////////////
PUBLIC bool InputIsDown(eGameInput type)
{
	switch(type)
	{
	case INP_UP:
	case INP_DOWN:
	case INP_LEFT:
	case INP_RIGHT:
		//for keyboard, check normal
		//special check for joystick
		if(INPXKbIsPressed(g_keyboardBtns[type])
			|| (g_joystick && INPXJoystickIsArrowPressed(g_joystick, (eGamePadArrows)g_joystickBtns[type])))
			return true;

		break;
	
	default:
		if(INPXKbIsPressed(g_keyboardBtns[type]) 
			|| (g_joystick && INPXJoystickIsBtnPressed(g_joystick, g_joystickBtns[type])))
			return true;

		break;
	}

	return false;
}

/////////////////////////////////////
// Name:	InputIsReleased
// Purpose:	check for given input state
// Output:	none
// Return:	true if given input was
//			released
/////////////////////////////////////
PUBLIC bool InputIsReleased(eGameInput type)
{
	switch(type)
	{
	case INP_UP:
	case INP_DOWN:
	case INP_LEFT:
	case INP_RIGHT:
		//for keyboard, check normal
		//special check for joystick
		if(INPXKbIsReleased(g_keyboardBtns[type])
			|| (g_joystick && INPXJoystickIsArrowReleased(g_joystick, (eGamePadArrows)g_joystickBtns[type])))
			return true;
		break;

	default:
		if(INPXKbIsReleased(g_keyboardBtns[type]) 
			|| (g_joystick && INPXJoystickIsBtnReleased(g_joystick, g_joystickBtns[type])))
			return true;
		break;
	}

	return false;
}

/////////////////////////////////////
// Name:	InputIsDoubleTap
// Purpose:	did we double tapped?
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool InputIsDoubleTap(eGameInput type)
{
	return g_keyTaps[type].bDTapped;
}

/////////////////////////////////////
// Name:	InputAnyBtnReleased
// Purpose:	check to see if any buttons
//			has been released.
//			excludes up/down/left/right
// Output:	none
// Return:	true if any btn. released
/////////////////////////////////////
PUBLIC bool InputAnyBtnReleased()
{
	for(int i = INP_A; i < INP_MAX; i++)
	{
		if(InputIsReleased((eGameInput)i))
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	InputUpdate
// Purpose:	updates the input stuff
// Output:	stuff updated
// Return:	none
/////////////////////////////////////
PUBLIC void InputUpdate()
{
	if(g_joystick)
		INPXJoystickUpdate(g_joystick);

	//check for taps
	for(int i = 0; i < INP_MAX; i++)
	{
		g_keyTaps[i].bDTapped = false;

		if(InputIsDown((eGameInput)i))
		{
			if(g_keyTaps[i].bCheckTap)
			{
				double d = TimeGetTime(&g_keyTaps[i].timer)/TimeGetDelay(&g_keyTaps[i].timer);

				if(d <= 1)
				{
					g_keyTaps[i].bDTapped = true;
					g_keyTaps[i].bCheck = true;
				}

				g_keyTaps[i].bCheckTap = false;
			}
		}
		else if(InputIsReleased((eGameInput)i))
		{
			if(g_keyTaps[i].bCheck)
				g_keyTaps[i].bCheck = false;
			else
			{
				g_keyTaps[i].bCheckTap = true;
				TimeReset(&g_keyTaps[i].timer);
			}
		}
	}
}

/////////////////////////////////////
// Name:	InputClear
// Purpose:	clear the input buffer
// Output:	buffer cleared
// Return:	none
/////////////////////////////////////
PUBLIC void InputClear()
{
	INPXKbClear();

	if(g_joystick)
		INPXJoystickClear(g_joystick);
}

/////////////////////////////////////
// Name:	InputDestroy
// Purpose:	destroy all input
//			components
//			NOTE: call this on game
//			exit.
// Output:	input stuff destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void InputDestroy()
{
	if(g_joystick)
		INPXJoystickDestroy(&g_joystick);
}

/////////////////////////////////////
// Name:	InputChangeJoystickDevice
// Purpose:	change the joystick device
// Output:	joystick changed
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE InputChangeJoystickDevice(int enumInd)
{
	if(enumInd != g_joystickEnum  && INPXJoystickGetNumEnum() > 0)
	{
		hJOYSTICK newJstick = INPXJoystickInit(g_hwnd, enumInd, JOYSTICK_MIN, JOYSTICK_MAX);

		if(newJstick)
		{
			InputDestroy();
			g_joystick = newJstick;
			g_joystickEnum = enumInd;
		}
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	InputGetJoystickEnumInd
// Purpose:	grab the enum. index of
//			joystick
// Output:	none
// Return:	the joystick enum. index,
//			-1 if no joystick.
/////////////////////////////////////
PUBLIC int InputGetJoystickEnumInd()
{
	if(g_joystick)
		return g_joystickEnum;//INPXJoystickGetIndEnum(g_joystick);

	return -1;
}