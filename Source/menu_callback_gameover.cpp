#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

RETCODE MCB_GameOver(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		//which button pressed?
		switch(wParam)
		{
		case GAMEOVER_RESTART:
			g_world->MapReload();
			
			g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

			MenuExitAll();

			//load in-game menu
			MenuGameAdd(GAMEMENU_PAUSEINGAME);
			break;

		case GAMEOVER_RETURNCLOUD:
			//load cloud level
			g_world->MapPostLoadCloud(0, 0, true);

			MenuExitAll();

			g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

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
		break;

	case MENU_MSG_DRAW:
		break;
	}

	return RETCODE_SUCCESS;
}