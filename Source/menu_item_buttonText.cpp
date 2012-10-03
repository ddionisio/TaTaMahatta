#include "tata_main.h"

#include "tata_menu.h"
#include "tata_menu_item_common.h"

#include "tata_globals.h"

MTButtonText::MTButtonText(const Id & parentID, hCFG cfg, const char *section)
: MenuItem(parentID, MITM_TYPE_BUTTONTEXT, cfg, section), m_fntInd(0),
m_startLoc(0,0), m_endLoc(0,0), m_curLoc(0,0), m_clr(0xffffffff), m_r(255),
m_g(255), m_b(255), m_startAlpha(0), m_endAlpha(0), m_bCenter(false)
{
	SIZE scrnSize = GFXGetScrnSize();

	//set the cursor offset to pixel
	m_cursorOffset.x *= scrnSize.cx;
	m_cursorOffset.y *= scrnSize.cy;

	char buff[MAXCHARBUFF];

	/////////////////////////////////////////////////////////////////
	//Load font index and string
	m_fntInd = CfgGetItemInt(cfg, section, "fontInd");

	if(CfgGetItemStr(cfg, section, "text", buff))
		m_str = buff;

	/////////////////////////////////////////////////////////////////
	//Load other components
	if(CfgGetItemStr(cfg, section, "sLoc", buff))
	{
		sscanf(buff, "%f,%f", &m_startLoc.x, &m_startLoc.y);

		m_startLoc.x *= scrnSize.cx;
		m_startLoc.y *= scrnSize.cy;
	}

	if(CfgGetItemStr(cfg, section, "eLoc", buff))
	{
		sscanf(buff, "%f,%f", &m_endLoc.x, &m_endLoc.y);

		m_endLoc.x *= scrnSize.cx;
		m_endLoc.y *= scrnSize.cy;
	}

	if(CfgGetItemStr(cfg, section, "color", buff))
	{
		int r,g,b;
		sscanf(buff, "%d,%d,%d", &r, &g, &b);
		m_r = r; m_g = g; m_b = b;
	}

	m_startAlpha = CfgGetItemFloat(cfg, section, "sAlpha");
	m_endAlpha = CfgGetItemFloat(cfg, section, "eAlpha");

	m_clr = D3DCOLOR_RGBA(m_r,m_g,m_b,(BYTE)(m_startAlpha));

	TimeInit(&m_delay, CfgGetItemFloat(cfg, section, "delay"));

	int center = CfgGetItemInt(cfg, section, "bCenter");

	m_bCenter = center == 1 ? true : false;
}

MTButtonText::~MTButtonText()
{
}

int MTButtonText::Callback(unsigned int msg, unsigned int wParam, int lParam)
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

					m_clr = D3DCOLOR_RGBA(m_r,m_g,m_b,(BYTE)(alpha));
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

					m_clr = D3DCOLOR_RGBA(m_r,m_g,m_b,(BYTE)(alpha));
				}
				break;
			}

			if(bDone)
				return RETCODE_BREAK;
		}
		break;

	case MENU_ITM_MSG_DRAW:
		{
			hMENU parent = (hMENU)IDPageQuery(m_parentID);

			if(parent)
			{
				D3DXVECTOR3 loc(m_curLoc);

				if(m_bCenter && m_str.c_str() != 0)
				{
					SIZE fntSize;
					FontGetStrSize(parent->GetFont(m_fntInd), (char*)m_str.c_str(), &fntSize);

					loc.x -= fntSize.cx/2;
				}

				FontPrintf2D(parent->GetFont(m_fntInd), 
				loc.x, loc.y, m_clr, m_str.c_str());
			}
		}
		break;

	case MENU_ITM_MSG_SELECT:
		break;

	case MENU_ITM_MSG_ACTIVATE:
		break;

	case MENU_ITM_MSG_STATECHANGE:
		TimeReset(&m_delay);
		break;

	case MENU_ITM_MSG_SETTEXT: //set the text, wParam = (char*)
		m_str = (char*)wParam;
		break;

	case MENU_ITM_MSG_GETTEXT:	//get the text, return (const char*)
		return (int)m_str.c_str();
	}

	return RETCODE_SUCCESS;
}

D3DXVECTOR3 MTButtonText::GetCursorOffset()
{
	//this is in terms of pixel
	
	return D3DXVECTOR3(m_endLoc.x+m_cursorOffset.x, m_endLoc.y+m_cursorOffset.y, 0);
}