#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Steak::Steak() : Object(OBJECT_STEAK)
{
	//we do not want steaks to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);
}

Steak::~Steak()
{
}

int Steak::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}