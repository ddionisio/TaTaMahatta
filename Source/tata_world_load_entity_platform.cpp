#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_Platform(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new platform
	Platform *newObj = new Platform; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//get the waypoint
	pStr = entityDat.GetVal("waypointID");
	if(pStr)
		newObj->SetWayPt(WaypointGet(pStr));

	WayptNode  *pWayPt = newObj->GetWayPoint();

	if(pWayPt)
	{
		///////////////////////////////////////////////////////
		//get backward mode
		pStr = entityDat.GetVal("bBackward");
		if(pStr)
		{
			int isBackward; sscanf(pStr, "%d", &isBackward);

			if(isBackward)
				pWayPt->Reverse(true);
		}

		///////////////////////////////////////////////////////
		//get waypoint mode
		pStr = entityDat.GetVal("mode");
		if(pStr)
			pWayPt->SetMode(WaypointGetMode(pStr));
	}

	///////////////////////////////////////////////////////
	//get platform speed
	pStr = entityDat.GetVal("spdDelay");
	if(pStr)
	{
		float spd; sscanf(pStr, "%f", &spd);

		newObj->SetMoveSpd(spd);
	}

	///////////////////////////////////////////////////////
	//is this a door?
	pStr = entityDat.GetVal("bDoor");
	if(pStr)
	{
		int isDoor; sscanf(pStr, "%d", &isDoor);

		if(isDoor)
			newObj->SetFlag(OBJ_FLAG_DOOR, true);
	}

	return RETCODE_SUCCESS;
}