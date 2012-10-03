#include "tata_main.h"

#include "tata_menu_item.h"
#include "tata_menu_item_common.h"

char *g_menuItemNames[MITM_TYPE_MAX] = {"Button3D", "Button2D", "ButtonText"};

MenuItem::MenuItem(const Id & parentID, eMenuItmType type, hCFG cfg, const char *section)
: GameID(this), m_parentID(parentID), m_type(type), m_cursorOffset(0,0,0), m_mID(0),
  m_scale(1,1,1), m_actionSnd(0)
{
	char buff[MAXCHARBUFF]={0};

	//load user ID
	int mID = CfgGetItemInt(cfg, section, "ID");

	if(mID >= 0)
		m_mID = mID;

	//load cursor offset
	if(CfgGetItemStr(cfg, section, "cursorOfs", buff))
		sscanf(buff, "%f,%f,%f", &m_cursorOffset.x, &m_cursorOffset.y, &m_cursorOffset.z);

	//set links to -1
	for(int i = 0; i < MITM_MAX; i++)
		m_link[i] = -1;

	//load links
	if(CfgGetItemStr(cfg, section, "links", buff))
		sscanf(buff, "%d,%d,%d,%d", &m_link[MITM_UP], &m_link[MITM_DOWN], &m_link[MITM_LEFT], &m_link[MITM_RIGHT]);

	//load sound
	if(CfgGetItemStr(cfg, section, "actionSnd", buff))
	{
		string sndPath(GAMEFOLDER);
		sndPath += "\\";
		sndPath += buff;

		m_actionSnd = BASS_SampleLoad(FALSE, (void *)sndPath.c_str(), 0, 0, 10, 0);
	}
}

MenuItem::~MenuItem()
{
}

int	MenuItem::GetLinkItem(eMenuItmLink link)
{
	return m_link[link];
}

bool MenuItem::PlayActionSnd()
{
	if(m_actionSnd)
	{
		if(BASS_SamplePlay(m_actionSnd))
			return true;
	}

	return false;
}

/********************************************************************
*																	*
*							Functions								*
*																	*
********************************************************************/

/////////////////////////////////////
// Name:	MenuItemGetType
// Purpose:	get the enum type of given
//			string.
// Output:	none
// Return:	item type, -1 if not found
/////////////////////////////////////
PUBLIC int MenuItemGetType(const char *str)
{
	for(int i = 0; i < MITM_TYPE_MAX; i++)
	{
		if(stricmp(g_menuItemNames[i], str) == 0)
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	MenuItemCreate
// Purpose:	create the menu item based
//			on given 'str'
// Output:	new menu item created
// Return:	the menu item
/////////////////////////////////////
PUBLIC MenuItem *MenuItemCreate(const Id & parentID, const char *str, hCFG cfg, const char *section)
{
	int type = MenuItemGetType(str);

	MenuItem *newMenuItem = 0;

	switch(type)
	{
	case MITM_TYPE_BUTTON3D:
		newMenuItem = new MTButton3D(parentID, cfg, section);
		break;

	case MITM_TYPE_BUTTON2D:
		newMenuItem = new MTButton2D(parentID, cfg, section);
		break;

	case MITM_TYPE_BUTTONTEXT:
		newMenuItem = new MTButtonText(parentID, cfg, section);
		break;
	}

	return newMenuItem;
}