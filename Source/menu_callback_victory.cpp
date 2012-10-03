#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#include "tata_stage_select.h"

PRIVATE void _VictoryExit()
{
	//set to ending scene, even if there is none
	g_world->CutscenePrepareEnding();

	MenuExitAll();

	g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

	//load cloud-in-game menu
	MenuGameAdd(GAMEMENU_PAUSECLOUD);

	g_world->HUDActive(true);

	//super special bkfx
	fadeinout_init sBKFX;
	sBKFX.r = sBKFX.g = sBKFX.b = 255;
	sBKFX.delay = 2000;
	TataSetBKFX(BKFXCreate(eBKFX_FADEINOUT, &sBKFX));
}

RETCODE MCB_Victory(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		{
			hMenu->SetFlag(MENU_FLAG_NO_DARK_BKGRND, true);

			g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);

			//we don't need the cursor on this one
			hMenu->CursorShow(false);

			g_world->HUDActive(false);

			char buff[MAXCHARBUFF]={0};

			////////////////////////////////////////////////////////////
			//set clock
			Clock clock = g_world->GetClock();

			sprintf(buff, "%02d:%02d:%02d", clock.hour, clock.min, clock.sec);

			hMenu->SendItemMessage(VICTORY_TIME, MENU_ITM_MSG_SETTEXT, 
									(WPARAM)buff, 0);

			////////////////////////////////////////////////////////////
			//set artafact
			if(g_world->GetArTaFactMax() > 0)
			{
				sprintf(buff, "%d/%d", g_world->GetArTaFactMin(),
					g_world->GetArTaFactMax());
			}
			else
			{
				sprintf(buff, "N\\A");
			}

			hMenu->SendItemMessage(VICTORY_ARTAFACT, MENU_ITM_MSG_SETTEXT, 
									(WPARAM)buff, 0);
		}
		break;

	case MENU_MSG_BTN:
		if(lParam == INP_STATE_RELEASED)
		{
			switch(wParam)
			{
			case INP_A:
			case INP_B:
			case INP_C:
			case INP_D:
			case INP_START:
				_VictoryExit();
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		_VictoryExit();
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			if(INPXKbIsReleased(DIK_ESCAPE) || INPXKbIsReleased(DIK_RETURN))
				_VictoryExit();
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