#include "tata_main.h"

#include "tata_world.h"

#include "tata_script.h"

#include "tata_globals.h"

//Image (string filename, float sx, float sy, float ex, float ey, int r, int g, int b, int a)
//create an image display in the world, defaults to disable
PROTECTED long S_ImageCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	Id *pID = (Id *)ScriptVarGetData(var);

	switch(msg)
	{
	case VAR_CREATE:
		pID = new Id; assert(pID);

		pID->ID = 0;
		pID->counter = -1;

		ScriptVarSetData(var, pID);
		break;

	case VAR_SET:
		{
			//remove old image if it exists
			if(g_world)
			{
				GameImage *pImg = (GameImage *)IDPageQuery(*pID);
				
				if(pImg)
					delete pImg;

				pID->counter = -1;

				hPARAM param = (hPARAM)wparam;

				char *fileStr;
				float sx,sy,ex,ey;
				int r,g,b,a;
					
				if(ScriptParamGetType(param, 0) == eVarPtr)
					fileStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
				else
					fileStr = (char *)ScriptParamGetVar(param, 0);

				sx = *(float*)ScriptParamGetVar(param, 1);
				sy = *(float*)ScriptParamGetVar(param, 2);
				ex = *(float*)ScriptParamGetVar(param, 3);
				ey = *(float*)ScriptParamGetVar(param, 4);

				r = *(int*)ScriptParamGetVar(param, 5);
				g = *(int*)ScriptParamGetVar(param, 6);
				b = *(int*)ScriptParamGetVar(param, 7);
				a = *(int*)ScriptParamGetVar(param, 8);

				if(fileStr[0] != 0)
				{
					string imgPath = GAMEFOLDER;
					imgPath += "\\";
					imgPath += fileStr;

					hTXT txt = TextureCreate(0, imgPath.c_str(), false, 0);

					//create new image display
					*pID = g_world->ImageDisplayAdd(txt, sx, sy, ex, ey, r, g, b, a);

					TextureDestroy(&txt);
				}
			}
		}
		break;

	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			pOut->paramVarType = eVarInt;

			pOut->dat._int = GAME_IDVALID(*pID);
		}
		break;

	case VAR_DESTROY:
		{
			GameImage *pImg = (GameImage*)IDPageQuery(*pID);

			if(pImg)
				delete pImg;

			delete pID;
		}
		break;
	}

	return RETCODE_SUCCESS;
}

//ImageSetStatus (image img, int state)
//check to see if an entity by the given name exists, iOut = 1 if found,
//0 otherwise.
PROTECTED long S_ImageSetStatus(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	int *pIDat = (int*)ScriptFuncGetData(func);

	switch(msg)
	{
	case FUNC_CREATE:
		pIDat = new int;
		*pIDat = 0;
		ScriptFuncSetData(func, pIDat);
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			Id *pID = (Id*)ScriptParamGetVar(param, 0);

			int state = *(int*)ScriptParamGetVar(param, 1);

			GameImage *pImg = (GameImage*)IDPageQuery(*pID);

			if(!pImg)
				return FUNCRET_DONE;

			if(*pIDat == 0)
			{
				pImg->SetState(state);
				*pIDat = 1;
			}

			if(pImg->GetState() == GameImage::STATE_NORMAL
				|| pImg->GetState() == GameImage::STATE_DISABLE)
			{
				*pIDat = 0;
				return FUNCRET_DONE;
			}
		}
		return FUNCRET_NOTDONE;

	case FUNC_DESTROY:
		if(pIDat)
			delete pIDat;
		break;
	}

	return RETCODE_SUCCESS;
}