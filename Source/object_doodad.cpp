#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Doodad::Doodad() : Object(OBJECT_DOODAD), m_collectReq("none")
{
	//we do not want platforms to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);
}

Doodad::~Doodad()
{
}

const char * Doodad::GetCollectionReq()
{
	if(strcmp(m_collectReq.c_str(), "none") == 0)
		return 0;
	
	return m_collectReq.c_str();
}

void Doodad::SetCollectReq(const char *str)
{
	m_collectReq = str;
}

int Doodad::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		//Update waypoint
		if(m_pWayPt)
		{
			D3DXVECTOR3 destPt, target, lookDir;

			//GetCurrentLinearLoc(&destPt);
			m_pWayPt->GetCurrentCurvedLoc(&destPt, &target);

			lookDir = target - GetLoc();
			//D3DXVec3Normalize(&lookDir, &lookDir);
			SetDir(lookDir);

			D3DXVECTOR3 dir(destPt - GetLoc());
			float len = D3DXVec3Length(&dir);

			float spd = m_moveSpd.MoveUpdate(g_timeElapse);
			
			if(len > spd)
			{
				dir /= len;

				SetVel(GetVel() + (spd*dir));
			}
			//we are close enough
			//get to next waypoint node
			else
			{
				SetVel(dir);

				m_pWayPt->GotoNext();
			}
			
			//if(m_pWayPt->IsDone())
			//	RemoveWayPt();
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;

	case ENTITYMSG_PROFILEPREP:
		Show(wParam == 1 ? true : false);
		break;
	}

	return RETCODE_SUCCESS;
}
