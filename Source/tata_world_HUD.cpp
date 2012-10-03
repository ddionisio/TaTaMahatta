#include "tata_main.h"
#include "tata_world.h"

#include "tata_creature_tata.h"

//HUD stuff...
#define FADE_DELAY 3000

#define MAX_HUD_ALPHA	255

HUD::HUD() : m_blankTxt(0), m_selectTxt(0), m_ouchTxt(0), m_healthTxt(0), m_goalTxt(0),
m_fnt(0), m_goalSX(0), m_goalSY(0), m_goalEX(0), m_goalEY(0),
m_goalMinFixed(0), m_goalMin(0), m_goalMax(0),
m_startX(0), m_startY(0), m_endX(0), m_endY(0), m_space(0), m_alphaCur(0),
m_alphaNext(0), m_fntRelSize(0)
{
	m_selectOBJ.counter = -1;
	TimeInit(&m_delay, FADE_DELAY);
}

HUD::~HUD()
{
	if(m_blankTxt)
		TextureDestroy(&m_blankTxt);

	if(m_selectTxt)
		TextureDestroy(&m_selectTxt);

	if(m_ouchTxt)
		TextureDestroy(&m_ouchTxt);

	if(m_healthTxt)
		TextureDestroy(&m_healthTxt);

	hOBJ selectOBJ = OBJQuery(&m_selectOBJ);
	if(selectOBJ)
		OBJDestroy(&selectOBJ);

	HUDSetGoalTxt(0);

	if(m_fnt)
		FontDestroy(&m_fnt);
}

RETCODE HUD::HUDLoad(hCFG cfg)
{
	if(cfg)
	{
		string mainFldr=GAMEFOLDER, filePath;
		char buff[MAXCHARBUFF];

		//load blank texture
		if(CfgGetItemStr(cfg, "HUD", "blank", buff))
		{
			filePath = mainFldr;
			filePath += "\\";
			filePath += buff;
			m_blankTxt = TextureCreate(0, filePath.c_str(), false, 0);
		}

		//load select texture
		if(CfgGetItemStr(cfg, "HUD", "select", buff))
		{
			filePath = mainFldr;
			filePath += "\\";
			filePath += buff;
			m_selectTxt = TextureCreate(0, filePath.c_str(), false, 0);
		}

		//load ouch texture
		if(CfgGetItemStr(cfg, "HUD", "ouch", buff))
		{
			filePath = mainFldr;
			filePath += "\\";
			filePath += buff;
			m_ouchTxt = TextureCreate(0, filePath.c_str(), false, 0);
		}

		//load health texture
		if(CfgGetItemStr(cfg, "HUD", "health", buff))
		{
			filePath = mainFldr;
			filePath += "\\";
			filePath += buff;
			m_healthTxt = TextureCreate(0, filePath.c_str(), false, 0);
		}

		//load font
		m_fntRelSize = CfgGetItemFloat(cfg, "HUD", "fntSize");

		if(CfgGetItemStr(cfg, "HUD", "fntType", buff))
			m_fnt = FontCreate(buff, (unsigned int)(m_fntRelSize*GFXGetScrnSize().cx), 0);

		//load the location and stuff...
		m_goalSX = CfgGetItemFloat(cfg, "HUD", "goalSX");
		m_goalSY = CfgGetItemFloat(cfg, "HUD", "goalSY");
		m_goalEX = CfgGetItemFloat(cfg, "HUD", "goalEX");
		m_goalEY = CfgGetItemFloat(cfg, "HUD", "goalEY");

		m_goalFNTX = CfgGetItemFloat(cfg, "HUD", "goalFNTX");
		m_goalFNTY = CfgGetItemFloat(cfg, "HUD", "goalFNTY");

		m_startX = CfgGetItemFloat(cfg, "HUD", "startX");
		m_startY = CfgGetItemFloat(cfg, "HUD", "startY");
		m_endX = CfgGetItemFloat(cfg, "HUD", "endX");
		m_endY = CfgGetItemFloat(cfg, "HUD", "endY");

		m_space = CfgGetItemFloat(cfg, "HUD", "space");

		//load up select stuff

		/////////////////////////////////////////////////////
		//load the model
		hMDL selectMDL=0;

		if(CfgGetItemStr(cfg, "HUD", "selectMDL", buff))
		{
			filePath = mainFldr;
			filePath += "\\";
			filePath += buff;
			selectMDL = MDLCreate(0, filePath.c_str());
		}

		if(selectMDL)
		{
			char mdlANMPath[MAXCHARBUFF];

			strcpy(mdlANMPath, filePath.c_str());
			strcpy(GetExtension(mdlANMPath), ".anm");

			hOBJ selectOBJ = OBJCreate(0, selectMDL, 0,0,0, eOBJ_ANIMATE, mdlANMPath);

			if(selectOBJ)
			{
				m_selectOBJ = OBJGetKey(selectOBJ);

				//not gonna show it yet
				OBJActivate(selectOBJ, false);
			}
		}
		/////////////////////////////////////////////////////

		//load the select delay and initialize the select delay
		TimeInit(&m_selectDelay, CfgGetItemFloat(cfg, "HUD", "selectDelay"));

		//load the select rotation speed
		m_selectRotSpd.MoveSetSpeed(CfgGetItemFloat(cfg, "HUD", "selectRotSpd"));
	}

	return RETCODE_SUCCESS;
}

