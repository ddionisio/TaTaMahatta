#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

int g_curJInpInd;

//Joystick Config
RETCODE MCB_Jstick_Cfg(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		InputTemp();

		g_curJInpInd = -1;
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case JSTICK_CFG_A:
		case JSTICK_CFG_B:
		case JSTICK_CFG_C:
		case JSTICK_CFG_D:
		case JSTICK_CFG_E:
		case JSTICK_CFG_START:
			if(g_curJInpInd == -1)
			{
				g_curJInpInd = wParam - 1;

				hMenu->CursorShow(false);
			}
			break;

		case JSTICK_CFG_OK:
			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;

		case JSTICK_CFG_DEFAULT:
			InputDefault();
			break;

		case JSTICK_CFG_CANCEL:
			InputRestore();

			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;
		}
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			if(InputGetJoystick())
			{
				//check to see if there is input update
				//only if g_curJInpInd != -1
				if(g_curJInpInd != -1 
					&& (INPXJoystickAnyBtnReleased(InputGetJoystick())
					   || INPXJoystickAnyArrowReleased(InputGetJoystick())
					   || INPXKbIsReleased(DIK_ESCAPE)))
				{
					//gamepad arrow and keyboard escape is used
					//as cancel
					if(!INPXJoystickAnyArrowReleased(InputGetJoystick())
						&& !INPXKbIsReleased(DIK_ESCAPE))
					{
						for(int i = 0; i < MAXJOYSTICKBTN; i++)
						{
							if(INPXJoystickIsBtnReleased(InputGetJoystick(), (BYTE)i))
							{
								bool bGotCode = false;

								//check to see if this keycode is already
								//assigned to other keys
								int kCode;
								for(int j = INP_A; j < INP_MAX; j++)
								{
									kCode = InputGetCode(false, (eGameInput)j);

									//if so, swap the keycode
									if(kCode == i)
									{
										InputQuery(false, (eGameInput)j, InputGetCode(false, (eGameInput)(g_curJInpInd+INP_A)));
										InputQuery(false, (eGameInput)(g_curJInpInd+INP_A), kCode);

										bGotCode = true;

										break;
									}
								}

								if(!bGotCode)
									InputQuery(false, (eGameInput)(g_curJInpInd+INP_A), i);
							}
						}
					}

					hMenu->CursorShow(true);

					g_curJInpInd = -1;
				}

				//set the texts
				for(int i = 0; i < INP_MAX_BTN; i++)
				{
					if(g_curJInpInd == i)
						hMenu->SendItemMessage(((i+1)*1000)+1, MENU_ITM_MSG_SETTEXT, 
								(WPARAM)"press any btn", 0);
					else
					{
						char buff[MAXCHARBUFF]={0};

						sprintf(buff, "button %d", InputGetCode(false, (eGameInput)(i+INP_A)));

						hMenu->SendItemMessage(((i+1)*1000)+1, MENU_ITM_MSG_SETTEXT, 
								(WPARAM)buff, 0);
					}
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