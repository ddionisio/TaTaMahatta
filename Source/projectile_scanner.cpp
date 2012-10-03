#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

Scanner::Scanner(const Id & owner, int jointBeginInd, int jointEndInd, float length)
: Projectile(PROJ_SCANNER, owner), m_length(length), 
   m_jBInd(jointBeginInd), m_jEInd(jointEndInd),
   m_tLast(1)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	if(pEntity)
	{
		SetFlag(PROJ_FLAG_OBJINVUL, true);

		hOBJ obj = pEntity->GetOBJ();

		D3DXVECTOR3 jB, jE;

		OBJJointGetTranslate(obj, m_jBInd, (float *)jB);
		OBJJointGetTranslate(obj, m_jEInd, (float *)jE);

		m_normJoint = jB - jE;
		m_normJoint.y = 0;
		
		D3DXVec3Normalize(&m_normJoint, &m_normJoint);

		D3DXVECTOR3 v(1, 1, 1);

		BoundSetMax(v);
		BoundSetMin(-v);
		BoundSetRadius(1);
	}
}

Scanner::~Scanner()
{
}

int Scanner::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			hOBJ obj = pEntity->GetOBJ();

			D3DXVECTOR3 jB, jE, jV;

			////////////////////////////////////////////////////////
			//set location and velocity

			OBJJointGetWorldLoc(obj, m_jBInd, (float *)jB);

			jV = m_length*pEntity->GetDir();

			SetLoc(jB);
			SetVel(jV);

			////////////////////////////////////////////////////////
			//draw line segment
			OBJJointGetTranslate(obj, m_jBInd, (float *)jB);
			jE = jB + (m_tLast*m_length)*m_normJoint;
			jE.y = 0;
			OBJJointTranslate(obj, m_jEInd, jE, false);

			m_tLast = 1;
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			gfxTrace *pTrace = (gfxTrace *)lParam;

			if((pTrace->t < m_tLast
				&& pTrace->t > 0)
				|| m_tLast == 1)
				m_tLast = pTrace->t;
		}
		break;

	case ENTITYMSG_WORLDCOLLIDE:
		{
			gfxTrace *pTrace = (gfxTrace *)lParam;

			if(m_tLast == 1)
				m_tLast = pTrace->t;
		}
		break;

	case ENTITYMSG_DEATH:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}