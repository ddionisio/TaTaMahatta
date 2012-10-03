#include "tata_main.h"

#include "tata_menu.h"
#include "tata_menu_cursor.h"
#include "tata_menu_item.h"

#define CURSOR_DELAY	500

//sound names
const char *g_soundNames[MENU_SOUND_MAX] = {"action", "move"};

//global stack of menus
list< Id > g_menus;

/*
HSAMPLE		m_actionSound;			//sound when a button is pressed
	HSAMPLE		m_moveSound;			//sound when cursor moves
*/

void Menu::Load(const char *filename, MENUPROC callbackFunc)
{
	m_filePath = filename;

	int i = 0;

	//set sounds to zero
	memset(m_sounds, 0, sizeof(m_sounds));

	//get the name
	char buff[MAXCHARBUFF]={0};
	GetFilename(filename, buff, MAXCHARBUFF-1);
	SetName(buff);

	//load the menu 'config' file
	hCFG cfg = CfgFileLoad(filename);

	if(cfg)
	{
		//load the main stuff

		/////////////////////////////////////////////////////////////////
		//load the cursor
		int cursorType = CfgGetItemInt(cfg, "main", "cursorType");

		switch(cursorType)
		{
		case CURSOR_3D:
			m_pCursor = new MenuCursor3D(CURSOR_DELAY, cfg);
			break;

		case CURSOR_2D:
			m_pCursor = new MenuCursor2D(CURSOR_DELAY, cfg);
			break;
		}

		D3DXVECTOR3 v;

		/////////////////////////////////////////////////////////////////
		//load the camera stuff
		if(CfgGetItemStr(cfg, "main", "cameraLoc", buff))
		{
			sscanf(buff, "%f,%f,%f", &v.x,&v.y,&v.z);
			ViewSetNewPt(v);
		}

		if(CfgGetItemStr(cfg, "main", "cameraTarget", buff))
		{
			sscanf(buff, "%f,%f,%f", &v.x,&v.y,&v.z);
			ViewSetNewTarget(v);
		}
		/////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////
		//set the light
		memset(&m_light, 0, sizeof(m_light));

		m_light.Type = D3DLIGHT_DIRECTIONAL;
		m_light.Attenuation0 = 1.0f;

		if(CfgGetItemStr(cfg, "main", "lightDiffuse", buff))
			sscanf(buff, "%f,%f,%f,%f", &m_light.Diffuse.r, &m_light.Diffuse.g, &m_light.Diffuse.b, &m_light.Diffuse.a);

		if(CfgGetItemStr(cfg, "main", "lightSpecular", buff))
			sscanf(buff, "%f,%f,%f,%f", &m_light.Specular.r, &m_light.Specular.g, &m_light.Specular.b, &m_light.Specular.a);

		if(CfgGetItemStr(cfg, "main", "lightAmbient", buff))
			sscanf(buff, "%f,%f,%f,%f", &m_light.Ambient.r, &m_light.Ambient.g, &m_light.Ambient.b, &m_light.Ambient.a);

		if(CfgGetItemStr(cfg, "main", "lightDir", buff))
			sscanf(buff, "%f,%f,%f", &m_light.Direction.x, &m_light.Direction.y, &m_light.Direction.z);
		else
			SetFlag(MENU_NO_LIGHT, true);
		/////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////
		//get starting item (in terms of index, not userID)
		m_curItem = CfgGetItemInt(cfg, "main", "startItmInd");

		/////////////////////////////////////////////////////////////////
		//load fonts
		int fntInd = 0;
		float fntSize;
		char fntItmStr[MAXCHARBUFF], fntStr[MAXCHARBUFF], *pBuff;

		while(1)
		{
			sprintf(fntItmStr, "font_%d", fntInd);

			if(CfgItemExist(cfg, "fonts", fntItmStr))
			{
				if(CfgGetItemStr(cfg, "fonts", fntItmStr, buff))
				{
					pBuff = (char*)buff;

					ParserReadStringBuff(&pBuff, fntStr, MAXCHARBUFF, '"', '"');
					ParserSkipCharBuff(&pBuff, ',');

					sscanf(pBuff, "%f", &fntSize);
					fntSize *= GFXGetScrnSize().cx;

					hFNT newFnt = FontCreate(fntStr, fntSize, 0);

					if(newFnt)
						m_fonts.push_back(newFnt);
				}
				fntInd++;
			}
			else
				break;
		}

		/////////////////////////////////////////////////////////////////
		//load sounds
		string sndPath;

		for(i = 0; i < MENU_SOUND_MAX; i++)
		{
			if(CfgGetItemStr(cfg, "sound", g_soundNames[i], buff))
			{
				sndPath = GAMEFOLDER;
				sndPath += "\\";
				sndPath += buff;

				m_sounds[i] = BASS_SampleLoad(FALSE, (void *)sndPath.c_str(), 0, 0, 10, 0);
			}
		}

		/////////////////////////////////////////////////////////////////
		//load items
		_DestroyAllItems();

		i = 0;

		//load 'em
		while(1)
		{
			sprintf(buff, "itm_%d", i);

			if(CfgSectionExist(cfg, buff))
			{
				//get type
				char menuTypeName[MAXCHARBUFF]={0};

				if(CfgGetItemStr(cfg, buff, "type", menuTypeName))
				{					
					//load item
					MenuItem * newItm = MenuItemCreate(GetID(), menuTypeName, cfg, buff);

					//add to the list
					if(newItm)
						m_items.push_back(newItm->GetID());
				}

				i++;
			}
			else
				break;
		}

		//set cursor location
		if(m_curItem >= 0 && m_curItem < m_items.size())
		{
			MenuItem *pMenuItm = (MenuItem *)IDPageQuery(m_items[m_curItem]);

			if(pMenuItm)
			{
				if(m_pCursor)
					m_pCursor->SetCursorLoc(pMenuItm->GetCursorOffset());

				//call item and tell it that we are selecting it.
				pMenuItm->Callback(MENU_ITM_MSG_SELECT, 1, 0);
			}
		}

		//load custom stuff
		m_callback(this, MENU_MSG_LOAD, (WPARAM)cfg, 0);

		CfgFileDestroy(&cfg);
	}
}