void HUD::HUDResize()
{
	FontResize(m_fnt, (unsigned int)(m_fntRelSize*GFXGetScrnSize().cx));
}

RETCODE HUD::HUDUpdate()
{
	double t;

	//check to see if we need to fade
	if(m_alphaCur != m_alphaNext)
	{
		t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

		if(t >= 1)
		{
			TimeReset(&m_delay);

			t = 1;
		}

		m_alphaCur = m_alphaCur + t*(m_alphaNext-m_alphaCur);
	}

	//update selector
	hOBJ selectOBJ = OBJQuery(&m_selectOBJ);

	if(selectOBJ && !OBJIsDeactivate(selectOBJ))
	{
		t = TimeGetTime(&m_selectDelay)/TimeGetDelay(&m_selectDelay);

		if(t >= 1)
			t = 1;

		OBJSetAlpha(selectOBJ, 1.0-t);

		//rotate
		OBJRotY(selectOBJ, m_selectRotSpd.MoveUpdate(g_timeElapse));

		if(t == 1)
		{
			OBJRemoveFromParent(selectOBJ);
			OBJActivate(selectOBJ, false);
		}
	}

	return RETCODE_SUCCESS;
}

RETCODE HUD::HUDDisplay(const Id *pIDs, int numTaTa, int curTaTa)
{
	DWORD clr = D3DCOLOR_RGBA(255,255,255,(BYTE)(m_alphaCur/3));
	DWORD clrSel = D3DCOLOR_RGBA(255,255,255,(BYTE)m_alphaCur);
	TaTaCommon *pTata;

	float scrnW = GFXGetScrnSize().cx;
	float scrnH = GFXGetScrnSize().cy;

	float sX=m_startX*scrnW, eX=m_endX*scrnW;
	float sY=m_startY*scrnH, eY=m_endY*scrnH;

	float ofX = ((m_endX-m_startX)+m_space)*scrnW;

	float portW = eX - sX, portH = eY - sY;

	//display the portraits and stuff...
	for(int i = 0; i < numTaTa; i++)
	{
		pTata = (TaTaCommon *)IDPageQuery(pIDs[i]);

		if(pTata)
		{
			//if this Ta-Ta is selected, display select
			if(i == curTaTa)
			{
				//first, display the portrait
				TextureStretchBlt(pTata->GetPortrait(), 
					sX, sY, portW, portH,
					0, &clrSel, 0);

				TextureStretchBlt(m_selectTxt, 
					sX, sY, portW, portH,
					0, &clrSel, 0);
			}
			else
				//first, display the portrait
				TextureStretchBlt(pTata->GetPortrait(), 
					sX, sY, portW, portH,
					0, &clr, 0);

			//display health points
			//1/4 the size of portrait
			int maxPt = pTata->GetCurHit();

			float hW = portW/4;
			float hH = portH/4;

			float hSX = sX, hEX = hSX+hW, 
				hSY = sY+(eY-sY);

			for(int j = 0; j < maxPt; j++)
			{
				//display health pt.
				TextureStretchBlt(m_healthTxt, 
					hSX,hSY,hW,hH,
					0, &clrSel, 0);

				//move the position for next health
				hSX += hW; hEX += hW;
			}

			//display ouch if hurt CREANIM_OUCH
			if(OBJGetCurState(pTata->GetOBJ()) == CREANIM_OUCH)
				TextureStretchBlt(m_ouchTxt, sX, sY, portW, portH, 0, &clrSel, 0);

			//display if Ta-Ta is at home (goal)
			if(m_goalTxt && pTata->CheckFlag(ENTITY_FLAG_GOAL))
			{
				float goalW = portW*0.5f, goalH = portH*0.5f;
				TextureStretchBlt(m_goalTxt, sX, sY+goalH, goalW, goalH, 0, &clrSel, 0);
			}
		}
		//no Ta-Ta in this slot, display blank
		else
			TextureStretchBlt(m_blankTxt, sX, sY, portW, portH, 0, &clr, 0);

		//move the position for next portrait
		sX += ofX; eX += ofX;
	}

	//display goal img
	if(m_goalTxt)
	{
		sX=m_goalSX*scrnW; sY=m_goalSY*scrnH;
		eX=m_goalEX*scrnW; eY=m_goalEY*scrnH;

		portW = eX - sX; portH = eY - sY;

		TextureStretchBlt(m_goalTxt, sX, sY, portW, portH, 0, &clrSel, 0);

		FontPrintf2D(m_fnt, m_goalFNTX*scrnW, 
			m_goalFNTY*scrnH, clrSel, "%d/%d", m_goalMinFixed+m_goalMin, m_goalMax);
	}

	return RETCODE_SUCCESS;
}

