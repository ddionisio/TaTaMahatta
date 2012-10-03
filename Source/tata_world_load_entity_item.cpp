#include "tata_main.h"
#include "tata_world.h"

RETCODE World::EntityLoad_Item(hQBSP qbsp, const EntityParse & entityDat)
{
	Item * newItm = 0;

	const char * pType = entityDat.GetVal("ID");
	const char * pName = entityDat.GetVal("type");

	switch(ItemGetType(pType))
	{
	case ITM_RESOURCE:
		newItm = new Resource;
		break;
	case ITM_EGG:
		break;
	case ITM_COLLECTION:
		{
			bool bCreateItm = true;

			//collection already exists
			if(ProfileGetCfg())
			{
				if(CfgGetItemInt(ProfileGetCfg(), PROFILE_COLLECT_SECTION, pName)
					== 1)
				{
					SetArTaFactMin(GetArTaFactMin()+1);
					bCreateItm = false;
				}
			}

			//check to see if this artafact has already been collected
			if(bCreateItm)
				newItm = new Collectible;

			SetArTaFactMax(GetArTaFactMax()+1);
		}
		break;
	}

	if(newItm && pName)
	{
		//load item
		newItm->Load(pName);

		//set location
		D3DXVECTOR3 theLoc;
		sscanf(entityDat.GetVal("origin"), "%f %f %f", 
			&theLoc.x, &theLoc.z, &theLoc.y);

		theLoc.z *= -1;

		newItm->SetLoc(theLoc);
		newItm->SetBoyantMinMax();
	}

	return RETCODE_SUCCESS;
}