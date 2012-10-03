#include "tata_main.h"
#include "tata_world.h"

#include "tata_creature_tata.h"

RETCODE World::EntityLoad_TaTa(hQBSP qbsp, const EntityParse & entityDat)
{
	//first check to see if we have an available slot in the play list.
	//if(TataIsPlayListFull())
	//	return RETCODE_SUCCESS; //ignore adding anything...

	string tataTypeName;

	const char * pName = entityDat.GetVal("ID");

	bool bGotName = false;

	//if ID is specified, then set the Ta-Ta type to that
	if(pName)
	{
		tataTypeName = pName;

		bGotName = true;
	}
	//otherwise get the next available Ta-Ta type in the roster
	else
		//make sure there is a Ta-Ta available inside the roster
		bGotName = TataRetrieveFromRoster(tataTypeName);

	//we have the Ta-Ta type name, so now get the path and type
	if(bGotName)
	{
		/////////////////////////////////
		//set location
		D3DXVECTOR3 theLoc;

		sscanf(entityDat.GetVal("origin"), "%f %f %f", 
			&theLoc.x, &theLoc.z, &theLoc.y);

		theLoc.z *= -1;

		Creature *newTata;

		if(entityDat.GetVal("datType") 
			&& stricmp(entityDat.GetVal("datType"), "none") != 0)
			newTata = TataSpawn(tataTypeName.c_str(), theLoc, entityDat.GetVal("datType"));
		else
			newTata = TataSpawn(tataTypeName.c_str(), theLoc);

		if(newTata)
		{
			newTata->SetSpawnLoc(theLoc);

			/////////////////////////////////
			//set direction
			if(entityDat.GetVal("angle"))
			{
				D3DXMATRIX mtx;
				D3DXVECTOR3 dir(1,0,0);
				float angle;
				sscanf(entityDat.GetVal("angle"), "%f", &angle);
				
				angle = D3DXToRadian(angle);

				D3DXMatrixRotationY(&mtx, angle);
				D3DXVec3TransformNormal(&dir, &dir, &mtx);

				newTata->SetDir(dir);
			}

			/////////////////////////////////
			//set waypoint
			const char * pStr = entityDat.GetVal("waypointID");
			if(pStr)
				newTata->SetWayPt(WaypointGet(pStr));

			WayptNode  *pWayPt = newTata->GetWayPoint();

			if(pWayPt)
			{
				///////////////////////////////////////////////////////
				//get waypoint mode
				pStr = entityDat.GetVal("WPMode");
				if(pStr)
					pWayPt->SetMode(WaypointGetMode(pStr));
			}

			/////////////////////////////////
			//check if it is playable
			int NPC = 0;
			if(entityDat.GetVal("bNPC"))
				sscanf(entityDat.GetVal("bNPC"), "%d", &NPC);

			/////////////////////////////////
			//add Ta-Ta to the play list
			if(NPC == 0)
				TataAddToPlayList(newTata);
		}
	}

	return RETCODE_SUCCESS;
}