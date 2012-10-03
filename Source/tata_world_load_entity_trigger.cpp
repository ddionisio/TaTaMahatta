#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_Trigger(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new trigger
	Trigger *newObj = new Trigger; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;
	int			iVal;

	///////////////////////////////////////////////////////
	//can it only be turned on once?
	pStr = entityDat.GetVal("bOnce");
	if(pStr)
		sscanf(pStr, "%d", &iVal);
	else
		iVal = 0;

	newObj->SetFlag(OBJ_FLAG_ONCE_ONLY, iVal ? true : false);

	//get script file
	char scriptPath[MAXCHARBUFF];

	strcpy(scriptPath, m_filePath.c_str());
	strcpy(GetExtension(scriptPath), SCENE_EXT);

	///////////////////////////////////////////////////////
	//check if we want multiple entities to activate the trigger
	pStr = entityDat.GetVal("bAllowMultiple");
	if(pStr)
	{
		sscanf(pStr, "%d", &iVal);
		newObj->AllowMultipleEntities(iVal == 1 ? true : false);
	}

	///////////////////////////////////////////////////////
	//set the script for 'on'
	pStr = entityDat.GetVal("script");
	if(pStr)
		newObj->LoadScript(scriptPath, pStr);

	return RETCODE_SUCCESS;
}