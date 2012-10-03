#include "tata_main.h"

#include "tata_world.h"

#include "tata_menu.h"
#include "tata_menu_game.h"

#include "tata_stage_select.h"

#include "tata_globals.h"

//global stuff

hSCRIPT g_creditScene = 0;

PUBLIC RETCODE CreditProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		TataDisplayLoad();

		//////////////////////////////////////////////////////

		//initialize game world
		g_world = new World;

		if(!g_world) { ErrorMsg("GameProc", "Unable to create world", true); return RETCODE_FAILURE; }

		//load credits level
		g_world->MapLoad(g_creditsMapPath.c_str());

		//load credits scene (different from world)
		if(g_world)
			g_creditScene = ScriptLoad(g_world, g_creditsScenePath.c_str(), 0, 0);

		//do not have input on world
		g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);

		break;

	case GMSG_UPDATE:
		if(InputAnyBtnReleased()
			|| INPXKbIsReleased(DIK_ESCAPE)
			|| INPXKbIsReleased(DIK_RETURN))
		{
			//return to main menu
			ProcChangeState(PROC_GAME);
		}

		//check to see if we want to pause
		if(g_world)
		{
			//update menu
			MenuUpdateCurrent();

			//update script
			if(g_creditScene)
			{
				if(ScriptUpdate(g_world, g_creditScene) == RETCODE_BREAK)
					ScriptReset(g_creditScene);
			}

			//update world
			g_world->Update();
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
		//destroy scene
		if(g_creditScene)
		{ ScriptDestroy(g_world, &g_creditScene); g_creditScene = 0; }

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