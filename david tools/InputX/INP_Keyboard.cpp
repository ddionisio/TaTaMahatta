#include "inputX.h"
#include "inputX_i.h"

LPDIRECTINPUTDEVICE8 g_pDKeyboard=0;			//The keyboard device interface

DIDEVICEOBJECTDATA	g_pKbBuff[SAMPLE_BUFFER_SIZE];					//Keyboard buffer

DWORD g_prevKNum=0;

BYTE g_keyState[MAXKBKEY]={0};

#define KEYDOWN(data)		(g_keyState[data]==INP_DOWN)
#define KEYRELEASED(data)	(g_keyState[data]==INP_RELEASED)

/////////////////////////////////////
// Name:	INPXKeyboardDestroy
// Purpose:	destroys the keyboard device
// Output:	keyboard destroyed (virtually)
// Return:	none
/////////////////////////////////////
PUBLIC void INPXKeyboardDestroy()
{
	if(g_pDKeyboard)
	{
		g_pDKeyboard->Unacquire();
		g_pDKeyboard->Release();
		g_pDKeyboard=0;
	}
}

/////////////////////////////////////
// Name:	INPXKeyboardInit
// Purpose:	intializes the keyboard
//			device
// Output:	keyboard initialize
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXKeyboardInit(HWND hwnd)
{
	INPXKeyboardDestroy();

	//create the keyboard device
	if(_INPXCheckError(g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDKeyboard, NULL), 
		true, "Error in INPXKeyboardInit")) return RETCODE_FAILURE;

	//set keyboard device as a REAL keyboard
	if(_INPXCheckError(g_pDKeyboard->SetDataFormat(&c_dfDIKeyboard),
		true, "Error in INPXKeyboardInit")) return RETCODE_FAILURE;

	//set up keyboard coop level
	if(_INPXCheckError(g_pDKeyboard->SetCooperativeLevel(hwnd, g_coopFlag),
		true, "Error in INPXKeyboardInit")) return RETCODE_FAILURE;

	//set up buffering
	DIPROPDWORD dipdw;

    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

	//set up keyboard property to tell that we are using buffer
    if(_INPXCheckError(g_pDKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph),true,"Error in _INPXInitDevice"))
        return RETCODE_FAILURE;

	//acquire keyboard device
	g_pDKeyboard->Acquire();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	INPXKeyboardUpdate
