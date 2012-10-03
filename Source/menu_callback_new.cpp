#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#define MAX_NAME_CHAR	12

char g_name[MAX_NAME_CHAR];
int g_curC;

PRIVATE void _SetNextChar(char c)
{
	if(g_curC < MAX_NAME_CHAR)
	{
		g_name[g_curC] = c;

		if(g_curC < MAX_NAME_CHAR-1)
			g_curC++;
	}
}

PRIVATE void _DeleteChar()
{
	if(g_curC > 0)
		g_curC--;

	for(int i = g_curC; i < MAX_NAME_CHAR; i++)
		g_name[i] = 0;
}

RETCODE MCB_New(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		memset(g_name, 0, sizeof(g_name));
		g_curC = 0;
		break;

	case MENU_MSG_BTN:
		/////////////////////////////////////////////////////////////
		//update controls
		/*if(lParam == INP_STATE_RELEASED)
		{
			switch(wParam)
			{
			case INP_B:
				_DeleteChar();
				break;
			}
		}*/
		break;

	case MENU_MSG_ITEM:
		//which button pressed?
		switch(wParam)
		{
		case NEW_A:
			_SetNextChar('A');
			break;

		case NEW_B:
			_SetNextChar('B');
			break;

		case NEW_C:
			_SetNextChar('C');
			break;

		case NEW_D:
			_SetNextChar('D');
			break;

		case NEW_E:
			_SetNextChar('E');
			break;

		case NEW_F:
			_SetNextChar('F');
			break;

		case NEW_G:
			_SetNextChar('G');
			break;

		case NEW_H:
			_SetNextChar('H');
			break;

		case NEW_I:
			_SetNextChar('I');
			break;

		case NEW_J:
			_SetNextChar('J');
			break;

		case NEW_K:
			_SetNextChar('K');
			break;

		case NEW_L:
			_SetNextChar('L');
			break;

		case NEW_M:
			_SetNextChar('M');
			break;

		case NEW_N:
			_SetNextChar('N');
			break;

		case NEW_O:
			_SetNextChar('O');
			break;

		case NEW_P:
			_SetNextChar('P');
			break;

		case NEW_Q:
			_SetNextChar('Q');
			break;

		case NEW_R:
			_SetNextChar('R');
			break;

		case NEW_S:
			_SetNextChar('S');
			break;

		case NEW_T:
			_SetNextChar('T');
			break;

		case NEW_U:
			_SetNextChar('U');
			break;

		case NEW_V:
			_SetNextChar('V');
			break;

		case NEW_W:
			_SetNextChar('W');
			break;

		case NEW_X:
			_SetNextChar('X');
			break;

		case NEW_Y:
			_SetNextChar('Y');
			break;

		case NEW_Z:
			_SetNextChar('Z');
			break;

		case NEW_DEL:
			_DeleteChar();
			break;

		case NEW_DONE:
			//there should at least be one letter
			if(g_name[0] != 0)
			{
				MenuExitAll();

				g_world->CutsceneRemove();

				g_world->ProfileNew(g_name);
				g_world->ProfilePrepare();

				//load play tutorial...
				MenuGameAdd(GAMEMENU_PLAYTUTORIAL);
				MenuEnableCurrent(true);
			}
			break;

		case NEW_CANCEL:
			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;
		}
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			{
				if(INPXKbIsReleased(DIK_BACKSPACE))
					_DeleteChar();

				hMenu->SendItemMessage(NEW_TEXT, MENU_ITM_MSG_SETTEXT, (WPARAM)g_name, 0);
			}
			break;

		case MENU_UPDATE_ENTERING:
		case MENU_UPDATE_EXITING:
			return RETCODE_BREAK;
		}
		break;

	case MENU_MSG_DRAW:
		break;
	}

	return RETCODE_SUCCESS;
}