void HUD::HUDActive(bool bYes)
{
	if(bYes)
		m_alphaNext = MAX_HUD_ALPHA;
	else
		m_alphaNext = 0;

	TimeReset(&m_delay);
}

void HUD::HUDSetGoalTxt(hTXT txt)
{
	if(m_goalTxt)
		TextureDestroy(&m_goalTxt);

	m_goalTxt = txt;
}

void HUD::HUDSetGoalMinFixed(int n)
{
	m_goalMinFixed = n;
}

void HUD::HUDSetGoalMin(int n)
{
	m_goalMin = n;
}

void HUD::HUDSetGoalMax(int n)
{
	m_goalMax = n;
}

int HUD::HUDGetGoalMinFixed()
{
	return m_goalMinFixed;
}

int HUD::HUDGetGoalMin()
{
	return m_goalMin;
}

int HUD::HUDGetGoalMax()
{
	return m_goalMax;
}

bool HUD::HUDIsGoalComplete()
{
	return (m_goalMax > 0 && ((m_goalMinFixed+m_goalMin) >= m_goalMax));
}

void HUD::HUDSetSelect(const Id & EntityID)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(EntityID);

	hOBJ selectOBJ = OBJQuery(&m_selectOBJ);

	if(pEntity && selectOBJ)
	{
		hOBJ obj = pEntity->GetOBJ();

		if(obj)
		{
			//activate the selector
			OBJActivate(selectOBJ, true);

			//set alpha
			OBJSetAlpha(selectOBJ, 1.0f);

			//initialize timer
			TimeReset(&m_selectDelay);

			//put the selector at the top of the object
			gfxBound boundOBJ;
			OBJGetBounds(obj, &boundOBJ, true);

			gfxBound boundSelect;
			OBJGetBounds(selectOBJ, &boundSelect, true);

			float loc[eMaxPt] = {0, boundOBJ.max[eY] + (boundSelect.max[eY]*2), 0};
			OBJTranslate(selectOBJ, loc, false);

			//set selector as child of object
			//if the object has the 'body' joint, then use that joint
			OBJAddChild(obj, selectOBJ, 
				OBJJointGetIndex(obj, "body"), false);

			OBJSetToLast(obj);
		}
	}
}

void HUD::HUDRemoveSelect()
{
	hOBJ selectOBJ = OBJQuery(&m_selectOBJ);

	if(selectOBJ)
	{
		OBJRemoveFromParent(selectOBJ);
		OBJActivate(selectOBJ, false);
	}
}