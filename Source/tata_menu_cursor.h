#ifndef _tata_menu_cursor_h
#define _tata_menu_cursor_h

#include "tata_menu_item.h"

typedef enum {
	CURSOR_3D,
	CURSOR_2D,
	CURSOR_MAX
} eMenuCursorType;

class MenuCursor {
public:
	MenuCursor(double delay);
	virtual ~MenuCursor();

	void SetCursorLoc(MenuItem * pMItm);		//set the cursor destination to given menu item user ID
	void SetCursorLoc(const D3DXVECTOR3 & loc);	//instant cursor loc change

	void Update();
	virtual void Display();

protected:
	virtual void UpdateSub();

	D3DXVECTOR3 m_cursorCurLoc;		  //current location of cursor

private:
	D3DXVECTOR3 m_cursorDestLoc;	  //destination of cursor
	win32Time	m_cursorDelay;		  //delay for updating cursor location to destination
};

//////////////////////////////////////////////////////////////////////
// Cursor 3D
class MenuCursor3D : public MenuCursor {
public:
	MenuCursor3D(double delay, hCFG cfg);
	~MenuCursor3D();

	void Display();

protected:
	void UpdateSub();

private:
	hOBJ		m_cursor;			  //used to indicate what is currently selected
	Bound		m_cursorBound;		  //bound of cursor
};

//////////////////////////////////////////////////////////////////////
// Cursor 2D
class MenuCursor2D : public MenuCursor {
public:
	MenuCursor2D(double delay, hCFG cfg);
	~MenuCursor2D();

	void Display();

protected:
	void UpdateSub();

private:
	hTXT		m_cursor;	//the 2D cursor!
	D3DXVECTOR2 m_size;
	DWORD		m_clr;
};

#endif