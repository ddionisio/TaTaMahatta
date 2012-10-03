#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

RETCODE World::EntityLoad_Steak(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new steak
	Steak *newObj = new Steak; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	return RETCODE_SUCCESS;
}