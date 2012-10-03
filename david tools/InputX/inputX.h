#ifndef _inputX_h
#define _inputX_h

#include <InitGuid.h>

#define DIRECTINPUT_VERSION 0x0800

#include "..\common.h"

#include "dinput.h"

#define SAMPLE_BUFFER_SIZE 16  // arbitrary number of buffer elements

//public constants
#define MAXKBKEY		256
#define MAXJOYSTICKBTN	32

//enums
typedef enum { //X/Y axis
	eGamePadLeft,
	eGamePadRight,
	eGamePadUp,
	eGamePadDown,
	eGamePadNum
} eGamePadArrows;

//handles
typedef struct _inputJoystick *hJOYSTICK;

//
// Main interface
//

/////////////////////////////////////
// Name:	INPXInit
// Purpose:	this will initialize the
//			Direct Input Interface
// Output:	stuff
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXInit(HWND hwnd, HINSTANCE hinst, DWORD coopFlag);

/////////////////////////////////////
// Name:	INPXDestroy
// Purpose:	this will terminate DInput
//			as well as all the other devices
// Output:	everything destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void INPXDestroy();

//
// Keyboard interface
//

/////////////////////////////////////
// Name:	INPXKeyboardInit
// Purpose:	intializes the keyboard
//			device
// Output:	keyboard initialize
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXKeyboardInit(HWND hwnd);

/////////////////////////////////////
// Name:	INPXKeyboardDestroy
// Purpose:	destroys the keyboard device
// Output:	keyboard destroyed (virtually)
// Return:	none
/////////////////////////////////////
PUBLIC void INPXKeyboardDestroy();

/////////////////////////////////////
// Name:	INPXKeyboardUpdate
// Purpose:	updates the keyboard
// Output:	buffer filled
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXKeyboardUpdate();

/////////////////////////////////////
// Name:	INPXKbAnyKeyPressed
// Purpose:	checks to see if anything
//			is pressed
// Output:	none
// Return:	true if there is pressed
/////////////////////////////////////
PUBLIC bool INPXKbAnyKeyPressed();

/////////////////////////////////////
// Name:	INPXKbAnyKeyReleased
// Purpose:	checks to see if anything
//			is released
//			NOTE: Only works on
//				  buffer update
// Output:	none
// Return:	true if there is pressed
/////////////////////////////////////
PUBLIC bool INPXKbAnyKeyReleased();

/////////////////////////////////////
// Name:	INPXKbIsPressed
// Purpose:	checks to see if given
//			kCode is pressed
// Output:	none
// Return:	true if kCode is pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXKbIsPressed(BYTE kCode);

/////////////////////////////////////
// Name:	INPXKbIsReleased
// Purpose:	checks to see if given
//			kCode was released
//			NOTE: Only works on
//				  buffer update
// Output:	none
// Return:	true if kCode is pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXKbIsReleased(BYTE kCode);

/////////////////////////////////////
// Name:	INPXKbClear
// Purpose:	clear the keyboard buffer
// Output:	buffer cleared
// Return:	none
/////////////////////////////////////
PUBLIC void INPXKbClear();

/////////////////////////////////////
// Name:	INPXKbGetAscii
// Purpose:	grabs the printable
//			ascii of kCode
// Output:	none
// Return:	none 0 for printable
/////////////////////////////////////
PUBLIC char INPXKbGetAscii(BYTE kCode);

/////////////////////////////////////
// Name:	INPXKbGetStr
// Purpose:	get the string version of
//			a key code
// Output:	none
// Return:	string
/////////////////////////////////////
PUBLIC void INPXKbGetStr(BYTE kCode, char *outbuffer, 
						 int outbuffersize);

//
// Joystick Interface
//

/////////////////////////////////////
// Name:	INPXJoystickEnum
// Purpose:	enumerate all the attached
//			joysticks
//			CALL THIS FIRST BEFORE
//			INITIALIZING ANY JOYSTICKS!!
// Output:	stuff
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXJoystickEnum();

