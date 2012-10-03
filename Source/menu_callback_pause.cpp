#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

RETCODE MCB_PauseCloud(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		//which button pressed?
		switch(wParam)
		{
		case PAUSECLOUD_RESUME:
			MenuEnableCurrent(false);

			g_world->SetFlag(WORLD_FLAG_PAUSE, false);
			break;

		case PAUSECLOUD_OPTIONS:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_OPTIONS);
			MenuEnableCurrent(true);
			break;

		case PAUSECLOUD_RETURNMENU:
			g_world->SetFlag(WORLD_FLAG_PAUSE, false);

			//reload game
			g_world->SetFlag(WORLD_FLAG_RELOAD_GAME, true);
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

	case MENU_MSG_ENABLE:
		if(wParam == 1)
			g_world->SetFlag(WORLD_FLAG_PAUSE, true);
		break;

	case MENU_MSG_DRAW:
		break;
	}

	return RETCODE_SUCCESS;
}