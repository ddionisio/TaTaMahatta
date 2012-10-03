#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	CATTERSHROOM_ANIM_IDLE,
	CATTERSHROOM_ANIM_SPIT,
	CATTERSHROOM_ANIM_THROW_PREP,
	CATTERSHROOM_ANIM_THROW,
	CATTERSHROOM_ANIM_ATTACK,
	CATTERSHROOM_ANIM_STUCK,
	CATTERSHROOM_ANIM_DIG,
	CATTERSHROOM_ANIM_OUCH,
	CATTERSHROOM_ANIM_DEATH
} eShroomPaAnim;

CatterShroom::CatterShroom() : EnemyCommon(BOSS_CATTERSHROOM), 
m_actionState(CATTERSHROOM_STATE_IDLE), m_spitCur(0),
m_spikeCur(0), m_patternCur(0), m_attackRadiusSQ(0),
m_bTataInRange(false), m_patternInd(0), m_bFfieldOn(false)
{
	m_mucusID.counter = -1;
	m_gasID.counter = -1;

	SetFlag(CRE_FLAG_NOFREEZE, true);
	SetFlag(CRE_FLAG_GASIMMUNE, true);
	SetFlag(CRE_FLAG_STUNIMMUNE, true);
	SetFlag(CRE_FLAG_MELEEIMMUNE, true);
	SetFlag(CRE_FLAG_SPITIMMUNE, true);
}

CatterShroom::~CatterShroom()
{
	if(m_spitMdl)
		MDLDestroy(&m_spitMdl);

	if(m_spitFXTxt)
		TextureDestroy(&m_spitFXTxt);

	if(m_mucusMdl)
		MDLDestroy(&m_mucusMdl);

	if(m_mucusFXTxt)
		TextureDestroy(&m_mucusFXTxt);

	if(m_spikeMdl)
		MDLDestroy(&m_spikeMdl);

	if(m_ffieldOBJ)
	{
		OBJDestroy(&m_ffieldOBJ);
	}

	if(m_ffieldTxtFX)
		TextureDestroy(&m_ffieldTxtFX);

	if(m_gasTxt)
		TextureDestroy(&m_gasTxt);
}

void CatterShroom::ChangeBehavior(int state)
{
	m_actionState = state;

	BehaviorUpdate(true);
}

