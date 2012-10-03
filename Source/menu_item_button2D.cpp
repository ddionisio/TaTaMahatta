#include "tata_main.h"

#include "tata_menu_item_common.h"

#include "tata_globals.h"

/*
hTXT			m_btnImg;		//the image representation
	D3DXVECTOR2		m_startLoc;		//the starting loc. once loaded
	D3DXVECTOR2		m_endLoc;		//the ending loc.
	D3DXVECTOR2		m_curLoc;		//current location
	D3DXVECTOR2		m_size;			//size
	DWORD			m_clr;			//
	float			m_startAlpha;	//the starting alpha [0,255]
	float			m_endAlpha;		//the ending alpha [0,255]
	win32Time		m_delay;		//delay for entering and exiting*/
	
MTButton2D::MTButton2D(const Id & parentID, hCFG cfg, const char *section)
: MenuItem(parentID, MITM_TYPE_BUTTON2D, cfg, section), m_btnImg(0),
m_startLoc(0,0), m_endLoc(0,0), m_curLoc(0,0), m_size(0,0), m_clr(0xffffffff),
m_startAlpha(0), m_endAlpha(0)
{
	SIZE scrnSize = GFXGetScrnSize();

	//set the cursor offset to pixel
	m_cursorOffset.x *= scrnSize.cx;
	m_cursorOffset.y *= scrnSize.cy;

	char buff[MAXCHARBUFF];

	/////////////////////////////////////////////////////////////////
	//Load image
	if(CfgGetItemStr(cfg, section, "image", buff))
	{
		string imgPath = GAMEFOLDER;
		imgPath += "\\";
		imgPath += buff;

		m_btnImg = TextureCreate(0, imgPath.c_str(), false, 0);
	}

	/////////////////////////////////////////////////////////////////
	//Load other components
	if(CfgGetItemStr(cfg, section, "sLoc", buff))
	{
		sscanf(buff, "%f,%f", &m_startLoc.x, &m_startLoc.y);

		m_startLoc.x *= scrnSize.cx;
		m_startLoc.y *= scrnSize.cy;

		m_curLoc=m_startLoc;
	}

	if(CfgGetItemStr(cfg, section, "eLoc", buff))
	{
		sscanf(buff, "%f,%f", &m_endLoc.x, &m_endLoc.y);

		m_endLoc.x *= scrnSize.cx;
		m_endLoc.y *= scrnSize.cy;
	}

	if(CfgGetItemStr(cfg, section, "size", buff))
	{
		sscanf(buff, "%f,%f", &m_size.x, &m_size.y);

		m_size.x *= scrnSize.cx;
		m_size.y *= scrnSize.cy;
	}

	m_startAlpha = CfgGetItemFloat(cfg, section, "sAlpha");
	m_endAlpha = CfgGetItemFloat(cfg, section, "eAlpha");

	TimeInit(&m_delay, CfgGetItemFloat(cfg, section, "delay"));
}

MTButton2D::~MTButton2D()
{
	if(m_btnImg)
		TextureDestroy(&m_btnImg);
}

int MTButton2D::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case MENU_ITM_MSG_UPDATE:
		{
			bool bDone = false;

			switch(wParam)
			{
			case MITM_UPDATE_NORMAL:
				break;

			case MITM_UPDATE_ENTERING:
				{
					float alpha;
					double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

					if(t >= 1)
					{
						t = 1;

						bDone = true;
					}

					D3DXVec2Lerp(&m_curLoc, &m_startLoc, &m_endLoc, t);
					
					alpha = m_startAlpha + t*(m_endAlpha - m_startAlpha);

					m_clr = D3DCOLOR_RGBA(255,255,255,(BYTE)(alpha));
				}
				break;

			case MITM_UPDATE_EXITING:
				{
					float alpha;
					double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

					if(t >= 1)
					{
						t = 1;

						bDone = true;
					}

					D3DXVec2Lerp(&m_curLoc, &m_endLoc, &m_startLoc, t);
					
					alpha = m_endAlpha + t*(m_startAlpha - m_endAlpha);

					m_clr = D3DCOLOR_RGBA(255,255,255,(BYTE)(alpha));
				}
				break;
			}

			if(bDone)
				return RETCODE_BREAK;
		}
		break;

	case MENU_ITM_MSG_DRAW:
		if(m_btnImg)
		{
			GFXBltModeEnable(0);

			TextureStretchBlt(m_btnImg, m_curLoc.x, m_curLoc.y, 
				m_size.x*m_scale.x, m_size.y*m_scale.y, 0, &m_clr, 0);

			GFXBltModeDisable();
		}
		break;

	case MENU_ITM_MSG_SELECT:
		break;

	case MENU_ITM_MSG_ACTIVATE:
		break;

	case MENU_ITM_MSG_STATECHANGE:
		TimeReset(&m_delay);
		break;
	}

	return RETCODE_SUCCESS;
}

D3DXVECTOR3 MTButton2D::GetCursorOffset()
{
	//this is in terms of pixel
	
	return D3DXVECTOR3(m_endLoc.x+m_cursorOffset.x, m_endLoc.y+m_cursorOffset.y, 0);
}