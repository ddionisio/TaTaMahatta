#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#include "tata_menu_options.h"

//Options
RETCODE MCB_Options_Input(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		break;

	case MENU_MSG_BTN:
		if(lParam == INP_STATE_RELEASED)
		{
			switch(hMenu->GetCurItemID())
			{
			case OPTIONS_JOYSTICK:
				if(wParam == INP_LEFT)
				{
					if(g_joyEnum == 0)
						g_joyEnum = INPXJoystickGetNumEnum()-1;
					else
						g_joyEnum--;
				}
				else if(wParam == INP_RIGHT)
				{
					if(g_joyEnum == INPXJoystickGetNumEnum()-1)
						g_joyEnum = 0;
					else
						g_joyEnum++;
				}

				InputChangeJoystickDevice(g_joyEnum);
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case OPTIONS_CFG_KEYBOARD:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_KB_CFG);
			MenuEnableCurrent(true);
			break;

		case OPTIONS_CFG_JOYSTICK:
			if(InputGetJoystick())
			{
				MenuEnableCurrent(false);

				MenuGameAdd(GAMEMENU_JSTICK_CFG);
				MenuEnableCurrent(true);
			}
			break;

		case OPTIONS_BACK:
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
				if(g_joyEnum != -1 && INPXJoystickGetNumEnum() > 0)
				{
					char buff[MAXCHARBUFF]={0};
					INPXJoystickGetNameEnum(g_joyEnum, buff);

					hMenu->SendItemMessage(OPTIONS_JOYSTICK_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)buff, 0);
				}
				else
				{
					hMenu->SendItemMessage(OPTIONS_JOYSTICK_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)"none", 0);
				}
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