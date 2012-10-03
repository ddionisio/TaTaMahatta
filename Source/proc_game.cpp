#include "tata_main.h"

#include "tata_world.h"

#include "tata_menu.h"
#include "tata_menu_game.h"

#include "tata_stage_select.h"

#include "tata_globals.h"

//global stuff

World *g_world = 0;

PUBLIC RETCODE GameProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		TataDisplayLoad();

		//initialize game world
		g_world = new World;

		if(!g_world) { ErrorMsg("GameProc", "Unable to create world", true); return RETCODE_FAILURE; }


		//In case we wanted to load a map right away
		if(g_gameFlag.CheckFlag(GAME_FLAG_MAP))
		{
			g_world->MapLoad(g_MapLoadPath.c_str());

			StageSelectInit(g_levelListPath.c_str(), 0);

			g_gameFlag.SetFlag(GAME_FLAG_MAP, false);

			//load pause-in-game menu
			MenuGameAdd(GAMEMENU_PAUSEINGAME);
		}
		else
		{
			//////////////////////////////////////////////////////
			//We will always start with the cloud level

			//load cloud level
			g_world->MapLoad(g_cloudMapPath.c_str());

			//load main menu
			MenuGameAdd(GAMEMENU_MAINMENU);
			MenuEnableCurrent(true);

			//do not have input on world
			g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);
		}

		break;

	case GMSG_UPDATE:
		//check to see if we want to pause
		if(g_world)
		{
			if(!g_world->CheckFlag(WORLD_FLAG_INPUT_DISABLE)
				&& !g_world->CheckFlag(WORLD_FLAG_PAUSE)
				&& InputIsReleased(INP_START))
			{
				//enable pause menu and pause the world
				//pause happens inside the menu pause
				MenuEnableCurrent(true);
			}
			else if(!g_world->CheckFlag(WORLD_FLAG_INPUT_DISABLE)
				&& !g_world->CheckFlag(WORLD_FLAG_PAUSE)
				&& INPXKbIsReleased(DIK_ESCAPE))
			{
				MenuEnableCurrent(false);

				//open up the 'exit' menu
				MenuGameAdd(GAMEMENU_EXIT);
				MenuEnableCurrent(true);
			}

			//update menu
			MenuUpdateCurrent();

			//update world
			g_world->Update();

			//check to see if we want to go back to the
			//cloud level.
			if(g_world->CheckFlag(WORLD_FLAG_RELOAD_GAME))
			{
				g_world->SetFlag(WORLD_FLAG_RELOAD_GAME, false);
				ProcChangeState(PROC_GAME);
			}
			else if(g_world->CheckFlag(WORLD_FLAG_CREDITS))
			{
				g_world->SetFlag(WORLD_FLAG_CREDITS, false);
				ProcChangeState(PROC_CREDITS);
			}
		}
		break;

	case GMSG_DISPLAY:
		//display world
		if(g_world)
			g_world->Display();

		//display menus
		MenuDisplayAll();

		break;

	case GMSG_DESTROY:
		//destroy world
		if(g_world)
		{ delete g_world; g_world = 0; }

		//destroy stages
		StageSelectDestroy();

		//destroy menus
		MenuDestroyAll();

		break;
	}

	return RETCODE_SUCCESS;
}