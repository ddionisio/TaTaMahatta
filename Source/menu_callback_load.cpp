#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

#define ID_TEXT_OFS		1000
#define MAX_TEXT		5
#define TEXT_SEL		2

struct loadDat {
	string		name;
	string		path;
};

vector<loadDat> g_loadList;

int g_loadStartInd;

void MenuLoadFileCallback(char *filepath, void *userData)
{
	char name[MAXCHARBUFF]={0};

	GetFilename(filepath, name, MAXCHARBUFF-1);

	loadDat newDat;
	newDat.name = name;
	newDat.path = filepath;

	g_loadList.push_back(newDat);
}

RETCODE MCB_Load(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		{
			g_loadList.clear();
			g_loadList.resize(0);

			char sPath[DMAXCHARBUFF]={0};
			TataGetSaveDir(sPath);

			string pattern = "*";
			pattern += SAVE_EXT;

			//load up the save games
			ParserFindFiles(sPath, (char*)pattern.c_str(), false, MenuLoadFileCallback, 0);

			//this will make current selection as '0'
			g_loadStartInd = -TEXT_SEL;
		}
		break;

	case MENU_MSG_BTN:
		if(lParam == INP_STATE_RELEASED)
		{
			switch(wParam)
			{
			case INP_UP:
				g_loadStartInd--;

				if(g_loadStartInd + TEXT_SEL < 0)
					g_loadStartInd = (g_loadList.size() - 1) - TEXT_SEL;
				break;

			case INP_DOWN:
				g_loadStartInd++;

				if(g_loadStartInd + TEXT_SEL >= g_loadList.size())
					g_loadStartInd = -TEXT_SEL;
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case LOAD_CONFIRM:
			if(g_loadList.size() > 0)
			{
				int txtInd = g_loadStartInd + TEXT_SEL;

				if(txtInd >= 0 && txtInd < g_loadList.size())
				{
					if((*g_loadList[txtInd].name.c_str()) != 0)
					{
						g_world->ProfileLoad(g_loadList[txtInd].name.c_str());
						g_world->ProfilePrepare();

						g_world->HUDActive(true);

						g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

						MenuExitAll();

						g_world->CutsceneRemove();

						//load cloud-in-game menu
						MenuGameAdd(GAMEMENU_PAUSECLOUD);

						InputClear();
					}
				}
			}
			break;

		case LOAD_CANCEL:
			MenuExitCurrent();
			MenuEnableCurrent(true);
			break;
		}
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			if(g_loadList.size() > 0)
			{
				int txtInd, alterInd;
				for(int i = 0; i < MAX_TEXT; i++)
				{
					txtInd = g_loadStartInd + i;

					if(txtInd < 0)
					{
						/*alterInd = g_loadList.size()+txtInd;

						while(alterInd < 0)
						{
							alterInd++;
						}

						hMenu->SendItemMessage(ID_TEXT_OFS+i, MENU_ITM_MSG_SETTEXT, (WPARAM)g_loadList[alterInd].name.c_str(), 0);*/
						hMenu->SendItemMessage(ID_TEXT_OFS+i, MENU_ITM_MSG_SETTEXT, (WPARAM)" ", 0);
					}
					else if(txtInd >= g_loadList.size())
					{
						/*alterInd = txtInd - g_loadList.size();

						while(alterInd >= g_loadList.size())
						{
							alterInd--;
						}

						hMenu->SendItemMessage(ID_TEXT_OFS+i, MENU_ITM_MSG_SETTEXT, (WPARAM)g_loadList[alterInd].name.c_str(), 0);*/
						hMenu->SendItemMessage(ID_TEXT_OFS+i, MENU_ITM_MSG_SETTEXT, (WPARAM)" ", 0);
					}
					else
						hMenu->SendItemMessage(ID_TEXT_OFS+i, MENU_ITM_MSG_SETTEXT, (WPARAM)g_loadList[txtInd].name.c_str(), 0);
				}
			}
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