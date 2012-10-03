#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

RETCODE MCB_MainMenu(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		{
			hCFG cfg = (hCFG)wParam;

			hMenu->SetFlag(MENU_FLAG_NO_DARK_BKGRND, true);

			//prepare world with empty profile
			if(g_world)
			{
				g_world->ProfilePrepare();

				//load up the main menu music
				char buff[MAXCHARBUFF];

				if(CfgGetItemStr(cfg, "special", "music", buff))
				{
					string musPath = GAMEFOLDER;
					musPath += "\\";
					musPath += buff;

					g_world->MusicSet(musPath.c_str());
				}
			}
		}
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		//which button pressed?
		switch(wParam)
		{
		case MAINMENU_NEW:
			MenuEnableCurrent(false);

			//open up the 'new' menu
			MenuGameAdd(GAMEMENU_NEW);
			MenuEnableCurrent(true);

			break;

		case MAINMENU_LOAD:
			MenuEnableCurrent(false);

			//open up the 'new' menu
			MenuGameAdd(GAMEMENU_LOAD);
			MenuEnableCurrent(true);
			break;

		case MAINMENU_OPTIONS:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_OPTIONS);
			MenuEnableCurrent(true);
			break;

		case MAINMENU_CREDITS:
			if(g_world)
				g_world->SetFlag(WORLD_FLAG_CREDITS, true);

			MenuEnableCurrent(false);
			break;

		case MAINMENU_EXIT:
			PostQuitMessage(0);
			break;
		}
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
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