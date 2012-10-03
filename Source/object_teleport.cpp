#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Teleport::Teleport() : Object(OBJECT_TELEPORT), m_dest(0,0,0)
{
	//we do not want teleports to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);
}

Teleport::~Teleport()
{
}

void Teleport::TeleportSetDest(const D3DXVECTOR3 & loc)
{
	m_dest = loc;
}

void Teleport::TeleportSetFX(hTXT txt, BYTE r, BYTE g, BYTE b)
{
	fxExpand_init tFX;

	tFX.expandTxt = txt;

	tFX.r = r;
	tFX.g = g;
	tFX.b = b;

	memcpy(tFX.center, (float*)GetLoc(), sizeof(float)*eMaxPt);

	tFX.normal[eX] = tFX.normal[eZ] = 0;
	tFX.normal[eY] = 1;

	tFX.scaleStart = 0;
	tFX.scaleEnd = BoundGetMax().x > BoundGetMax().z ? BoundGetMax().x : BoundGetMax().z;
	tFX.scaleEnd *= 2;

	tFX.delay = 1000;
	
	tFX.bRepeat = true;

	PARFXCreate(ePARFX_EXPAND, &tFX, -1, 0, -1, 0);
}

int Teleport::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
				|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
			{
				Creature *pCre = (Creature *)pEntity;
				pCre->Teleport(m_dest);
			}
			else if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT)
			{
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}