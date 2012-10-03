#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_Block(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new block
	Block *newObj = new Block; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	///////////////////////////////////////////////////////
	//load up the debris model
	const char *pStr = entityDat.GetVal("debrisModel");
	if(pStr)
	{
		string modelPath = GAMEFOLDER;
		modelPath += "\\";
		modelPath += pStr;

		newObj->SetDebrisModel(MDLCreate(0, modelPath.c_str()));
	}

	///////////////////////////////////////////////////////
	//set the flag to pushable, if this block is pushable
	pStr = entityDat.GetVal("bPushable");

	if(pStr)
	{
		int pushable;

		sscanf(entityDat.GetVal("bPushable"), "%d", &pushable);

		if(pushable)
			newObj->SetFlag(OBJ_FLAG_PUSHABLE, true);
	}

	return RETCODE_SUCCESS;
}