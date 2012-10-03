#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#include "tata_menu_options.h"

//Options
RETCODE MCB_Options_Audio(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		break;

	case MENU_MSG_BTN:
		if(lParam == INP_STATE_DOWN)
		{
			switch(hMenu->GetCurItemID())
			{
			case OPTIONS_SOUND:
				if(wParam == INP_LEFT && g_sVol > 0)
					g_sVol--;
				else if(wParam == INP_RIGHT && g_sVol < VOLUME_MAX)
					g_sVol++;

				BASS_SetGlobalVolumes(g_mVol, g_sVol, g_stVol);
				break;

			case OPTIONS_MUSIC:
				if(wParam == INP_LEFT && g_stVol > 0)
				{
					g_stVol--;
					g_mVol--;
				}
				else if(wParam == INP_RIGHT && g_stVol < VOLUME_MAX)
				{
					g_stVol++;
					g_mVol++;
				}

				BASS_SetGlobalVolumes(g_mVol, g_sVol, g_stVol);
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
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
}