/////////////////////////////////////
// Name:	INPXJoystickGetNumEnum
// Purpose:	get the number of enumerated
//			joysticks
// Output:	none
// Return:	number of joysticks
/////////////////////////////////////
PUBLIC int INPXJoystickGetNumEnum();

/////////////////////////////////////
// Name:	INPXJoystickGetNameEnum
// Purpose:	get the joystick name from
//			enumeration
// Output:	strOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickGetNameEnum(int ind, char *strOut);

/////////////////////////////////////
// Name:	INPXJoystickGetIndEnum
// Purpose:	get the index from enumeration
//			that matches the given joystick
//			name
// Output:	none
// Return:	index in enumeration, -1 if not found
/////////////////////////////////////
PUBLIC int INPXJoystickGetIndEnum(const char *name);

/////////////////////////////////////
// Name:	INPXJoystickInit
// Purpose:	intializes the joystick
//			device
//			the [min,max] range is for
//			the axis usu. (-1,1)
// Output:	joystick initialize
// Return:	success if success
/////////////////////////////////////
PUBLIC hJOYSTICK INPXJoystickInit(HWND hwnd, int enumInd,
								  int min, int max);

/////////////////////////////////////
// Name:	INPXJoystickDestroy
// Purpose:	destroys the joystick device
// Output:	joystick destroyed (virtually)
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickDestroy(hJOYSTICK *jStick);

/////////////////////////////////////
// Name:	INPXJoystickGetIndEnum
// Purpose:	get the index enum. of given
//			joystick.
// Output:	none
// Return:	enumeration index
/////////////////////////////////////
PUBLIC int INPXJoystickGetIndEnum(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickAddRef
// Purpose:	add a reference
// Output:	ref increment
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickAddRef(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickUpdate
// Purpose:	update the given joystick
// Output:	joystick states filled
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXJoystickUpdate(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickAnyBtnPressed
// Purpose:	determine if any btn. is
//			pressed
// Output:	none
// Return:	true if any is pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyBtnPressed(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickAnyBtnReleased
// Purpose:	determine if any btn. was
//			released
// Output:	none
// Return:	true if any is pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyBtnReleased(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickIsBtnPressed
// Purpose:	checks to see if given
//			btn. is pressed
// Output:	none
// Return:	true if btn. is pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsBtnPressed(hJOYSTICK jStick, BYTE btn);

/////////////////////////////////////
// Name:	INPXJoystickIsBtnReleased
// Purpose:	checks to see if given
//			btn. was released
// Output:	none
// Return:	true if btn. was released
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsBtnReleased(hJOYSTICK jStick, BYTE btn);

/////////////////////////////////////
// Name:	INPXJoystickAnyArrowPressed
// Purpose:	checks to see if any arrow
//			is pressed
// Output:	none
// Return:	true if pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyArrowPressed(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickAnyArrowReleased
// Purpose:	checks to see if any arrow
//			is released
// Output:	none
// Return:	true if pressed
/////////////////////////////////////
PUBLIC bool INPXJoystickAnyArrowReleased(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickIsArrowPressed
// Purpose:	checks to see if given arrow
//			is pressed
// Output:	none
// Return:	true if pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsArrowPressed(hJOYSTICK jStick, eGamePadArrows arrow);

/////////////////////////////////////
// Name:	INPXJoystickIsArrowReleased
// Purpose:	checks to see if given arrow
//			is pressed
// Output:	none
// Return:	true if released
/////////////////////////////////////
PUBLIC bool __fastcall INPXJoystickIsArrowReleased(hJOYSTICK jStick, eGamePadArrows arrow);

PUBLIC DIJOYSTATE2 INPXJoystickGetState(hJOYSTICK jStick);

/////////////////////////////////////
// Name:	INPXJoystickClear
// Purpose:	clear the joystick buffer
// Output:	buffer cleared
// Return:	none
/////////////////////////////////////
PUBLIC void INPXJoystickClear(hJOYSTICK jStick);

#endif