void CatterShroom::BehaviorUpdate(bool bBegin)
{
	switch(m_actionState)
	{
	case CATTERSHROOM_STATE_IDLE:
		if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//set to idle animation
			OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_IDLE);

			TimeReset(&m_idleDelay);
		}
		else if(m_bTataInRange)
		{
			if(TimeElapse(&m_idleDelay))
			{
				//prepare to attack
				m_patternCur = 0;

				//shuffle the pattern
				m_patternInd = (int)(GetMaxHit() - GetCurHit());
				
				if(m_patternInd >= m_patternAtk.size()) m_patternInd = m_patternAtk.size() - 1;

				vector<int>::iterator start, end;
				start = m_patternAtk[m_patternInd].begin();
				end = m_patternAtk[m_patternInd].end();
				random_shuffle(start, end);

				//change to pattern attack
				ChangeBehavior(CATTERSHROOM_STATE_ATTACK_PATTERN);
			}
		}
		break;

	case CATTERSHROOM_STATE_ATTACK_PATTERN:
		if(bBegin)
		{
			//activate barriers if not yet done
			FFieldActivate(true);
			GasActivate(true);

			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//check to see first if pattern is done
			if(m_patternCur >= m_patternAtk[m_patternInd].size())
			{
				//set to claw attack!
				ChangeBehavior(CATTERSHROOM_STATE_ATTACK);
			}
			//if current Ta-Ta is not in range, then just stand-by
			else if(!m_bTataInRange)
			{
				ChangeBehavior(CATTERSHROOM_STATE_IDLE);
			}
			else
			{
				//set to idle animation
				OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_IDLE);

				TimeReset(&m_patternDelay);
			}
		}
		else if(TimeElapse(&m_patternDelay))
		{
			//change attack depending on pattern
			switch(m_patternAtk[m_patternInd][m_patternCur])
			{
			case 0: //Spit!!!
				ChangeBehavior(CATTERSHROOM_STATE_SPIT);
				break;

			case 1: //Mucus
				ChangeBehavior(CATTERSHROOM_STATE_MUCUS);
				break;
			}

			m_patternCur++;
		}
		break;

	case CATTERSHROOM_STATE_SPIT:
		if(bBegin)
		{
			//set to spit animation
			OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_SPIT);
			OBJPauseOnStateEnd(GetOBJ(), true);

			m_spitCur = 0;

			TimeReset(&m_spitDelay);

			//play spit sound
			CREPlaySound(19);
		}
		else 
		{
			//set direction towards current Ta-Ta
			Creature *pCre;
		
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				D3DXVECTOR3 vec(pCre->GetLoc()-GetLoc());
				vec.y = 0;
				D3DXVec3Normalize(&vec, &vec);
				
				SetDir(vec);

				if(OBJIsStateEnd(GetOBJ()))
				{
					if(TimeElapse(&m_spitDelay))
					{
						D3DXVECTOR3 dir(GetDir());
						D3DXMATRIX theMtx; D3DXMatrixRotationY(&theMtx, TataRand(m_spitAngleMin, m_spitAngleMax));
						D3DXVec3TransformNormal(&dir, &dir, &theMtx);
						SetDir(dir);

						//spit!!!
						D3DXVECTOR3 spitLoc;
						OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "mouth"), (float*)spitLoc);
						new Spit(GetID(), spitLoc, m_spitSpd, m_spitMdl, 
							m_spitFXTxt, 0, 255, 0);

						m_spitCur++;
					}
				}
			}

			if(m_spitCur > m_spitMax)
				//get back to attack pattern
				ChangeBehavior(CATTERSHROOM_STATE_ATTACK_PATTERN);
		}
		break;

	case CATTERSHROOM_STATE_MUCUS:
		if(bBegin)
		{
			OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_THROW_PREP);
		}
		else
		{
			//set direction towards current Ta-Ta
			Creature *pCre;
		
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				D3DXVECTOR3 vec(pCre->GetLoc()-GetLoc());
				vec.y = 0;
				D3DXVec3Normalize(&vec, &vec);
				
				SetDir(vec);

				//This is stupid coding...but it works
				//...BETA is almost due, so...

				if(OBJIsStateEnd(GetOBJ())) 
				{
					int state = OBJGetCurState(GetOBJ());

					if(state == CATTERSHROOM_ANIM_THROW_PREP)
					{
						OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_THROW);

						//throw an upward spit
						SetDir(D3DXVECTOR3(0,1,0));

						D3DXVECTOR3 spitLoc;
						OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "mouth"), (float*)spitLoc);

						gfxBound bound = MDLGetBound(m_mucusMdl);
						spitLoc.y += bound.radius;

						Spit *pSpit = new Spit(GetID(), spitLoc, m_mucusSpd*2, m_mucusMdl, 
								m_mucusFXTxt, 0, 255, 0);

						if(pSpit)
							m_mucusID = pSpit->GetID();

						//play a stupid sound 'pop'
						CREPlaySound(33);
					}
					else if(state == CATTERSHROOM_ANIM_THROW)
					{
						//throw a downward spit
						SetDir(D3DXVECTOR3(0,-1,0));

						//set location near the cieling
						gfxTrace trace;
						D3DXVECTOR3 pt2(GetLoc());
						pt2.y += 1000000;

						QBSPCollision(g_world->GetMap(), 0, 0,
							  (float*)GetLoc(), (float*)pt2, &trace,
							  1, -1);

						gfxBound bound = MDLGetBound(m_mucusMdl);

						D3DXVECTOR3 spitLoc(pCre->GetLoc());
						spitLoc.y = GetLoc().y + (pt2.y - GetLoc().y)*trace.t;
						spitLoc.y -= bound.max[eY]*2;

						Spit *pSpit = new Spit(GetID(), spitLoc, m_mucusSpd, m_mucusMdl, 
								m_mucusFXTxt, 0, 255, 0);
						pSpit->SetFlag(ENTITY_FLAG_SHADOW, true);

						//get back to attack pattern
						ChangeBehavior(CATTERSHROOM_STATE_ATTACK_PATTERN);
					}
				}
			}
		}
		break;

	case CATTERSHROOM_STATE_ATTACK:
		if(bBegin)
		{
			//set to attack animation
			OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_ATTACK);
			OBJPauseOnStateEnd(GetOBJ(), true);

			m_spikeCur = 0;

			//set target as current Ta-Ta
			//set direction towards current Ta-Ta
			Creature *pCre;
		
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				m_spikeTarget = pCre->GetLoc();

				D3DXVECTOR3 vec(m_spikeTarget-GetLoc());
				vec.y = 0;

				m_spikeVec = vec;

				D3DXVec3Normalize(&vec, &vec);
				
				SetDir(vec);
			}

			gfxBound bound = MDLGetBound(m_spikeMdl);
			m_spikeStartY = GetLoc().y + BoundGetMin().y + bound.min[eY];
			m_spikeEndY = GetLoc().y  + BoundGetMin().y + bound.max[eY];

			TimeReset(&m_spikeDelay);

			//turn off gas field
			GasActivate(false);
		}
		else if(OBJIsStateEnd(GetOBJ()))
		{
			if(TimeElapse(&m_spikeDelay))
			{
				D3DXVECTOR3 loc(GetLoc());

				loc += m_spikeVec*((float)m_spikeCur/(float)m_spikeMax);
				loc.y = m_spikeStartY;

				new Spike(GetID(), m_spikeMdl, loc, m_spikeEndY, m_spikeSpd);

				m_spikeCur++;
			}

			if(m_spikeCur > m_spikeMax)
				//done with spike attack, set to 'stuck'
				ChangeBehavior(CATTERSHROOM_STATE_STUCK);
		}
		break;

	case CATTERSHROOM_STATE_STUCK:
		if(bBegin)
		{
			OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_STUCK);
			OBJPauseOnStateEnd(GetOBJ(), false);

			TimeReset(&m_stuckDelay);
		}
		else if(TimeElapse(&m_stuckDelay))
			//done with stuck, get back to idle
			ChangeBehavior(CATTERSHROOM_STATE_IDLE);
		break;

	case CATTERSHROOM_STATE_DIG:
		if(GetCurHit() > 0)
		{
			if(bBegin)
			{
				OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_DIG);
			}
			else if(OBJIsStateEnd(GetOBJ()))
			{
				//get the next dig location
				int digInd = (int)(GetMaxHit() - GetCurHit() - 1);

				if(digInd < 0) digInd = 0;
				else if(digInd >= m_digLocs.size()) digInd = m_digLocs.size()-1;

				SetLoc(m_digLocs[digInd]);

				ChangeBehavior(CATTERSHROOM_STATE_IDLE);
			}
		}
		break;
	}
}

