#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_Sign(hQBSP qbsp, const EntityParse & entityDat)
{
	hTXT txt;
	int r,g,b,a;

	//create new sign
	Sign *newObj = new Sign; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//load image
	pStr = entityDat.GetVal("image");

	if(pStr)
	{
		string imgPath = GAMEFOLDER;
		imgPath += "\\";
		imgPath += pStr;

		txt = TextureCreate(0, imgPath.c_str(), false, 0);
	}

	///////////////////////////////////////////////////////
	//get color
	pStr = entityDat.GetVal("color");

	if(pStr)
	{
		sscanf(pStr, "%d %d %d %d", &r,&g,&b,&a);
	}

	if(txt)
	{
		float sX = (SCRN_W/2) - (TextureGetWidth(txt)/2);
		float sY = (SCRN_H/2) - (TextureGetHeight(txt)/2);
		float eX = sX + TextureGetWidth(txt);
		float eY = sY + TextureGetHeight(txt);

		sX /= SCRN_W; sY /= SCRN_H;
		eX /= SCRN_W; eY /= SCRN_H;

		newObj->SetImageDisplay(ImageDisplayAdd(txt, sX,sY,eX,eY, r,g,b,a));
		TextureDestroy(&txt);
	}

	return RETCODE_SUCCESS;
}