void Menu::Destroy()
{
	//call callback for destruction
	m_callback(this, MENU_MSG_DESTROY, 0, 0);

	_DestroyAllItems();

	_DestroyAllFonts();

	//destroy sounds
	for(int i = 0; i < MENU_SOUND_MAX; i++)
	{
		if(m_sounds[i])
			BASS_SampleFree(m_sounds[i]);
	}

	//destroy cursor
	if(m_pCursor)
		delete m_pCursor;
}

void Menu::Reload()
{
	Destroy();
	Load(m_filePath.c_str(), m_callback);
}

Menu::Menu(const char *filename, MENUPROC callbackFunc)
 : GameID(this), m_mState(MENU_STATE_NORMAL), m_pCursor(0), m_curItem(0),
m_callback(callbackFunc)
{
	Load(filename, callbackFunc);

	//set to currently disabled
	//the user must activate the menu
	//Enable(false);
	SetFlag(MENU_FLAG_DISABLE, true);
}

void Menu::_DestroyAllItems()
{
	MenuItem *pMenuItm;

	//go through all the items and delete them.
	for(int i = 0; i < m_items.size(); i++)
	{
		pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

		if(pMenuItm)
			delete pMenuItm;
	}

	m_items.clear();
}

void Menu::_DestroyAllFonts()
{
	//destroy fonts
	for(int i = 0; i < m_fonts.size(); i++)
	{
		if(m_fonts[i])
			FontDestroy(&m_fonts[i]);
	}

	m_fonts.resize(0);
}

Menu::~Menu()
{
	Destroy();
}

void Menu::_UpdateCursor()
{
	if(CheckFlag(MENU_FLAG_CURSOR_DISABLE))
		return;

	if(m_pCursor)
		m_pCursor->Update();
}

