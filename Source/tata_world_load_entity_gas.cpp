#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_OuchField(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new ouch field
	OuchField *newObj = new OuchField; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;
	int			iVal=0;

	///////////////////////////////////////////////////////
	//get hit damage amt.
	pStr = entityDat.GetVal("hitamt");
	if(pStr)
		sscanf(pStr, "%d", &iVal);

	newObj->OuchSetHitAmt(iVal);

	///////////////////////////////////////////////////////
	//see if it is a gas area
	pStr = entityDat.GetVal("bGas");
	if(pStr)
		sscanf(pStr, "%d", &iVal);
	else
		iVal = 0;

	newObj->OuchSetGasFlag(iVal ? true : false);

	//get gas FX stuff
	if(iVal == 1)
	{
		hTXT txt=0;
		int r=255,g=255,b=255;

		///////////////////////////////////////////////////////
		//get gas img.
		pStr = entityDat.GetVal("TextureFX");
		if(pStr)
		{
			string txtPath = GAMEFOLDER;
			txtPath += "\\";
			txtPath += pStr;

			txt = TextureCreate(0, txtPath.c_str(), false, 0);
		}

		///////////////////////////////////////////////////////
		//get gas clr
		pStr = entityDat.GetVal("clrFX");
		if(pStr)
			sscanf(pStr, "%d,%d,%d", &r,&g,&b);

		if(txt)
		{
			newObj->OuchSetFX(txt, (BYTE)r, (BYTE)g, (BYTE)b);
			TextureDestroy(&txt);
		}
	}

	return RETCODE_SUCCESS;
}