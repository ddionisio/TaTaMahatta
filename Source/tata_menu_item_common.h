#ifndef _tata_menu_item_common_h
#define _tata_menu_item_common_h

#include "tata_menu_item.h"

//This holds all menu item types

///////////////////////////////////////////////////////////////////////////
// Button 3D
class MTButton3D : public MenuItem {
public:
	MTButton3D(const Id & parentID, hCFG cfg, const char *section);
	~MTButton3D();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	D3DXVECTOR3 GetCursorOffset();

private:
	hOBJ			m_btnOBJ;		//the object representation
	D3DXVECTOR3		m_startLoc;		//the starting loc. once loaded
	D3DXVECTOR3		m_endLoc;		//the ending loc.
	float			m_startAlpha;	//the starting alpha [0,1]
	float			m_endAlpha;		//the ending alpha [0,1]
	win32Time		m_delay;		//delay for entering and exiting
};

///////////////////////////////////////////////////////////////////////////
// Button Image 2D
class MTButton2D : public MenuItem {
public:
	MTButton2D(const Id & parentID, hCFG cfg, const char *section);
	~MTButton2D();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	D3DXVECTOR3 GetCursorOffset();

private:
	hTXT			m_btnImg;		//the image representation
	D3DXVECTOR2		m_startLoc;		//the starting loc. once loaded
	D3DXVECTOR2		m_endLoc;		//the ending loc.
	D3DXVECTOR2		m_curLoc;		//current location
	D3DXVECTOR2		m_size;			//size
	DWORD			m_clr;			//
	float			m_startAlpha;	//the starting alpha [0,255]
	float			m_endAlpha;		//the ending alpha [0,255]
	win32Time		m_delay;		//delay for entering and exiting
};

///////////////////////////////////////////////////////////////////////////
// Button Text
class MTButtonText : public MenuItem {
public:
	MTButtonText(const Id & parentID, hCFG cfg, const char *section);
	~MTButtonText();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	D3DXVECTOR3 GetCursorOffset();

private:
	int				m_fntInd;		//font index in parent
	string			m_str;			//the string to display
	D3DXVECTOR2		m_startLoc;		//the starting loc. once loaded
	D3DXVECTOR2		m_endLoc;		//the ending loc.

	D3DXVECTOR2		m_curLoc;		//current location

	DWORD			m_clr;			//
	BYTE			m_r,m_g,m_b;	//red,green,blue
	float			m_startAlpha;	//the starting alpha [0,255]
	float			m_endAlpha;		//the ending alpha [0,255]
	win32Time		m_delay;		//delay for entering and exiting

	bool			m_bCenter;		//font centered?
};

#endif