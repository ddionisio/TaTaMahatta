#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#include "tata_menu_options.h"

int g_joyEnum, g_joyTempEnum;

DWORD g_sVol=VOLUME_MAX, g_stVol=VOLUME_MAX, g_mVol=VOLUME_MAX;

DWORD g_sVolTemp=VOLUME_MAX, g_stVolTemp=VOLUME_MAX, g_mVolTemp=VOLUME_MAX;

bool g_bVertexLightOpt;

gfxDisplayMode g_tmpMode;

//Options
RETCODE MCB_Options(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		//reserve config in case we don't want to save
		g_joyEnum = InputGetJoystickEnumInd();
		g_joyTempEnum = g_joyEnum;
		g_bVertexLightOpt = g_bUseVertexLighting;

		BASS_GetGlobalVolumes(&g_mVol, &g_sVol, &g_stVol);
		BASS_GetGlobalVolumes(&g_mVolTemp, &g_sVolTemp, &g_stVolTemp);

		GFXGetMode(&g_tmpMode);

		InputTemp();
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case OPTIONS_GRAPHIC:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_OPTIONS_GFX);
			MenuEnableCurrent(true);
			break;

		case OPTIONS_AUDIO:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_OPTIONS_AUDIO);
			MenuEnableCurrent(true);
			break;

		case OPTIONS_CONTROL:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_OPTIONS_INPUT);
			MenuEnableCurrent(true);
			break;

		case OPTIONS_CFG_SAVE:
			//save game config...
			g_bUseVertexLighting = g_bVertexLightOpt;

			TataSaveGameCfg();

			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;

		case OPTIONS_CFG_NOSAVE:
			//revert stuff back
			if(g_world)
			{
				if(g_world->GetMap())
					QBSPUseVertexLighting(g_world->GetMap(), g_bUseVertexLighting);
			}

			BASS_SetGlobalVolumes(g_mVolTemp, g_sVolTemp, g_stVolTemp);

			//revert back to old resolution if changed g_tmpMode
			gfxDisplayMode mode;
			GFXGetMode(&mode);

			if(mode.width != g_tmpMode.width || mode.bpp != g_tmpMode.bpp)
			{
				GFXSetMode(&g_tmpMode);
				MenuReloadAll();

				if(g_world)
				{
					g_world->HUDResize();
					g_world->DialogResize();
				}
			}

			InputChangeJoystickDevice(g_joyTempEnum);

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

/*char *g_lightStr[2] = {"Lightmap (best)", "Vertex Lighting"};

//Options
RETCODE MCB_Options(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		//reserve config in case we don't want to save
		g_joyEnum = InputGetJoystickEnumInd();
		g_joyTempEnum = g_joyEnum;
		g_bVertexLightOpt = g_bUseVertexLighting;

		BASS_GetGlobalVolumes(&g_sVol, &g_stVol, &g_mVol);
		BASS_GetGlobalVolumes(&g_sVolTemp, &g_stVolTemp, &g_mVolTemp);

		InputTemp();
		break;

	case MENU_MSG_BTN: //INPXJoystickGetNumEnum()
		if(lParam == INP_STATE_DOWN)
		{
			switch(hMenu->GetCurItemID())
			{
			case OPTIONS_SOUND:
				if(wParam == INP_LEFT && g_sVol > 0)
					g_sVol--;
				else if(wParam == INP_RIGHT && g_sVol < VOLUME_MAX)
					g_sVol++;

				BASS_SetGlobalVolumes(g_sVol, g_stVol, g_mVol);
				break;

			case OPTIONS_MUSIC:
				if(wParam == INP_LEFT && g_stVol > 0)
					g_stVol--;
				else if(wParam == INP_RIGHT && g_stVol < VOLUME_MAX)
					g_stVol++;

				BASS_SetGlobalVolumes(g_sVol, g_stVol, g_mVol);
				break;
			}
		}
		else if(lParam == INP_STATE_RELEASED)
		{
			switch(hMenu->GetCurItemID())
			{
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

			case OPTIONS_JOYSTICK:
				if(wParam == INP_LEFT)
				{
					if(g_joyEnum == 0)
						g_joyEnum = INPXJoystickGetNumEnum()-1;
					else
						g_joyEnum--;
				}
				else if(wParam == INP_RIGHT)
				{
					if(g_joyEnum == INPXJoystickGetNumEnum()-1)
						g_joyEnum = 0;
					else
						g_joyEnum++;
				}

				InputChangeJoystickDevice(g_joyEnum);
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case OPTIONS_CFG_KEYBOARD:
			MenuEnableCurrent(false);

			MenuGameAdd(GAMEMENU_KB_CFG);
			MenuEnableCurrent(true);
			break;

		case OPTIONS_CFG_JOYSTICK:
			if(InputGetJoystick())
			{
				MenuEnableCurrent(false);

				MenuGameAdd(GAMEMENU_JSTICK_CFG);
				MenuEnableCurrent(true);
			}
			break;

		case OPTIONS_CFG_SAVE:
			//save game config...
			g_bUseVertexLighting = g_bVertexLightOpt;

			TataSaveGameCfg();

			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;

		case OPTIONS_CFG_NOSAVE:
			//revert stuff back
			if(g_world)
			{
				if(g_world->GetMap())
					QBSPUseVertexLighting(g_world->GetMap(), g_bUseVertexLighting);
			}

			BASS_SetGlobalVolumes(g_sVolTemp, g_stVolTemp, g_mVolTemp);

			InputChangeJoystickDevice(g_joyTempEnum);

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
				hMenu->SendItemMessage(OPTIONS_LIGHTING_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)g_lightStr[(int)g_bVertexLightOpt], 0);

				if(g_joyEnum != -1 && INPXJoystickGetNumEnum() > 0)
				{
					char buff[MAXCHARBUFF]={0};
					INPXJoystickGetNameEnum(g_joyEnum, buff);

					hMenu->SendItemMessage(OPTIONS_JOYSTICK_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)buff, 0);
				}
				else
				{
					hMenu->SendItemMessage(OPTIONS_JOYSTICK_TEXT, MENU_ITM_MSG_SETTEXT, 
					(WPARAM)"none", 0);
				}

				D3DXVECTOR3 sVec(((float)g_sVol)/VOLUME_MAX, 1, 1);
				D3DXVECTOR3 mVec(((float)g_stVol)/VOLUME_MAX, 1, 1);

				MenuItem *pMItm = hMenu->GetItem(OPTIONS_SOUND_BAR);
				
				if(pMItm)
					pMItm->SetScale(sVec);

				pMItm = hMenu->GetItem(OPTIONS_MUSIC_BAR);
				
				if(pMItm)
					pMItm->SetScale(mVec);
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
}*/