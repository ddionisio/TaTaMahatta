#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_Button(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new button
	Button *newObj = new Button; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	///////////////////////////////////////////////////////
	//set the 'off' location
	newObj->SetOffLoc(newObj->GetLoc());

	const char *pStr;
	int			iVal;

	///////////////////////////////////////////////////////
	//is pushable?
	pStr = entityDat.GetVal("bPushable");
	if(pStr)
		sscanf(pStr, "%d", &iVal);
	else
		iVal = 0;

	newObj->SetFlag(OBJ_FLAG_PUSHABLE, iVal ? true : false);

	///////////////////////////////////////////////////////
	//can it only be turned on once?
	pStr = entityDat.GetVal("bOnce");
	if(pStr)
		sscanf(pStr, "%d", &iVal);
	else
		iVal = 0;

	newObj->SetFlag(OBJ_FLAG_ONCE_ONLY, iVal ? true : false);

	///////////////////////////////////////////////////////
	//set the 'on' location
	pStr = entityDat.GetVal("target");
	if(pStr)
	{
		D3DXVECTOR3 vec;
		TargetGet(pStr, vec);
		newObj->SetOnLoc(vec);
	}
	else
		newObj->SetOnLoc(newObj->GetLoc());

	//get script file
	char scriptPath[MAXCHARBUFF];

	strcpy(scriptPath, m_filePath.c_str());
	strcpy(GetExtension(scriptPath), SCENE_EXT);

	///////////////////////////////////////////////////////
	//set the script for 'on'
	pStr = entityDat.GetVal("OnScript");
	if(pStr)
		newObj->LoadScript(true, scriptPath, pStr);

	///////////////////////////////////////////////////////
	//set the script for 'off'
	pStr = entityDat.GetVal("OffScript");
	if(pStr)
		newObj->LoadScript(false, scriptPath, pStr);

	return RETCODE_SUCCESS;
}