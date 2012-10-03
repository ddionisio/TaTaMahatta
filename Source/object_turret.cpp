#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_projectiles_common.h"

#include "tata_globals.h"

Turret::Turret() : Object(OBJECT_TURRET), m_projMdl(0), m_projFXTxt(0),
m_attackSpd(0)
{
	//we do not want turrets to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);

	TimeInit(&m_soundDelay, 5000);
}

Turret::~Turret()
{
	if(m_projMdl)
		MDLDestroy(&m_projMdl);

	if(m_projFXTxt)
		TextureDestroy(&m_projFXTxt);
}

void Turret::SetProjMDL(hMDL mdl)
{
	m_projMdl = mdl;
}

void Turret::SetProjTXT(hTXT txt)
{
	m_projFXTxt = txt;
}

void Turret::SetProjSpd(float spd)
{
	m_attackSpd = spd;
}

void Turret::SetProjDelay(double d)
{
	TimeInit(&m_attackDelay, d);
}

void Turret::SetProjClr(BYTE r, BYTE g, BYTE b)
{
	m_r = r; m_g = g; m_b = b;
}

int Turret::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			if(TimeElapse(&m_attackDelay))
			{
				if(m_projMdl && m_projFXTxt)
				{
					//cannon sound
					if(TimeElapse(&m_soundDelay))
					{
						BASS_3DVECTOR pos; memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
						BASS_3DVECTOR orient; memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;
						TaTaSoundPlay(28, &pos, &orient, 0);
					}

					new Spit(GetID(), GetLoc(), m_attackSpd, m_projMdl, m_projFXTxt, m_r, m_g, m_b);
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 29;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}