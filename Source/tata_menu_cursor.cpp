#include "tata_main.h"

#include "tata_menu.h"
#include "tata_menu_item.h"
#include "tata_menu_cursor.h"

/*
class MenuCursor {
public:
	MenuCursor(double delay);
	virtual ~MenuCursor();

	void SetCursorLoc(unsigned int mID);	//set the cursor destination to given menu item user ID
	void SetCursorLoc(D3DXVECTOR3 loc);		//instant cursor loc change

	void Update();
	virtual void Display();

protected:
	virtual void UpdateSub();

	D3DXVECTOR3 m_cursorCurLoc;		  //current location of cursor

private:
	D3DXVECTOR3 m_cursorDestLoc;	  //destination of cursor
	win32Time	m_cursorDelay;		  //delay for updating cursor location to destination
};

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
};*/

MenuCursor::MenuCursor(double delay) : m_cursorCurLoc(0,0,0),
m_cursorDestLoc(0,0,0)
{
	TimeInit(&m_cursorDelay, delay);
}

MenuCursor::~MenuCursor()
{
}

void MenuCursor::SetCursorLoc(MenuItem * pMItm)
{
	if(pMItm)
	{
		double t = TimeGetTime(&m_cursorDelay)/TimeGetDelay(&m_cursorDelay);

		if(t > 1)
			t = 1;

		D3DXVec3Lerp(&m_cursorCurLoc, &m_cursorCurLoc, &m_cursorDestLoc, t);

		TimeReset(&m_cursorDelay);

		m_cursorDestLoc = pMItm->GetCursorOffset();
	}
}

void MenuCursor::SetCursorLoc(const D3DXVECTOR3 & loc)
{
	m_cursorCurLoc = m_cursorDestLoc = loc;
}

void MenuCursor::Update()
{
	/////////////////////////////////////////////////////////////
	//go to the destination
	double t = TimeGetTime(&m_cursorDelay)/TimeGetDelay(&m_cursorDelay);

	if(t > 1)
	{
		t = 1;
		TimeReset(&m_cursorDelay);
	}

	D3DXVec3Lerp(&m_cursorCurLoc, &m_cursorCurLoc, &m_cursorDestLoc, t);

	UpdateSub();
}

void MenuCursor::Display()
{
}

void MenuCursor::UpdateSub()
{
}

/********************************************************************
*																	*
*							Cursor 3D								*
*																	*
********************************************************************/
#define CURSOR3D_SECTION	"cursor3D"
/*
hOBJ		m_cursor;			  //used to indicate what is currently selected
	Bound		m_cursorBound;		  //bound of cursor
*/
MenuCursor3D::MenuCursor3D(double delay, hCFG cfg) : MenuCursor(delay), m_cursor(0)
{
	if(cfg)
	{
		char buff[MAXCHARBUFF]={0};

		/////////////////////////////////////////////////////////////////
		//load the cursor
		hMDL cursorMdl=0;

		if(CfgGetItemStr(cfg, CURSOR3D_SECTION, "cursorMDL", buff))
		{
			string mdlPath = GAMEFOLDER;
			mdlPath += "\\";
			mdlPath += buff;

			cursorMdl = MDLCreate(0, mdlPath.c_str());
		}

		//load animation of cursor and the model
		if(CfgGetItemStr(cfg, CURSOR3D_SECTION, "cursorAnim", buff) && cursorMdl)
		{
			string mdlAnimPath = GAMEFOLDER;
			mdlAnimPath += "\\";
			mdlAnimPath += buff;

			//create the object
			m_cursor = OBJCreate(0, cursorMdl, 0,0,0, eOBJ_ANIMATE, mdlAnimPath.c_str());
		}
		else if(cursorMdl)
			m_cursor = OBJCreate(0, cursorMdl, 0,0,0, eOBJ_STILL, 0);

		//we don't want it to be shown with the world
		//we will update and display the object manually
		if(m_cursor)
			OBJActivate(m_cursor, false);

		//load cursor bounds
		D3DXVECTOR3 v;

		if(CfgGetItemStr(cfg, CURSOR3D_SECTION, "cursorBoundMin", buff))
		{
			sscanf(buff, "%f,%f,%f", &v.x,&v.y,&v.z);
			m_cursorBound.BoundSetMin(v);
		}

		if(CfgGetItemStr(cfg, CURSOR3D_SECTION, "cursorBoundMax", buff))
		{
			sscanf(buff, "%f,%f,%f", &v.x,&v.y,&v.z);
			m_cursorBound.BoundSetMax(v);
		}
		/////////////////////////////////////////////////////////////////
	}
}

MenuCursor3D::~MenuCursor3D()
{
	if(m_cursor)
		OBJDestroy(&m_cursor);
}

void MenuCursor3D::Display()
{
	//just display it
	if(m_cursor)
		OBJDisplay(m_cursor);
}

void MenuCursor3D::UpdateSub()
{
	/////////////////////////////////////////////////////////////
	//update object
	if(m_cursor)
	{
		OBJTranslate(m_cursor, (float*)m_cursorCurLoc, false);
		OBJUpdateFrame(m_cursor);
	}
}

/********************************************************************
*																	*
*							Cursor 2D								*
*																	*
********************************************************************/
#define CURSOR2D_SECTION	"cursor2D"

MenuCursor2D::MenuCursor2D(double delay, hCFG cfg) : MenuCursor(delay),
m_cursor(0), m_size(0,0), m_clr(0xffffffff)
{
	if(cfg)
	{
		char buff[MAXCHARBUFF];

		//Get the image
		if(CfgGetItemStr(cfg, CURSOR2D_SECTION, "img", buff))
		{
			string imgPath = GAMEFOLDER;
			imgPath += "\\";
			imgPath += buff;

			m_cursor = TextureCreate(0, imgPath.c_str(), false, 0);
		}

		//Get the size
		if(CfgGetItemStr(cfg, CURSOR2D_SECTION, "size", buff))
		{
			sscanf(buff, "%f,%f", &m_size.x, &m_size.y);

			m_size.x *= GFXGetScrnSize().cx;
			m_size.y *= GFXGetScrnSize().cy;
		}

		//Get the color
		if(CfgGetItemStr(cfg, CURSOR2D_SECTION, "color", buff))
		{
			int r,g,b,a;
			sscanf(buff, "%d,%d,%d,%d", &r,&g,&b,&a);
			m_clr = D3DCOLOR_RGBA(r,g,b,a);
		}
	}
}

MenuCursor2D::~MenuCursor2D()
{
	if(m_cursor)
		TextureDestroy(&m_cursor);
}

void MenuCursor2D::Display()
{
	if(m_cursor)
	{
		GFXBltModeEnable(0);
		TextureStretchBlt(m_cursor, m_cursorCurLoc.x, m_cursorCurLoc.y,
			m_size.x, m_size.y, 0, &m_clr, 0);
		GFXBltModeDisable();
	}
}

void MenuCursor2D::UpdateSub()
{
}