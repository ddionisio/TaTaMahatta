#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_stage_select.h"

#include "tata_globals.h"

//FIXME: there should only be one instance of Level Select
//		 make sure that we cannot create this menu more than once

#define LEVEL_START_SECTION	"level_start"

hFNT	g_titleFnt=0;
DWORD	g_titleClr=0xffffffff;

hFNT	g_descFnt=0;
DWORD	g_descClr=0xffffffff;

hTXT	g_bkImg=0;

/////////////////////////////
//these are in terms of pixel

float	g_bkOfs;
float	g_titleSY;

D3DXVECTOR2 g_descLoc;
D3DXVECTOR2 g_descSize;


//Level Start
RETCODE MCB_LevelStart(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		{
			SIZE scrnSize = GFXGetScrnSize();

			hCFG cfg = (hCFG)wParam;

			if(cfg)
			{
				char buff[MAXCHARBUFF]={0};
				int r,g,b,a;
				float fntSize;

				///////////////////////////////////////
				// Get Font Title
				fntSize = CfgGetItemFloat(cfg, LEVEL_START_SECTION, "titleFntSize");
				fntSize *= scrnSize.cx;

				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "titleFnt", buff))
					g_titleFnt = FontCreate(buff, fntSize, 0);

				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "titleFntClr", buff))
				{
					sscanf(buff, "%d,%d,%d,%d", &r,&g,&b,&a);
					g_titleClr = D3DCOLOR_RGBA(r,g,b,a);
				}

				///////////////////////////////////////
				// Get Font Desc.
				fntSize = CfgGetItemFloat(cfg, LEVEL_START_SECTION, "descFntSize");
				fntSize *= scrnSize.cx;

				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "descFnt", buff))
					g_descFnt = FontCreate(buff, fntSize, 0);

				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "descFntClr", buff))
				{
					sscanf(buff, "%d,%d,%d,%d", &r,&g,&b,&a);
					g_descClr = D3DCOLOR_RGBA(r,g,b,a);
				}

				///////////////////////////////////////
				// Get Background img
				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "bkImg", buff))
				{
					string imgPath = GAMEFOLDER;
					imgPath += "\\";
					imgPath += buff;

					g_bkImg = TextureCreate(0, imgPath.c_str(), false, 0);
				}

				///////////////////////////////////////
				// Load other components
				g_bkOfs = CfgGetItemFloat(cfg, LEVEL_START_SECTION, "bkOfs");
				g_bkOfs *= scrnSize.cx;

				g_titleSY = CfgGetItemFloat(cfg, LEVEL_START_SECTION, "titleY");
				g_titleSY *= scrnSize.cy;

				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "descSLoc", buff))
				{
					sscanf(buff, "%f,%f", &g_descLoc.x, &g_descLoc.y);

					g_descLoc.x *= scrnSize.cx;
					g_descLoc.y *= scrnSize.cy;
				}

				if(CfgGetItemStr(cfg, LEVEL_START_SECTION, "descELoc", buff))
				{
					D3DXVECTOR2 eLoc;
					sscanf(buff, "%f,%f", &eLoc.x, &eLoc.y);

					g_descSize.x = (eLoc.x*scrnSize.cx) - g_descLoc.x;
					g_descSize.y = (eLoc.y*scrnSize.cy) - g_descLoc.y;
				}
			}

			g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);
		}
		break;

	case MENU_MSG_BTN:
		if(lParam == INP_STATE_RELEASED)
		{
			switch(wParam)
			{
			case INP_A:
			case INP_B:
			case INP_C:
			case INP_D:
			case INP_START:
				g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);
				MenuExitCurrent();
				g_world->CutsceneRemove();
				InputClear();
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);
		MenuExitCurrent();
		g_world->CutsceneRemove();
		InputClear();
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			if(INPXKbIsReleased(DIK_ESCAPE) || INPXKbIsReleased(DIK_RETURN))
			{
				g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);
				MenuExitCurrent();
				g_world->CutsceneRemove();
			}
			break;

		case MENU_UPDATE_ENTERING:
		case MENU_UPDATE_EXITING:
			return RETCODE_BREAK;
		}
		break;

	case MENU_MSG_DRAW:
		if(g_world)
		{
			SIZE scrnSize = GFXGetScrnSize();

			SIZE fntSize;

			float fntX, fntY;
			float bkX, bkY, bkW, bkH;

			///////////////////////////////////////////
			//Display Title
			const char *titleStr = g_world->GetName();
			FontGetStrSize(g_titleFnt, (char*)titleStr, &fntSize);

			fntX = (scrnSize.cx/2)-(fntSize.cx/2);
			fntY = g_titleSY;

			bkX = fntX - g_bkOfs;
			bkY = fntY - g_bkOfs;

			bkW = fntSize.cx+(g_bkOfs*2);
			bkH = fntSize.cy+(g_bkOfs*2);

			GFXBltModeEnable(0);
			TextureStretchBlt(g_bkImg, bkX,bkY,bkW,bkH, 0, 0, 0);
			GFXBltModeDisable();

			FontPrintf2D(g_titleFnt, fntX, fntY, g_titleClr, titleStr);

			///////////////////////////////////////////
			//Display Description
			bkX = g_descLoc.x - g_bkOfs;
			bkY = g_descLoc.y - g_bkOfs;

			bkW = g_descSize.x+(g_bkOfs*2);
			bkH = g_descSize.y+(g_bkOfs*2);

			GFXBltModeEnable(0);
			TextureStretchBlt(g_bkImg, bkX,bkY,bkW,bkH, 0, 0, 0);
			GFXBltModeDisable();

			FontTextBox(g_descFnt, g_descLoc.x,g_descLoc.y, 
				g_descLoc.x+g_descSize.x, g_descLoc.y+g_descSize.y, g_descClr, g_world->MapGetDesc());

			///////////////////////////////////////////
			//Press Any Button To Start
			FontGetStrSize(g_descFnt, "Press Any Button To Start", &fntSize);

			fntX = (scrnSize.cx/2)-(fntSize.cx/2);
			fntY = scrnSize.cy - fntSize.cy;

			FontPrintf2D(g_descFnt, fntX, fntY, 0xffffffff, "Press Any Button To Start");
		}
		break;

	case MENU_MSG_DESTROY:
		if(g_titleFnt)
		{ FontDestroy(&g_titleFnt); g_titleFnt = 0; }

		if(g_descFnt)
		{ FontDestroy(&g_descFnt); g_descFnt = 0; }

		if(g_bkImg)
		{ TextureDestroy(&g_bkImg); g_bkImg = 0; }
		break;
	}

	return RETCODE_SUCCESS;
}