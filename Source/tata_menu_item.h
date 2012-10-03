#ifndef _tata_menu_item_h
#define _tata_menu_item_h

#include "tata_ID.h"

#include "tata_basic.h"

//flags for menu item
#define MENU_ITM_FLAG_SELECT	1	//is item selected?

//type of items...
typedef enum {
	MITM_TYPE_BUTTON3D,	//a normal 3D button
	MITM_TYPE_BUTTON2D,
	MITM_TYPE_BUTTONTEXT,
	MITM_TYPE_MAX
} eMenuItmType;

typedef enum {
	MITM_UP,
	MITM_DOWN,
	MITM_LEFT,
	MITM_RIGHT,
	MITM_MAX
} eMenuItmLink;

//update types
typedef enum {
	MITM_UPDATE_NORMAL,
	MITM_UPDATE_ENTERING,
	MITM_UPDATE_EXITING
} eMenuItmUpdateType;

//messages for items
typedef enum {
	MENU_ITM_MSG_UPDATE,	//update menu item, wParam = eMenuItmUpdateType
							//return RETCODE_BREAK when done
							//break on enter,exit = done with enter,exit

	MENU_ITM_MSG_DRAW,		//drawing

	MENU_ITM_MSG_STATECHANGE, //changing state to normal, entering or exiting...

	MENU_ITM_MSG_SELECT,	//wParam = 0(unselect) or 1(select)

	MENU_ITM_MSG_ACTIVATE,	//what to do on activation (action)

	//PUBLIC MESSAGES
	MENU_ITM_MSG_SETTEXT,	//set the text, wParam = (char*)
	MENU_ITM_MSG_GETTEXT	//get the text, return (const char*)

} eMenuItmMsg;

/********************************************************************
*																	*
*						Menu Item Base Class						*
*																	*
********************************************************************/

//NOTE: name is used as an ID
class MenuItem : public GameID, public Flag {
public:
	MenuItem(const Id & parentID, eMenuItmType type, hCFG cfg, const char *section);
	virtual ~MenuItem();

	eMenuItmType GetType()	{ return m_type; }
	unsigned int GetUserID() { return m_mID; }

	int	GetLinkItem(eMenuItmLink link);

	void SetCursorOffset(const D3DXVECTOR3 & v) { m_cursorOffset = v; }

	void SetScale(const D3DXVECTOR3 & v) { m_scale = v; }

	bool PlayActionSnd();				//play the action sound (returns true if sound is played succefully)
	
	virtual D3DXVECTOR3 GetCursorOffset() { return m_cursorOffset; }

	virtual int Callback(unsigned int msg, unsigned int wParam, int lParam) { return 1; }

protected:
	Id				m_parentID;
	D3DXVECTOR3		m_cursorOffset;		//the cursor offset
	D3DXVECTOR3     m_scale;			//the scale

private:
	eMenuItmType	m_type;				//type of item
	unsigned int	m_mID;			    //user's unique ID for this item
	int				m_link[MITM_MAX];	//link to other menu items via user ID.(-1 = no link)

	HSAMPLE			m_actionSnd;		//the sound the item makes when the button is pressed
};

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
PUBLIC int MenuItemGetType(const char *str);

/////////////////////////////////////
// Name:	MenuItemCreate
// Purpose:	create the menu item based
//			on given 'str'
// Output:	new menu item created
// Return:	the menu item
/////////////////////////////////////
PUBLIC MenuItem *MenuItemCreate(const Id & parentID, const char *str, hCFG cfg, const char *section);

#endif