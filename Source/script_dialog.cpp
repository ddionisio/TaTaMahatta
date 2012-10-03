#include "tata_main.h"

#include "tata_world.h"

#include "tata_script.h"

#include "tata_globals.h"

//Dialog (string imgfile, float sx, float sy, float ex, float ey)
//a dialog, must set font before use
//make sure to open it to display
PROTECTED long S_DialogCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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
			GameDialog *pDlg = (GameDialog*)IDPageQuery(*pID);

			if(pDlg)
				delete pDlg;

			pID->counter = -1;

			if(g_world)
			{
				hPARAM param = (hPARAM)wparam;

				char *imgFilePath;
				float sx,sy,ex,ey;

				if(ScriptParamGetType(param, 0) == eVarPtr)
					imgFilePath = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
				else
					imgFilePath = (char *)ScriptParamGetVar(param, 0);

				sx = *(float*)ScriptParamGetVar(param, 1);
				sy = *(float*)ScriptParamGetVar(param, 2);
				ex = *(float*)ScriptParamGetVar(param, 3);
				ey = *(float*)ScriptParamGetVar(param, 4);

				if(imgFilePath[0] != 0)
				{
					string imgPath = GAMEFOLDER;
					imgPath += "\\";
					imgPath += imgFilePath;

					hTXT txt = TextureCreate(0, imgPath.c_str(), false, 0);

					*pID = g_world->DialogAdd(txt, sx,sy,ex,ey);
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
			GameDialog *pDlg = (GameDialog*)IDPageQuery(*pID);

			if(pDlg)
				delete pDlg;

			delete pID;
		}
		break;
	}

	return RETCODE_SUCCESS;
}

//DialogSetFont (dialog dlg, string typeName, int size, int fmtFlag)
//set the dialog font
PROTECTED long S_DialogSetFont(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			GameDialog *pDlg;
			char *typeStr;
			float size;
			int fmtFlag;

			pDlg = (GameDialog *)IDPageQuery(*(Id*)ScriptParamGetVar(param, 0));
				
			if(ScriptParamGetType(param, 1) == eVarPtr)
				typeStr = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				typeStr = (char *)ScriptParamGetVar(param, 1);

			size = *(float *)ScriptParamGetVar(param, 2);

			fmtFlag = *(int *)ScriptParamGetVar(param, 3);

			if(pDlg)
				pDlg->SetFont(typeStr, size, fmtFlag);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//DialogOpen (dialog dlg, int bOpen)
//open/close dialog (bOpen == 0 close, 1 open)
PROTECTED long S_DialogOpen(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			GameDialog *pDlg;
			int bOpen;

			pDlg = (GameDialog *)IDPageQuery(*(Id*)ScriptParamGetVar(param, 0));

			bOpen = *(int *)ScriptParamGetVar(param, 1);

			if(pDlg)
			{
				if(bOpen)
					pDlg->Open();
				else
					pDlg->Close();
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//DialogSetText (dialog dlg, string text)
//set dialog text
PROTECTED long S_DialogSetText(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	int *pIDat = (int *)ScriptFuncGetData(func);

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

			GameDialog *pDlg;
			char *str;

			pDlg = (GameDialog *)IDPageQuery(*(Id*)ScriptParamGetVar(param, 0));

			if(*pIDat == 0)
			{
				if(ScriptParamGetType(param, 1) == eVarPtr)
					str = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
				else
					str = (char *)ScriptParamGetVar(param, 1);

				if(pDlg)
				{
					pDlg->SetString(str);
					*pIDat = 1;
				}
			}
			else if(pDlg->IsTextComplete())
				return FUNCRET_DONE;
		}
		return FUNCRET_NOTDONE;

	case FUNC_DESTROY:
		if(pIDat)
			delete pIDat;
		break;
	}

	return RETCODE_SUCCESS;
}

//DialogSetTextClr (dialog dlg, int r, int g, int b)
//set dialog text color
PROTECTED long S_DialogSetTextClr(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			GameDialog *pDlg;
			int r,g,b;

			pDlg = (GameDialog *)IDPageQuery(*(Id*)ScriptParamGetVar(param, 0));

			r = *(int *)ScriptParamGetVar(param, 1);
			g = *(int *)ScriptParamGetVar(param, 2);
			b = *(int *)ScriptParamGetVar(param, 3);

			if(pDlg)
				pDlg->SetStringClr(r,g,b);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}