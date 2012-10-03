#include "tata_main.h"

#include "tata_menu_item_common.h"

#include "tata_globals.h"

//animation
typedef enum {
	BUTTON3D_ANIM_UNSELECT,
	BUTTON3D_ANIM_SELECT
} eButton3DAnim;

MTButton3D::MTButton3D(const Id & parentID, hCFG cfg, const char *section)
: MenuItem(parentID, MITM_TYPE_BUTTON3D, cfg, section), m_btnOBJ(0),
m_startLoc(0,0,0), m_endLoc(0,0,0), m_startAlpha(0), m_endAlpha(0)
{
	char buff[MAXCHARBUFF]={0};

	/////////////////////////////////////////////////////////////////
	//load the button model
	hMDL mdl=0;

	if(CfgGetItemStr(cfg, section, "btnMDL", buff))
	{
		string mdlPath = GAMEFOLDER;
		mdlPath += "\\";
		mdlPath += buff;

		mdl = MDLCreate(0, mdlPath.c_str());
	}

	//load animation of cursor and the model
	if(CfgGetItemStr(cfg, section, "btnAnim", buff) && mdl)
	{
		string mdlAnimPath = GAMEFOLDER;
		mdlAnimPath += "\\";
		mdlAnimPath += buff;

		//create the object
		m_btnOBJ = OBJCreate(0, mdl, 0,0,0, eOBJ_ANIMATE, mdlAnimPath.c_str());
	}
	else
		m_btnOBJ = OBJCreate(0, mdl, 0,0,0, eOBJ_STILL, 0);

	//we don't want it to be shown with the world
	//we will update and display the object manually
	if(m_btnOBJ)
		OBJActivate(m_btnOBJ, false);

	/////////////////////////////////////////////////////////////////
	//Load other components
	if(CfgGetItemStr(cfg, section, "sLoc", buff))
		sscanf(buff, "%f,%f,%f", &m_startLoc.x, &m_startLoc.y, &m_startLoc.z);

	if(CfgGetItemStr(cfg, section, "eLoc", buff))
		sscanf(buff, "%f,%f,%f", &m_endLoc.x, &m_endLoc.y, &m_endLoc.z);

	m_startAlpha = CfgGetItemFloat(cfg, section, "sAlpha");
	m_endAlpha = CfgGetItemFloat(cfg, section, "eAlpha");

	TimeInit(&m_delay, CfgGetItemFloat(cfg, section, "delay"));
}

MTButton3D::~MTButton3D()
{
	if(m_btnOBJ)
		OBJDestroy(&m_btnOBJ);
}

int MTButton3D::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case MENU_ITM_MSG_UPDATE:
		{
			bool bDone = false;
			D3DXVECTOR3 curLoc; OBJGetLoc(m_btnOBJ, (float*)curLoc);

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

					D3DXVec3Lerp(&curLoc, &m_startLoc, &m_endLoc, t);
					
					alpha = m_startAlpha + t*(m_endAlpha - m_startAlpha);

					OBJTranslate(m_btnOBJ, (float*)curLoc, false);
					OBJSetAlpha(m_btnOBJ, alpha);
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

					D3DXVec3Lerp(&curLoc, &m_endLoc, &m_startLoc, t);
					
					alpha = m_endAlpha + t*(m_startAlpha - m_endAlpha);

					OBJTranslate(m_btnOBJ, (float*)curLoc, false);
					OBJSetAlpha(m_btnOBJ, alpha);
				}
				break;
			}

			OBJUpdateFrame(m_btnOBJ);

			if(bDone)
				return RETCODE_BREAK;
		}
		break;

	case MENU_ITM_MSG_DRAW:
		OBJDisplay(m_btnOBJ);
		break;

	case MENU_ITM_MSG_SELECT:
		if(wParam)
		{
			OBJSetState(m_btnOBJ, BUTTON3D_ANIM_SELECT);
		}
		else
		{
			OBJSetState(m_btnOBJ, BUTTON3D_ANIM_UNSELECT);
		}
		break;

	case MENU_ITM_MSG_ACTIVATE:
		break;

	case MENU_ITM_MSG_STATECHANGE:
		TimeReset(&m_delay);
		break;
	}

	return RETCODE_SUCCESS;
}

D3DXVECTOR3 MTButton3D::GetCursorOffset() 
{
	//cursor offset is in terms of model space
	//set to world space
	return m_endLoc+m_cursorOffset; 
}