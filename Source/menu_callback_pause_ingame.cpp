#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

RETCODE MCB_PauseInGame(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
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
		case PAUSEINGAME_RESUME:
			MenuEnableCurrent(false);

			g_world->SetFlag(WORLD_FLAG_PAUSE, false);
			break;

		case PAUSEINGAME_OPTIONS:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_OPTIONS);
			MenuEnableCurrent(true);
			break;

		case PAUSEINGAME_RESTART:
			g_world->SetFlag(WORLD_FLAG_PAUSE, false);

			g_world->MapReload();

			MenuEnableCurrent(false);
			break;

		case PAUSEINGAME_RETURNCLOUD:

			//load cloud level, if we have a profile
			g_world->MapPostLoadCloud(0, 0, true);

			MenuExitAll();

			g_world->SetFlag(WORLD_FLAG_PAUSE, false);

			//load cloud-in-game menu
			MenuGameAdd(GAMEMENU_PAUSECLOUD);
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