RETCODE Menu::Update()
{
	if(!CheckFlag(MENU_FLAG_DISABLE))
	{
		//update view
		ViewUpdate();

		//update the cursor
		_UpdateCursor();

		bool bMainDone = false;
		int i;
		MenuItem *pMenuItm;

		switch(m_mState)
		{
		case MENU_STATE_NORMAL:
			{
				//////////////////////////////////////////////////////////////////
				//check for input
				for(i = 0; i < INP_MAX; i++)
				{
					if(InputIsDown((eGameInput)i))
						m_callback(this, MENU_MSG_BTN, i, INP_STATE_DOWN);
					else if(InputIsReleased((eGameInput)i))
						m_callback(this, MENU_MSG_BTN, i, INP_STATE_RELEASED);
				}

				//////////////////////////////////////////////////////////////////
				//check to see if we are selecting another item
				if(m_items.size() > 0 && !CheckFlag(MENU_FLAG_CURSOR_DISABLE))
				{
					pMenuItm = (MenuItem *)IDPageQuery(m_items[m_curItem]);

					if(pMenuItm)
					{
						int nextMID = -1;
						
						if(InputIsReleased(INP_UP))
							nextMID = pMenuItm->GetLinkItem(MITM_UP);
						else if(InputIsReleased(INP_DOWN))
							nextMID = pMenuItm->GetLinkItem(MITM_DOWN);

						if(InputIsReleased(INP_LEFT))
							nextMID = pMenuItm->GetLinkItem(MITM_LEFT);
						else if(InputIsReleased(INP_RIGHT))
							nextMID = pMenuItm->GetLinkItem(MITM_RIGHT);

						if(nextMID != -1)
						{
							PlayMenuSound(MENU_SOUND_CURSOR_MOVE);

							//unselect the previous and set the new item
							pMenuItm->Callback(MENU_ITM_MSG_SELECT, 0, 0);

							SetCurItm(nextMID);

							SetCursorLoc(nextMID);

							pMenuItm = (MenuItem *)IDPageQuery(m_items[m_curItem]);

							if(pMenuItm)
								pMenuItm->Callback(MENU_ITM_MSG_SELECT, 1, 0);
						}
					}

					//////////////////////////////////////////////////////////////////
					//check for button press
					if(InputAnyBtnReleased()
						|| INPXKbIsReleased(DIK_RETURN))
					{
						pMenuItm = (MenuItem *)IDPageQuery(m_items[m_curItem]);

						if(pMenuItm)
						{
							//play the item's sound,
							//if not, then play the menu action sound
							if(!pMenuItm->PlayActionSnd())
								PlayMenuSound(MENU_SOUND_ACTION);

							//call activation on button
							pMenuItm->Callback(MENU_ITM_MSG_ACTIVATE, 0, 0);

							//call callback for item activation
							m_callback(this, MENU_MSG_ITEM, pMenuItm->GetUserID(), 0);
						}

						InputClear();
					}
				}

				//update the user stuff
				m_callback(this, MENU_MSG_UPDATE, MENU_UPDATE_NORMAL, 0);

				//////////////////////////////////////////////////////////////////
				//update all items
				for(i = 0; i < m_items.size(); i++)
				{
					pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

					if(pMenuItm)
						pMenuItm->Callback(MENU_ITM_MSG_UPDATE, MITM_UPDATE_NORMAL, 0);
				}
			}
			break;

		//if we are entering, just update the items as 'entering'
		case MENU_STATE_ENTERING:
			{
				int numDone = 0;

				if(m_callback(this, MENU_MSG_UPDATE, MENU_UPDATE_ENTERING, 0) == RETCODE_BREAK)
					bMainDone = true;
				else
					bMainDone = false;
				
				//go through all the items and update as entering
				for(i = 0; i < m_items.size(); i++)
				{
					pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

					//returning RETCODE_BREAK means done entering
					if(pMenuItm)
					{
						if(pMenuItm->Callback(MENU_ITM_MSG_UPDATE, MITM_UPDATE_ENTERING, 0) == RETCODE_BREAK)
							numDone++;
					}
					else
						numDone++;
				}

				//are all done?
				if(numDone >= m_items.size() && bMainDone)
				{
					m_mState = MENU_STATE_NORMAL;
					SetFlag(MENU_FLAG_DISABLE, false); //just to be sure
				}
			}
			break;

		case MENU_STATE_EXITING:
			{
				int numDone = 0;

				if(m_callback(this, MENU_MSG_UPDATE, MENU_UPDATE_EXITING, 0) == RETCODE_BREAK)
					bMainDone = true;
				else
					bMainDone = false;

				//go through all the items and update as exiting
				for(i = 0; i < m_items.size(); i++)
				{
					pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

					//returning RETCODE_BREAK means done entering
					if(pMenuItm
						&& pMenuItm->Callback(MENU_ITM_MSG_UPDATE, MITM_UPDATE_EXITING, 0)
						== RETCODE_BREAK)
						numDone++;
				}

				//are all done?
				if(numDone == m_items.size() && bMainDone)
				{
					m_mState = MENU_STATE_NORMAL;

					//do we just want to be disabled?
					if(CheckFlag(MENU_FLAG_DISABLE_ONEND))
					{
						SetFlag(MENU_FLAG_DISABLE_ONEND, false);
						SetFlag(MENU_FLAG_DISABLE, true);
					}
					else
						//ready to die
						return RETCODE_BREAK;
				}
			}
			break;
		}
	}

	return RETCODE_SUCCESS;
}

void Menu::_DisplayCursor()
{
	if(!m_pCursor || CheckFlag(MENU_FLAG_CURSOR_DISABLE))
		return;

	//just display it
	if(m_mState == MENU_STATE_NORMAL)
		m_pCursor->Display();
}

