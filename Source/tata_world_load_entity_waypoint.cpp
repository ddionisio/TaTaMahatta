#include "tata_main.h"
#include "tata_world.h"

RETCODE World::EntityLoad_Waypoint(hQBSP qbsp, const EntityParse & entityDat)
{
	string wayPtName = entityDat.GetVal("ID");

	Id wayPtID = WaypointGet(wayPtName.c_str());

	Waypoint * pWayPt = (Waypoint *)IDPageQuery(wayPtID);

	if(!pWayPt) //create the waypoint if it does not exist
	{
		pWayPt = new Waypoint(wayPtName.c_str());

		//add it to the waypoint list
		m_waypts.push_back(pWayPt);
	}

	//add new node
	wayPt newPt; memset(&newPt, 0, sizeof(wayPt));

	//get index
	sscanf(entityDat.GetVal("Index"), "%d", &newPt.ind);

	//get location
	sscanf(entityDat.GetVal("origin"), "%f %f %f", 
		&newPt.pt.x, &newPt.pt.z, &newPt.pt.y);

	newPt.pt.z *= -1;

	//get delay
	const char * pDelayStr = entityDat.GetVal("delay");

	if(pDelayStr)
		sscanf(pDelayStr, "%lf", &newPt.delay);
	else
		newPt.delay = 0;

	//////////////////////////////////////////
	//See if we are targeting something
	//If so, set target
	const char * pTargetStr = entityDat.GetVal("target");
	if(pTargetStr)
		TargetGet(pTargetStr, newPt.target);

	pWayPt->AddWayPt(newPt);

	return RETCODE_SUCCESS;
}