#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

/////////////////////////////////////////////////////////////////////////////
// Lightning Area
/////////////////////////////////////////////////////////////////////////////

LightningArea::LightningArea() : Object(OBJECT_LIGHTNING), m_lightning(0),
m_bInfinite(false)
{
	//we do not want lightnings to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);
}

LightningArea::~LightningArea()
{
	if(m_lightning)
		PARFXDestroy(&m_lightning);
}

void LightningArea::LightningInitFX(fxLightningY_init *pDat)
{
	if(m_lightning)
		PARFXDestroy(&m_lightning);

	m_lightning = PARFXCreate(ePARFX_LIGHTNINGY, pDat, -1, 0, -1, 0);

	if(m_lightning)
		PARFXActivate(m_lightning, false);
}

void LightningArea::LightningSetDeactivateDelay(double d)
{
	if(d >= 0)
		TimeInit(&m_deactivateDelay, d);
	else
		m_bInfinite = true;
}

int LightningArea::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		if(!CheckFlag(OBJ_FLAG_INACTIVE)
			&& !PARFXIsActive(m_lightning))
		{
			PARFXActivate(m_lightning, true);
			TimeReset(&m_deactivateDelay);

			//play lightning sound
			BASS_3DVECTOR pos; memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
			TaTaSoundPlay(43, &pos, 0, 0);
		}
		else if(!m_bInfinite && TimeElapse(&m_deactivateDelay))
		{
			SetFlag(OBJ_FLAG_INACTIVE, true);
			PARFXActivate(m_lightning, false);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}