#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

//Exit
RETCODE MCB_Exit(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		g_world->SetFlag(WORLD_FLAG_PAUSE, true);
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		//which button pressed?
		switch(wParam)
		{
		case EXIT_RETURN_TO_GAME:
			g_world->SetFlag(WORLD_FLAG_PAUSE, false);
			MenuExitCurrent();
			break;

		case EXIT_SAVE_RETURN_MENU:
			if(g_world)
			{
				g_world->SetFlag(WORLD_FLAG_PAUSE, false);

				//reload game
				g_world->SetFlag(WORLD_FLAG_RELOAD_GAME, true);
			}
			break;

		case EXIT_SAVE_QUIT_GAME:			
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

	case MENU_MSG_ENABLE:
		break;

	case MENU_MSG_DRAW:
		break;
	}

	return RETCODE_SUCCESS;
}