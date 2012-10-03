#ifndef _inputX_i_h
#define _inputX_i_h

#include "inputX.h"

#define INP_MALLOC(s) calloc(1, (s))//malloc((s))
#define INP_FREE(data) free((data))

#define INP_UP			0		//if button is untouched
#define INP_RELEASED	1		//if button was released (only used with buffer update)
#define INP_DOWN		0x80	//if button is down

//structures

typedef struct _inputJoystick {
	int						enumInd;	//index in enumeration array
	LPDIRECTINPUTDEVICE8	device;		//the device
	DIJOYSTATE2				state;		//state when updated

	int						min,max;	//min/max range

	BYTE					arrowStates[eGamePadNum];	//used by gamepads (up/released/down)
	BYTE					btnStates[MAXJOYSTICKBTN];	//btn states (up/released/down)

	int						ref;		//number of reference
} inputJoystick;

typedef vector< DIDEVICEINSTANCE > JoystickEnumArray;
typedef list< hJOYSTICK >		   JoystickList;

extern DWORD				g_coopFlag;				//behavior flag

extern LPDIRECTINPUT8       g_pDInput;				//The mac daddy of Dinput!

extern LPDIRECTINPUTDEVICE8 g_pDKeyboard;			//The keyboard device interface

extern DIDEVICEOBJECTDATA	g_pKbBuff[SAMPLE_BUFFER_SIZE];	//Keyboard buffer

extern JoystickEnumArray	g_joystickEnums;		//all the joystick enumeration
extern JoystickList			g_joysticks;			//all the joysticks

//checks error
PROTECTED bool _INPXCheckError(HRESULT hr, bool displayMsg, const char *header);

//joystick stuff

//destroys all joystick
PROTECTED void INPXJoystickDestroyAll();

#endif