#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Sign::Sign() : Object(OBJECT_SIGN), m_bCollide(false)
{
	//we do not want signs to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);

	m_imageID.counter = -1;
}

Sign::~Sign()
{
	GameImage *pImg = (GameImage *)IDPageQuery(m_imageID);

	if(pImg)
		delete pImg;
}

void Sign::SetImageDisplay(const Id & ID)
{
	m_imageID = ID;
}

void Sign::Show(bool bShow)
{
	if(g_world)
		g_world->ImageDisplayFadeIn(m_imageID, bShow);

	m_bCollide = bShow;
}

int Sign::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		if(m_bCollide)
			m_bCollide = false;
		else if(!m_bCollide)
			Show(false);
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}