void CatterShroom::GasActivate(bool bYes)
{
	Gasus *pGas = (Gasus *)IDPageQuery(m_gasID);

	if(bYes && !pGas)
	{
		pGas = new Gasus(GetID(), GetLoc(), m_gasBound, D3DXVECTOR3(0,1,0), 
			m_gasTxt, m_gasSize, 100, 255, 100, -1);

		m_gasID = pGas->GetID();
	}
	else if(!bYes && pGas)
		pGas->SetFlag(ENTITY_FLAG_POLLDEATH,true);
}

void CatterShroom::FFieldActivate(bool bYes)
{
	m_bFfieldOn = bYes;

	if(bYes)
	{
		if(OBJIsDeactivate(m_ffieldOBJ))
		{
			OBJActivate(m_ffieldOBJ, true);
			OBJAddChild(GetOBJ(), m_ffieldOBJ, -1, false);

			//play magical sound
			CREPlaySound(31);
		}
	}
	else
	{
		if(!OBJIsDeactivate(m_ffieldOBJ))
		{
			OBJRemoveFromParent(m_ffieldOBJ);
			OBJActivate(m_ffieldOBJ, false);

			//cause explosion
			fxExplode_init tFX;
			tFX.explodeTxt = m_ffieldTxtFX;
			tFX.r = 255; tFX.g = tFX.b = 0;
			tFX.scale = 5;
			memcpy(tFX.center, (float*)GetLoc(), sizeof(tFX.center));
			tFX.radius = BoundGetRadius()*2;
			tFX.maxParticle = 30;
			tFX.delay = 1500;

			PARFXCreate(ePARFX_EXPLODE, &tFX, -1, 0, -1, 0);

			//play exploding sound 2
			CREPlaySound(30);
		}
	}
}