RETCODE Menu::Display()
{
	if(!CheckFlag(MENU_FLAG_DISABLE))
	{
		//darken the background
		if(m_mState == MENU_STATE_NORMAL && !CheckFlag(MENU_FLAG_NO_DARK_BKGRND))
		{
			RECT r = {0,0,GFXGetScrnSize().cx,GFXGetScrnSize().cy};
			GFXDrawBox2D(&r, D3DCOLOR_RGBA(0,0,0,128));
		}

		//set light
		if(!CheckFlag(MENU_NO_LIGHT))
			LightSet(0, &m_light, TRUE);

		//clear depth buffer
		GFXClearDepth();

		//set view
		ViewSetScene();

		//call display to callback
		m_callback(this, MENU_MSG_DRAW, 0, 0);

		//display the items
		MenuItem *pMenuItm;
		for(int i = 0; i < m_items.size(); i++)
		{
			pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

			if(pMenuItm)
				pMenuItm->Callback(MENU_ITM_MSG_DRAW, 0, 0);
		}

		//display cursor
		_DisplayCursor();
	}

	return RETCODE_SUCCESS;
}

void Menu::SetCurItm(unsigned int mID)
{
	MenuItem *pMenuItm;

	//go through all the items and delete them.
	for(int i = 0; i < m_items.size(); i++)
	{
		pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

		if(pMenuItm && pMenuItm->GetUserID() == mID)
		{
			m_curItem = i;
			break;
		}
	}
}

void Menu::SetCurItm(const Id & ID)
{
	MenuItem *pMenuItm;

	//go through all the items and delete them.
	for(int i = 0; i < m_items.size(); i++)
	{
		pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

		if(pMenuItm && GAME_IDISEQUAL(pMenuItm->GetID(), ID))
		{
			m_curItem = i;
			break;
		}
	}
}

int Menu::GetCurItemID()
{
	MenuItem *pMenuItm = (MenuItem *)IDPageQuery(m_items[m_curItem]);

	if(pMenuItm)
		return pMenuItm->GetUserID();

	return -1;
}

MenuItem * Menu::GetItem(unsigned int mID)
{
	MenuItem *pMenuItm = 0;

	//go through all the items and delete them.
	for(int i = 0; i < m_items.size(); i++)
	{
		pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

		if(pMenuItm && pMenuItm->GetUserID() == mID)
			break;
	}

	return pMenuItm;
}

void Menu::Enable(bool bYes)
{
	if(bYes)
	{
		m_mState = MENU_STATE_ENTERING;
		SetFlag(MENU_FLAG_DISABLE, false);
		SetFlag(MENU_FLAG_DISABLE_ONEND, false);

		m_callback(this, MENU_MSG_ENABLE, 1, 0);
	}
	else if(!CheckFlag(MENU_FLAG_DISABLE))
	{
		m_mState = MENU_STATE_EXITING;
		SetFlag(MENU_FLAG_DISABLE_ONEND, true);

		m_callback(this, MENU_MSG_ENABLE, 0, 0);
	}
	else
		return;

	_ItemsStateChange();
}

void Menu::Exit()
{
	m_mState = MENU_STATE_EXITING;
	_ItemsStateChange();
}

void Menu::SetCursorLoc(unsigned int mID)
{
	if(m_pCursor)
	{
		MenuItem * pMItm = GetItem(mID);
		m_pCursor->SetCursorLoc(pMItm);
	}
}

void Menu::_ItemsStateChange()
{
	MenuItem *pMenuItm;
	for(int i = 0; i < m_items.size(); i++)
	{
		pMenuItm = (MenuItem *)IDPageQuery(m_items[i]);

		if(pMenuItm)
			pMenuItm->Callback(MENU_ITM_MSG_STATECHANGE, 0, 0);
	}
}

void Menu::CursorShow(bool bShow)
{
	SetFlag(MENU_FLAG_CURSOR_DISABLE, !bShow);
}

void Menu::PlayMenuSound(eMenuSoundType type)
{
	if(m_sounds[type])
		BASS_SamplePlay(m_sounds[type]);
}

RETCODE Menu::SendItemMessage(DWORD ID, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	MenuItem * pMItm = GetItem(ID);

	if(pMItm)
		return pMItm->Callback(msg, wParam, lParam);

	return RETCODE_FAILURE;
}

/********************************************************************
*																	*
*							Functions								*
*																	*
********************************************************************/