// Purpose:	updates the keyboard
// Output:	buffer filled
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE INPXKeyboardUpdate()
{
	int i;

	for(i=0; i < g_prevKNum; i++)
	{
		if(KEYRELEASED(g_pKbBuff[i].dwOfs))
			g_keyState[g_pKbBuff[i].dwOfs] = INP_UP;
	}

	HRESULT hr;

	g_prevKNum = SAMPLE_BUFFER_SIZE;
	
	//First, check to see if the keyboard is still working/functioning
    hr= g_pDKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),g_pKbBuff,&g_prevKNum,0);
    if(hr != DI_OK)
	{
		g_prevKNum = 0;

		//did we lose the keyboard?
        if(hr==DIERR_INPUTLOST || hr==DIERR_NOTACQUIRED) 
		{
			//we gotta have it back!
			if(_INPXCheckError(g_pDKeyboard->Acquire(), false, "Error in INPXKeyboardUpdate"))
				return RETCODE_FAILURE;
			return RETCODE_SUCCESS;
		}
		else
			return RETCODE_FAILURE;
	}

	//update the keyboard states
	for(i=0; i < g_prevKNum; i++)
	{
		g_keyState[g_pKbBuff[i].dwOfs] = 
			(g_pKbBuff[i].dwData & INP_DOWN) ? INP_DOWN : INP_RELEASED;
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	INPXKbAnyKeyPressed
// Purpose:	checks to see if anything
//			is pressed
// Output:	none
// Return:	true if there is pressed
/////////////////////////////////////
PUBLIC bool INPXKbAnyKeyPressed()
{
	for (int count = 0; count < MAXKBKEY; count++)
		if(g_keyState[count] == INP_DOWN)
			return true;

	return false;
}

/////////////////////////////////////
// Name:	INPXKbAnyKeyReleased
// Purpose:	checks to see if anything
//			is released
//			NOTE: Only works on
//				  buffer update
// Output:	none
// Return:	true if there is pressed
/////////////////////////////////////
PUBLIC bool INPXKbAnyKeyReleased()
{
	for (int count = 0; count < MAXKBKEY; count++)
		if(g_keyState[count] == INP_RELEASED)
			return true;

	return false;
}

/////////////////////////////////////
// Name:	INPXKbIsPressed
// Purpose:	checks to see if given
//			kCode is pressed
// Output:	none
// Return:	true if kCode is pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXKbIsPressed(BYTE kCode)
{
	return (g_keyState[kCode] == INP_DOWN);
}

/////////////////////////////////////
// Name:	INPXKbIsReleased
// Purpose:	checks to see if given
//			kCode was released
//			NOTE: Only works on
//				  buffer update
// Output:	none
// Return:	true if kCode is pressed
/////////////////////////////////////
PUBLIC bool __fastcall INPXKbIsReleased(BYTE kCode)
{
	return (g_keyState[kCode] == INP_RELEASED);
}

/////////////////////////////////////
// Name:	INPXKbClear
// Purpose:	clear the keyboard buffer
// Output:	buffer cleared
// Return:	none
/////////////////////////////////////
PUBLIC void INPXKbClear()
{
	memset(g_keyState, 0, sizeof(g_keyState));
}

/////////////////////////////////////
// Name:	INPXKbGetAscii
// Purpose:	grabs the printable
//			ascii of kCode
// Output:	none
// Return:	none 0 for printable
/////////////////////////////////////
PUBLIC char INPXKbGetAscii(BYTE kCode)
{
	bool shiftOn = INPXKbIsPressed(DIK_LSHIFT) || INPXKbIsPressed(DIK_RSHIFT);

	switch(kCode)
	{
	case DIK_0:
		return shiftOn ? ')' : '0';
	case DIK_1:
		return shiftOn ? '!' : '1';
	case DIK_2:
		return shiftOn ? '@' : '2';
	case DIK_3:
		return shiftOn ? '#' : '3';
	case DIK_4:
		return shiftOn ? '$' : '4';
	case DIK_5:
		return shiftOn ? '%' : '5';
	case DIK_6:
		return shiftOn ? '^' : '6';
	case DIK_7:
		return shiftOn ? '&' : '7';
	case DIK_8:
		return shiftOn ? '*' : '8';
	case DIK_9:
		return shiftOn ? '(' : '9';
	case DIK_MINUS:
		return shiftOn ? '_' : '-';
	case DIK_EQUALS:
		return shiftOn ? '+' : '=';
	case DIK_BACKSLASH:
		return shiftOn ? '|' : '\\';
	case DIK_TAB:
		return 9; //TAB is 9, I think...
	case DIK_A:
		return shiftOn ? 'A' : 'a';
	case DIK_B:
		return shiftOn ? 'B' : 'b';
	case DIK_C:
		return shiftOn ? 'C' : 'c';
	case DIK_D:
		return shiftOn ? 'D' : 'd';
	case DIK_E:
		return shiftOn ? 'E' : 'e';
	case DIK_F:
		return shiftOn ? 'F' : 'f';
	case DIK_G:
		return shiftOn ? 'G' : 'g';
	case DIK_H:
		return shiftOn ? 'H' : 'h';
	case DIK_I:
		return shiftOn ? 'I' : 'i';
	case DIK_J:
		return shiftOn ? 'J' : 'j';
	case DIK_K:
		return shiftOn ? 'K' : 'k';
	case DIK_L:
		return shiftOn ? 'L' : 'l';
	case DIK_M:
		return shiftOn ? 'M' : 'm';
	case DIK_N:
		return shiftOn ? 'N' : 'n';
	case DIK_O:
		return shiftOn ? 'O' : 'o';
	case DIK_P:
		return shiftOn ? 'P' : 'p';
	case DIK_Q:
		return shiftOn ? 'Q' : 'q';
	case DIK_R:
		return shiftOn ? 'R' : 'r';
	case DIK_S:
		return shiftOn ? 'S' : 's';
	case DIK_T:
		return shiftOn ? 'T' : 't';
	case DIK_U:
		return shiftOn ? 'U' : 'u';
	case DIK_V:
		return shiftOn ? 'V' : 'v';
	case DIK_W:
		return shiftOn ? 'W' : 'w';
	case DIK_X:
		return shiftOn ? 'X' : 'x';
	case DIK_Y:
		return shiftOn ? 'Y' : 'y';
	case DIK_Z:
		return shiftOn ? 'Z' : 'z';
	case DIK_LBRACKET:
		return shiftOn ? '{' : '[';
	case DIK_RBRACKET:
		return shiftOn ? '}' : ']';
	case DIK_SEMICOLON:
		return shiftOn ? ':' : ';';
	case DIK_APOSTROPHE:
		return shiftOn ? '"' : '\'';
	case DIK_COMMA:
		return shiftOn ? '<' : ',';
	case DIK_PERIOD:
		return shiftOn ? '>' : '.';
	case DIK_SLASH:
		return shiftOn ? '?' : '/';
	case DIK_NUMPADENTER:
	case DIK_RETURN:
		return '\n';
	case DIK_DIVIDE:
		return '/';
	case DIK_MULTIPLY:
		return '*';
	case DIK_SUBTRACT:
		return '-';
	case DIK_ADD:
		return '+';
	}

	return 0;
}

/////////////////////////////////////
// Name:	INPXKbGetStr
// Purpose:	get the string version of
//			a key code
// Output:	none
// Return:	string
/////////////////////////////////////
PUBLIC void INPXKbGetStr(BYTE kCode, char *outbuffer, 
						 int outbuffersize)
{
	DIPROPSTRING string;

	string.diph.dwSize = sizeof(DIPROPSTRING);
	string.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	string.diph.dwObj = kCode;
	string.diph.dwHow = DIPH_BYOFFSET;

	g_pDKeyboard->GetProperty(DIPROP_KEYNAME,
	&string.diph);
	WideCharToMultiByte(CP_ACP, 0, string.wsz, -1,
	outbuffer, outbuffersize, 
	NULL, NULL);
}