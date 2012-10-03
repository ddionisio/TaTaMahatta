#ifndef _tata_menu_h
#define _tata_menu_h

#include "tata_ID.h"

#include "tata_view.h"
#include "tata_basic.h"

#include "tata_menu_cursor.h"

#include "tata_menu_item.h"

//flags for menu
#define MENU_FLAG_DISABLE			1	//is menu disabled?  (no draw, no update)
#define MENU_FLAG_DISABLE_ONEND		2	//do we want to disable after exiting state?
#define MENU_FLAG_CURSOR_DISABLE	4	//disable cursor, no update/display
#define MENU_FLAG_NO_DARK_BKGRND	8	//no dark fade on the background
#define MENU_NO_LIGHT				16	//menu does not have light

//This includes all the menu interfaces

//menu state
typedef enum {
	MENU_STATE_NORMAL,
	MENU_STATE_ENTERING,
	MENU_STATE_EXITING
} eMenuState;

//update types
typedef enum {
	MENU_UPDATE_NORMAL,
	MENU_UPDATE_ENTERING,
	MENU_UPDATE_EXITING
} eMenuUpdateType;

typedef enum {
	MENU_SOUND_ACTION,
	MENU_SOUND_CURSOR_MOVE,
	MENU_SOUND_MAX
} eMenuSoundType;

typedef enum {
	MENU_MSG_LOAD,		//load from given config, wParam = hCFG cfg

	MENU_MSG_BTN,		//wParam = eGameInput, lParam = eGameInputState

	MENU_MSG_ITEM,		//item activated, wParam = userID

	MENU_MSG_UPDATE,	//update, wParam = MENU_UPDATE_(NORMAL,ENTERING,EXITING)
						//return RETCODE_BREAK when done
						//break on enter,exit = done with enter,exit

	MENU_MSG_DRAW,		//drawing

	MENU_MSG_ENABLE,	//enable/disable, wParam = 0(disable), 1(enable)

	MENU_MSG_DESTROY	//unload
} eMenuMsg;

typedef class Menu *	hMENU;

//function pointer defines
typedef RETCODE (* MENUPROC) (hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//This includes all the menu interfaces

class Menu : public GameID, public Name, public Flag, public View {
public:
	Menu(const char *filename, MENUPROC callbackFunc);
	~Menu();

	virtual void Load(const char *filename, MENUPROC callbackFunc);
	virtual void Destroy();

	void Reload();

	virtual RETCODE Update();		  //update menu, returns RETCODE_BREAK if ready to die
	virtual RETCODE Display();		  //display menu

	void SetCurItm(unsigned int mID); //set the current menu item by user ID
	void SetCurItm(const Id & ID);	  //set the current menu item by ID

	int GetCurItemID();		//get the current item selected (user ID), -1 if invalid

	MenuItem *GetItem(unsigned int mID); //get the menu item of given user ID

	void Enable(bool bYes);			  //enable or disable
	void Exit();					  //set to exit and prepare to die

	void SetCursorLoc(unsigned int mID);	//set the cursor destination to given menu item user ID

	eMenuState GetState() { return m_mState; }

	hFNT GetFont(int ind) { return m_fonts[ind]; }

	void CursorShow(bool bShow);

	void PlayMenuSound(eMenuSoundType type);

	RETCODE SendItemMessage(DWORD ID, DWORD msg, WPARAM wParam, LPARAM lParam);

protected:
	vector<Id>	m_items;
	int			m_curItem;

private:
	string		m_filePath;
	eMenuState	m_mState;			  //menu state
	
	MenuCursor	*m_pCursor;			  //if not null, a cursor!

	GFXLIGHT	m_light;			  //the light

	vector<hFNT> m_fonts;				//fonts used by several items

	HSAMPLE		m_sounds[MENU_SOUND_MAX];//sounds used by menu (eMenuSoundType)

	MENUPROC	m_callback;			  //user defined callback

	void		_UpdateCursor();	  //update the cursor
	void		_DisplayCursor();	  //display the cursor

	void		_ItemsStateChange();	  //just call the items that we are changing state

	void		_DestroyAllItems();
	void		_DestroyAllFonts();
};

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
PUBLIC RETCODE MenuAdd(const char *filename, MENUPROC callbackFunc);

/////////////////////////////////////
// Name:	MenuEnableCurrent
// Purpose:	enable or disable current
//			menu
// Output:	menu enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void MenuEnableCurrent(bool bEnable);

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
PUBLIC void MenuExitCurrent();

/////////////////////////////////////
// Name:	MenuUpdateCurrent
// Purpose:	update the current menu
//			input and everything...
// Output:	update
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE MenuUpdateCurrent();

/////////////////////////////////////
// Name:	MenuDisplayAll
// Purpose:	displays all the menu
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE MenuDisplayAll();

/////////////////////////////////////
// Name:	MenuReloadAll
// Purpose:	will reload all menus
// Output:	all dead
// Return:	none
/////////////////////////////////////
PUBLIC void MenuReloadAll();

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
PUBLIC void MenuExitAll();

/////////////////////////////////////
// Name:	MenuDestroyAll
// Purpose:	will destroy all the menu
// Output:	all dead
// Return:	none
/////////////////////////////////////
PUBLIC void MenuDestroyAll();

/////////////////////////////////////
// Name:	MenuIsAvailable
// Purpose:	check to see if the given
//			menu name is created
// Output:	none
// Return:	true if menu found
/////////////////////////////////////
PUBLIC bool MenuIsAvailable(const char *name);

#endif