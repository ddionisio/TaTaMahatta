#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

int g_curInpInd;

//Keyboard Config
RETCODE MCB_Kb_Cfg(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		InputTemp();

		g_curInpInd = -1;
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case KB_CFG_UP:
		case KB_CFG_DOWN:
		case KB_CFG_LEFT:
		case KB_CFG_RIGHT:
		case KB_CFG_A:
		case KB_CFG_B:
		case KB_CFG_C:
		case KB_CFG_D:
		case KB_CFG_E:
		case KB_CFG_START:
			if(g_curInpInd == -1)
			{
				g_curInpInd = wParam - 1;

				hMenu->CursorShow(false);
			}
			break;

		case KB_CFG_OK:
			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;

		case KB_CFG_DEFAULT:
			InputDefault();
			break;

		case KB_CFG_CANCEL:
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
			{
				//check to see if there is input update
				//only if g_curInpInd != -1
				if(g_curInpInd != -1 && INPXKbAnyKeyReleased())
				{
					//only escape is not used,
					//used as cancel
					if(!INPXKbIsReleased(DIK_ESCAPE))
					{
						for(int i = 0; i < MAXKBKEY; i++)
						{
							if(INPXKbIsReleased((BYTE)i))
							{
								bool bGotCode = false;

								//check to see if this keycode is already
								//assigned to other keys
								int kCode;
								for(int j = 0; j < INP_MAX; j++)
								{
									kCode = InputGetCode(true, (eGameInput)j);

									//if so, swap the keycode
									if(kCode == i)
									{
										InputQuery(true, (eGameInput)j, InputGetCode(true, (eGameInput)g_curInpInd));
										InputQuery(true, (eGameInput)g_curInpInd, kCode);

										bGotCode = true;

										break;
									}
								}

								if(!bGotCode)
									InputQuery(true, (eGameInput)g_curInpInd, i);

								break;
							}
						}
					}

					hMenu->CursorShow(true);

					g_curInpInd = -1;
				}

				//set the texts
				for(int i = 0; i < INP_MAX; i++)
				{
					if(g_curInpInd == i)
						hMenu->SendItemMessage(((i+1)*1000)+1, MENU_ITM_MSG_SETTEXT, 
								(WPARAM)"press any key", 0);
					else
					{
						char buff[MAXCHARBUFF]={0};

						INPXKbGetStr(InputGetCode(true, (eGameInput)i), buff, MAXCHARBUFF);

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