void CatterShroom::FFieldUpdate()
{
	if(!OBJIsDeactivate(m_ffieldOBJ))
	{
		OBJRotY(m_ffieldOBJ, m_ffieldRotSpd.MoveUpdate(g_timeElapse));
	}
}

bool CatterShroom::FFieldIsActive()
{
	if(OBJIsDeactivate(m_ffieldOBJ))
		return false;

	return true;
}

int CatterShroom::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			char buff[MAXCHARBUFF]={0};
			hCFG cfg = (hCFG)wParam;

			///////////////////////////////////////////////////////////////
			// Spit

			//Load the model for spit
			if(CfgGetItemStr(cfg, "spit", "model", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_spitMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "spit", "trail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_spitFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load attack stuff
			m_spitSpd = CfgGetItemFloat(cfg, "spit", "spd");

			m_spitMax = CfgGetItemInt(cfg, "spit", "max");

			m_spitAngleMin = D3DXToRadian(CfgGetItemFloat(cfg, "spit", "angleMin"));
			m_spitAngleMax = D3DXToRadian(CfgGetItemFloat(cfg, "spit", "angleMax"));

			TimeInit(&m_spitDelay, CfgGetItemFloat(cfg, "spit", "delay"));
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Big Mucus
			
			//Load the model for mucus
			if(CfgGetItemStr(cfg, "mucus", "model", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_mucusMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for mucus trails
			if(CfgGetItemStr(cfg, "mucus", "trail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_mucusFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			m_mucusSpd = CfgGetItemFloat(cfg, "mucus", "spd");
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Spike

			//Load the model for spike
			if(CfgGetItemStr(cfg, "spike", "model", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_spikeMdl = MDLCreate(0, mdlPath.c_str());
			}

			m_spikeSpd = CfgGetItemFloat(cfg, "spike", "spd");

			m_spikeMax = CfgGetItemInt(cfg, "spike", "max");

			TimeInit(&m_spikeDelay, CfgGetItemFloat(cfg, "spike", "delay"));
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Fire Force Field
			
			//Load the model for force field
			if(CfgGetItemStr(cfg, "forcefield", "model", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				hMDL ffieldMDL = MDLCreate(0, mdlPath.c_str());

				m_ffieldOBJ = OBJCreate(0, ffieldMDL,0,0,0,eOBJ_STILL,0);

				if(m_ffieldOBJ)
					OBJActivate(m_ffieldOBJ, false);
			}

			//load rotation speed
			m_ffieldRotSpd.MoveSetSpeed(CfgGetItemFloat(cfg, "forcefield", "rotSpd"));

			//load Force Field FX texture
			if(CfgGetItemStr(cfg, "forcefield", "FXtexture", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_ffieldTxtFX = TextureCreate(0, txtPath.c_str(), false, 0);
			}
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Gaseous Area
	
			//load gas FX texture
			if(CfgGetItemStr(cfg, "gas", "texture", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_gasTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			m_gasSize = CfgGetItemFloat(cfg, "gas", "size");

			//m_gasBound
			if(CfgGetItemStr(cfg, "gas", "bound", buff))
				sscanf(buff, "%f,%f,%f", &m_gasBound.x, &m_gasBound.y, &m_gasBound.z);
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Misc.

			m_attackRadiusSQ = CfgGetItemFloat(cfg, "special", "attackRadius");
			m_attackRadiusSQ *= m_attackRadiusSQ;

			TimeInit(&m_stuckDelay, CfgGetItemFloat(cfg, "special", "stuckDelay"));
			TimeInit(&m_idleDelay, CfgGetItemFloat(cfg, "special", "idleDelay"));
			TimeInit(&m_patternDelay, CfgGetItemFloat(cfg, "special", "patternDelay"));

			//set pattern attack size to maximum health points
			m_patternAtk.resize((int)GetMaxHit());

			//get patterns
			int numSpit, numMucus, total;

			for(int i = 0; i < m_patternAtk.size(); i++)
			{
				sprintf(buff, "pattern_%d", i);

				numSpit  = CfgGetItemInt(cfg, buff, "spit");
				numMucus = CfgGetItemInt(cfg, buff, "mucus");

				total = numSpit + numMucus;

				//fill in the patterns
				if(total > 0)
				{
					m_patternAtk[i].resize(total);

					for(int s = 0; s < numSpit; s++)
						m_patternAtk[i][s] = 0;

					for(int m = numSpit; m < total; m++)
						m_patternAtk[i][m] = 1;
				}
			}

			char tgtName[MAXCHARBUFF];
			D3DXVECTOR3 dTgt;

			int maxDig = (int)(GetMaxHit() - 1);

			if(maxDig > 0)
			{
				m_digLocs.resize(maxDig);

				for(int d = 0; d < m_digLocs.size(); d++)
				{
					sprintf(buff, "digDest_%d", d);
					if(CfgGetItemStr(cfg, "special", buff, tgtName))
					{
						if(!g_world->TargetGet(tgtName, m_digLocs[d]))
							m_digLocs[d] = GetLoc();
					}
					else
						m_digLocs[d] = GetLoc();
				}
			}

			///////////////////////////////////////////////////////////////

			//set stupid draw bound
			float dbMin[eMaxPt]={-BoundGetRadius(),-BoundGetRadius(),-BoundGetRadius()};
			float dbMax[eMaxPt]={BoundGetRadius(),BoundGetRadius(),BoundGetRadius()};
			OBJSetDrawBound(GetOBJ(), dbMin, dbMax);

			//set to idle
			ChangeBehavior(CATTERSHROOM_STATE_IDLE);

			FFieldActivate(true);
			GasActivate(true);
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			FFieldUpdate();

			//check if current Ta-Ta is in range
			D3DXVECTOR3 vec;
			float vecLenSq;
			Creature *pCre;
			
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				vec = pCre->GetLoc() - GetLoc();
				vecLenSq = D3DXVec3LengthSq(&vec);

				if(vecLenSq <= m_attackRadiusSQ
					&& pCre->GetLoc().y >= (GetLoc().y + BoundGetMin().y + pCre->BoundGetMin().y)
					&& pCre->GetLoc().y <= (GetLoc().y + BoundGetMax().y + pCre->BoundGetMax().y))
				{
					m_bTataInRange = true;
				}
				else
					m_bTataInRange = false;
			}

			BehaviorUpdate(false);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
			{
				//check to see if it is the freeze attack
				//is force field active?
				if(pEntity->GetSubType() == PROJ_FROST
					&& FFieldIsActive())
					//remove force field
					FFieldActivate(false);
				//check to see if it is a melee attack
				//is force field not active?
				else if(pEntity->GetSubType() == PROJ_ATTACKMELEE
					&& !FFieldIsActive())
				{
					//get hit
					Hit();

					//dig
					ChangeBehavior(CATTERSHROOM_STATE_DIG);
				}
					
			}
		}
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 20;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}

int CatterShroom::GetDeathState()
{
	return CATTERSHROOM_ANIM_DEATH;
}

int CatterShroom::GetOuchState()
{
	return CATTERSHROOM_ANIM_OUCH;
}

void CatterShroom::MoveAnim(eMoveType type)
{
	int OBJState = OBJGetCurState(GetOBJ());

	if(OBJState == CATTERSHROOM_ANIM_OUCH && OBJIsStateEnd(GetOBJ()))
		OBJSetState(GetOBJ(), CATTERSHROOM_ANIM_IDLE);
}