/////////////////////////////////////
// Name:	MenuAdd
// Purpose:	add a menu, defaults as
//			disabled.  This will be
//			the new menu controlled in
//			the game.
// Output:	Menu added
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE MenuAdd(const char *filename, MENUPROC callbackFunc)
{
	Menu *newMenu = new Menu(filename, callbackFunc);

	g_menus.push_front(newMenu->GetID());

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MenuEnableCurrent
// Purpose:	enable or disable current
//			menu
// Output:	menu enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void MenuEnableCurrent(bool bEnable)
{
	/*Menu *pMenu = (Menu *)IDPageQuery(g_menus.front());

	if(pMenu)
		pMenu->Enable(bEnable);*/
	Menu *pMenu;

	for(list< Id >::iterator i = g_menus.begin(); i != g_menus.end(); i++)
	{
		pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu)
		{
			if(pMenu->GetState() == MENU_UPDATE_NORMAL)
			{
				pMenu->Enable(bEnable);
				return;
			}
		}
	}
}

/////////////////////////////////////
// Name:	MenuExitCurrent
// Purpose:	this will exit the current
//			menu.  The menu before the
//			current will then be the
//			new controlled menu.  You
//			must manually enable the new
//			current.
// Output:	delete the current
// Return:	none
/////////////////////////////////////
PUBLIC void MenuExitCurrent()
{
	if(g_menus.size() > 0)
	{
		Menu *pMenu = (Menu *)IDPageQuery(*g_menus.begin());

		if(pMenu)
			pMenu->Exit();
	}
}

/////////////////////////////////////
// Name:	MenuUpdateCurrent
// Purpose:	update the current menu
//			input and everything...
// Output:	update
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE MenuUpdateCurrent()
{
	if(g_menus.size() > 0)
	{
		//update the first
		list< Id >::iterator i = g_menus.begin(), iNext;

		iNext = i;
		iNext++;

		Menu *pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu)
		{
			if(pMenu->Update() == RETCODE_BREAK)
			{
				delete pMenu;
				g_menus.pop_front();
			}
		}

		//update the rest, only if they are entering or exiting

		i = iNext;

		for(; i != g_menus.end();)
		{
			iNext = i;
			iNext++;

			pMenu = (Menu *)IDPageQuery(*i);

			if(pMenu)
			{
				if(pMenu->GetState() == MENU_UPDATE_ENTERING
					|| pMenu->GetState() == MENU_UPDATE_EXITING)
				{
					if(pMenu->Update() == RETCODE_BREAK)
					{
						delete pMenu;
						g_menus.erase(i);
					}
				}
				//else
				//	pMenu->Enable(false);
			}

			i = iNext;
		}
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MenuDisplayAll
// Purpose:	displays all the menu
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE MenuDisplayAll()
{
	Menu *pMenu;

	LightReset();

	//GFXBegin();

	for(list< Id >::iterator i = g_menus.begin(); i != g_menus.end(); i++)
	{
		pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu)
			pMenu->Display();
	}

	//GFXEnd();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MenuExitAll
// Purpose:	set all menus to exit,
//			they will be destroyed once
//			they are done exiting.
//			NOTE: this is similar to
//			MenuDestroyAll, but will not
//			instantly delete all menu.
// Output:	all menus exit.
// Return:	none
/////////////////////////////////////
PUBLIC void MenuExitAll()
{
	Menu *pMenu;

	for(list< Id >::iterator i = g_menus.begin(); i != g_menus.end(); i++)
	{
		pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu)
			pMenu->Exit();
	}
}

/////////////////////////////////////
// Name:	MenuReloadAll
// Purpose:	will reload all menus
// Output:	all dead
// Return:	none
/////////////////////////////////////
PUBLIC void MenuReloadAll()
{
	Menu * pMenu;
	for(list< Id >::iterator i = g_menus.begin(); i != g_menus.end(); i++)
	{
		pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu)
			pMenu->Reload();
	}

	MenuEnableCurrent(true);
}

/////////////////////////////////////
// Name:	MenuDestroyAll
// Purpose:	will destroy all the menu
// Output:	all dead
// Return:	none
/////////////////////////////////////
PUBLIC void MenuDestroyAll()
{
	Menu * pMenu;
	for(list< Id >::iterator i = g_menus.begin(); i != g_menus.end(); i++)
	{
		pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu)
			delete pMenu;
	}

	g_menus.clear();
}

/////////////////////////////////////
// Name:	MenuIsAvailable
// Purpose:	check to see if the given
//			menu name is created
// Output:	none
// Return:	true if menu found
/////////////////////////////////////
PUBLIC bool MenuIsAvailable(const char *name)
{
	Menu *pMenu;

	for(list< Id >::iterator i = g_menus.begin(); i != g_menus.end(); i++)
	{
		pMenu = (Menu *)IDPageQuery(*i);

		if(pMenu && strcmp(pMenu->GetName(), name) == 0)
			return true;
	}

	return false;
}