#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#include "tata_menu_options.h"

char *g_lightStr[2] = {"Lightmap (best)", "Vertex Lighting"};

/*const char *g_resName[MAXRES]
gfxDisplayMode g_resDat[MAXRES]*/

/*
#define OPTIONS_RESOLUTION			1
#define OPTIONS_RESOLUTION_TEXT		1002
#define OPTIONS_RESOLUTION_APPLY	2
*/
int g_curRes=0;

//Options
RETCODE MCB_Options_GFX(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		{
			//set to correct resolution
			gfxDisplayMode curMode; GFXGetMode(&curMode);

			for(int i = 0; i < MAXRES; i++)
			{
				if(curMode.width == g_resDat[i].width
					&& curMode.bpp == g_resDat[i].bpp)
				{ g_curRes = i; break; }
			}
		}
		break;

	case MENU_MSG_BTN:
		if(lParam == INP_STATE_RELEASED)
		{
			switch(hMenu->GetCurItemID())
			{
			case OPTIONS_RESOLUTION:
				if(wParam == INP_LEFT)
				{ g_curRes--; if(g_curRes<0) g_curRes = MAXRES-1; }
				else if(wParam == INP_RIGHT)
				{ g_curRes++; if(g_curRes==MAXRES) g_curRes = 0; }
				break;

			case OPTIONS_LIGHTING:
				if(wParam == INP_LEFT || wParam == INP_RIGHT)
				{
					g_bVertexLightOpt = !g_bVertexLightOpt;

					if(g_world)
					{
						if(g_world->GetMap())
							QBSPUseVertexLighting(g_world->GetMap(), g_bVertexLightOpt);
					}
				}
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case OPTIONS_RESOLUTION_APPLY:
			{
				//set resolution
				gfxDisplayMode curMode; GFXGetMode(&curMode);

				if(curMode.width != g_resDat[g_curRes].width
						|| curMode.bpp != g_resDat[g_curRes].bpp)
				{
					//better save out the temporary res. mode
					memcpy(&curMode, &g_tmpMode, sizeof(curMode));

					GFXSetMode(&g_resDat[g_curRes]);
					MenuReloadAll();

					memcpy(&g_tmpMode, &curMode, sizeof(curMode));

					if(g_world)
					{
						g_world->HUDResize();
						g_world->DialogResize();
					}
				}
			}
			break;

		case OPTIONS_BACK:
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
				hMenu->SendItemMessage(OPTIONS_RESOLUTION_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)g_resName[g_curRes], 0);

				hMenu->SendItemMessage(OPTIONS_LIGHTING_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)g_lightStr[(int)g_bVertexLightOpt], 0);
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