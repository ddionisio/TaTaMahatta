#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	FUNGASPY_ANIM_IDLE,
	FUNGASPY_ANIM_OUCH,
	FUNGASPY_ANIM_DEATH,
	FUNGASPY_ANIM_ATTACK
} eFungaSpyAnim;

FungaSpy::FungaSpy() : EnemyCommon(ENEMY_FUNGASPY),
m_angleScan(0), m_lengthScan(0), m_bScanRangeInit(false),
m_rotDir(1), m_spikeMDL(0), m_bWait(false),
m_angleAccum(0)
{
}

FungaSpy::~FungaSpy()
{
	if(m_spikeMDL)
		MDLDestroy(&m_spikeMDL);
}

int FungaSpy::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			char buff[MAXCHARBUFF]={0};
			hCFG cfg = (hCFG)wParam;

			//get the spike model
			if(CfgGetItemStr(cfg, "special", "spikeModel", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_spikeMDL = MDLCreate(0, mdlPath.c_str());
			}

			//get the spike speed
			m_spikeSpd = CfgGetItemFloat(cfg, "special", "spikeSpd");

			//create the scanner projectile
			Scanner *newScan = new Scanner(GetID(), OBJJointGetIndex(GetOBJ(), "eye_b"), OBJJointGetIndex(GetOBJ(), "eye_e"), m_lengthScan);
			m_scannerID = newScan->GetID();

			//extend the bound so that we can see our laser sight
			D3DXVECTOR3 bMin(BoundGetMin()), bMax(BoundGetMax());

			bMin.x -= m_lengthScan;
			bMin.y -= m_lengthScan;
			bMin.z -= m_lengthScan;
			
			bMax.x += m_lengthScan;
			bMax.y += m_lengthScan;
			bMax.z += m_lengthScan;

			OBJSetDrawBound(GetOBJ(), (float*)bMin, (float*)bMax);
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			if(!m_bScanRangeInit)
			{
				m_bScanRangeInit = true;

				m_origDir = GetDir();

				//float rot[eMaxPt];
				//OBJGetRotate(GetOBJ(), rot);

				m_angleMin = - m_angleScan;//rot[eY] - m_angleScan;
				m_angleMax = m_angleScan;//rot[eY] + m_angleScan;
			}

			//get back to scanning
			if(OBJIsStateEnd(GetOBJ()) 
				&& OBJGetCurState(GetOBJ()) != FUNGASPY_ANIM_IDLE)
				OBJSetState(GetOBJ(), FUNGASPY_ANIM_IDLE);
			//while scanning...
			else if(OBJGetCurState(GetOBJ()) == FUNGASPY_ANIM_IDLE)
			{
				if(m_bWait)
				{
					if(TimeElapse(&m_scanDelay))
						m_bWait = false;
				}
				else
				{
					//have a look around
					m_angleAccum += m_rotDir*GetRotMove().MoveUpdate(g_timeElapse);

					D3DXVECTOR3 dir;
					D3DXMATRIX theMtx;
					D3DXMatrixRotationY(&theMtx, m_angleAccum);
					D3DXVec3TransformNormal(&dir, &m_origDir, &theMtx);
					SetDir(dir);
					
					//Rotate(m_rotDir);

					//float rot[eMaxPt];
					//OBJGetRotate(GetOBJ(), rot);

					if((m_angleAccum <= m_angleMin && m_rotDir < 0)
						|| (m_angleAccum >= m_angleMax && m_rotDir > 0))
					{
						m_rotDir *= -1;
						m_bWait = true;
						TimeReset(&m_scanDelay);
					}
				}

				//check to see if the scanner hits something
				Scanner *pScan = (Scanner *)IDPageQuery(m_scannerID);

				if(pScan)
				{
					EntityCommon *pEntity = (EntityCommon *)IDPageQuery(pScan->GetLastCollided());

					//check to see if this entity is a Ta-Ta
					if(pEntity && pEntity->GetEntityType() == ENTITY_TYPE_TATA)
					{
						Creature *pCre = (Creature *)pEntity;

						if(pCre->GetStatus() == CRESTATUS_NORMAL)
						{
							//change to attack animation
							OBJSetState(GetOBJ(), FUNGASPY_ANIM_ATTACK);

							//initialize spike
							D3DXVECTOR3 spikeLoc(pCre->GetLoc());
							float maxY = spikeLoc.y;
							spikeLoc.y -= (pCre->BoundGetMax().y + MDLGetBound(m_spikeMDL).radius);

							if(m_spikeMDL)
							{
								new Spike(GetID(), m_spikeMDL, spikeLoc, 
									maxY, m_spikeSpd);
							}
						}
					}
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}

int FungaSpy::GetDeathState()
{
	return FUNGASPY_ANIM_DEATH;
}

int FungaSpy::GetOuchState() 
{ 
	return FUNGASPY_ANIM_OUCH; 
}

void FungaSpy::SetAngleScan(float radian)
{
	m_angleScan = radian;
}

float FungaSpy::GetAngleScan()
{
	return m_angleScan;
}

void FungaSpy::SetLengthScan(float l)
{
	m_lengthScan = l;
}

float FungaSpy::GetLengthScan()
{
	return m_lengthScan;
}

void FungaSpy::SetScanDelay(double delay)
{
	TimeInit(&m_scanDelay, delay);
}