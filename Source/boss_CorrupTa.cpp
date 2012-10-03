#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	CORRUPTA_ANIM_IDLE1,
	CORRUPTA_ANIM_IDLE2,
	CORRUPTA_ANIM_IDLE3,
	CORRUPTA_ANIM_CHARGE,
	CORRUPTA_ANIM_FIREBALL,
	CORRUPTA_ANIM_UPTHROW,
	CORRUPTA_ANIM_UPTHROWIDLE,
	CORRUPTA_ANIM_OUCH,
	CORRUPTA_ANIM_DEATH
} eCorrupTaAnim;

CorrupTa::CorrupTa() : EnemyCommon(BOSS_CORRUPTA), m_actionState(CORRUPTA_STATE_IDLE_1),
m_teleScale(1), m_bGenerateFField(false), m_teleInd(-1), m_suckSnd(0)
{
	m_gasID.counter = -1;

	SetFlag(CRE_FLAG_NOFREEZE, true);
	SetFlag(CRE_FLAG_GASIMMUNE, true);
	SetFlag(CRE_FLAG_STUNIMMUNE, true);
	SetFlag(CRE_FLAG_MELEEIMMUNE, true);
	SetFlag(CRE_FLAG_SPITIMMUNE, true);
}

CorrupTa::~CorrupTa()
{
	if(m_fxCharge)
		PARFXDestroy(&m_fxCharge);

	if(m_blastFXTxt)
		TextureDestroy(&m_blastFXTxt);

	if(m_ballMdl)
		MDLDestroy(&m_ballMdl);

	if(m_ballFXTxt)
		TextureDestroy(&m_ballFXTxt);

	if(m_meteorFXTxt)
		TextureDestroy(&m_meteorFXTxt);

	if(m_ffieldOBJ)
		OBJDestroy(&m_ffieldOBJ);

	if(m_ffieldTxtFX)
		TextureDestroy(&m_ffieldTxtFX);

	if(m_gasTxt)
		TextureDestroy(&m_gasTxt);

	if(m_suckSnd)
		BASS_ChannelStop(m_suckSnd);
}

void CorrupTa::GasActivate(bool bYes)
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

void CorrupTa::FFieldActivate(bool bYes)
{
	m_bFfieldOn = bYes;

	if(bYes)
	{
		if(OBJIsDeactivate(m_ffieldOBJ))
		{
			float s[eMaxPt] = {0,0,0};
			//OBJScale(m_ffieldOBJ, s, false);
			OBJSetAlpha(m_ffieldOBJ, 0);

			m_teleScale = 0;
			TimeReset(&m_teleGrowDelay);

			OBJActivate(m_ffieldOBJ, true);
			OBJAddChild(GetOBJ(), m_ffieldOBJ, -1, true);

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
			tFX.g = 255; tFX.r = tFX.b = 127;
			tFX.scale = BoundGetRadius()/2;
			memcpy(tFX.center, (float*)GetLoc(), sizeof(tFX.center));
			tFX.radius = BoundGetRadius()*12;
			tFX.maxParticle = 40;
			tFX.delay = 2000;

			PARFXCreate(ePARFX_EXPLODE, &tFX, -1, 0, -1, 0);

			//play shatter sound
			CREPlaySound(35);
		}

		TimeReset(&m_teleActivateDelay);
	}
}

void CorrupTa::FFieldUpdate()
{
	if(!FFieldIsActive()
		&& m_bGenerateFField)
	{
		if(TimeElapse(&m_teleActivateDelay))
		{
			FFieldActivate(true);
		}
	}
	else
	{
		//OBJRotY(m_ffieldOBJ, m_ffieldRotSpd.MoveUpdate(g_timeElapse));

		if(m_teleScale < 1)
		{
			m_teleScale = TimeGetTime(&m_teleGrowDelay)/TimeGetDelay(&m_teleGrowDelay);

			if(m_teleScale > 1) m_teleScale = 1;

			//float s[eMaxPt] = {m_teleScale,m_teleScale,m_teleScale};
			//OBJScale(m_ffieldOBJ, s, false);
			OBJSetAlpha(m_ffieldOBJ, m_teleScale);
		}
	}
}

bool CorrupTa::FFieldIsActive()
{
	if(OBJIsDeactivate(m_ffieldOBJ))
		return false;

	return true;
}

void CorrupTa::TeleportME()
{
	//determine what teleport area to use
	Creature *pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

	int avoidInd = m_teleInd;

	if(pCre)
	{
		//top
		if(pCre->GetLoc().y > m_teleEquatorY)
		{
			while(m_teleInd == avoidInd)
				m_teleInd = rand()%m_teleTop.size();

			Teleport(m_teleTop[m_teleInd]);
		}
		//bottom
		else
		{
			while(m_teleInd == avoidInd)
				m_teleInd = rand()%m_teleBottom.size();

			Teleport(m_teleBottom[m_teleInd]);
		}
	}
}

void CorrupTa::GenerateBlast(Creature *pCre, double delay)
{
	fxExpand_init tFX={0};
	tFX.expandTxt = m_blastFXTxt;
	tFX.r = 0; tFX.g = 255; tFX.b = 0;

	D3DXVECTOR3 btm(pCre->GetLoc());
	btm.y += pCre->BoundGetMin().y + 0.05f;
	
	memcpy(tFX.center, (float*)btm, sizeof(tFX.center));

	tFX.normal[eY] = 1;

	tFX.scaleEnd = pCre->BoundGetRadius()*8;

	tFX.delay = delay/4;

	tFX.bRepeat = true;

	PARFXCreate(ePARFX_EXPAND, &tFX, delay, 0, -1, 0);
}

void CorrupTa::ChangeBehavior(int state)
{
	m_actionState = state;

	BehaviorUpdate(true);
}

void CorrupTa::BehaviorUpdate(bool bBegin)
{
	switch(m_actionState)
	{
	case CORRUPTA_STATE_IDLE_1:
		if(bBegin)
		{
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_IDLE1);
		}
		break;

	case CORRUPTA_STATE_IDLE_2:
		if(OBJGetCurState(GetOBJ()) == CORRUPTA_ANIM_DEATH)
			break;
		else if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//set to idle animation
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_IDLE3);

			TimeReset(&m_idleDelay);

			m_bGenerateFField = true;
		}
		else if(TimeElapse(&m_idleDelay))
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
			ChangeBehavior(CORRUPTA_STATE_PATTERN_ATTACK);
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
			}
		}
		break;

	case CORRUPTA_STATE_METEOR_1:
		if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//set to casting animation
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_IDLE2);

			TimeReset(&m_meteorDelay);
		}
		else if(TimeElapse(&m_meteorDelay))
		{
			Creature *pCre;
		
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				//throw a downward fireball
				SetDir(D3DXVECTOR3(0,-1,0));

				//set location near the cieling
				gfxTrace trace;
				D3DXVECTOR3 pt2(pCre->GetLoc());
				pt2.y += 100000.0f;

				QBSPCollision(g_world->GetMap(), 0, 0,
					  (float*)pCre->GetLoc(), (float*)pt2, &trace,
					  1, -1);

				//also make sure that the ceilling has no texture
				if(!QBSPGetTexture(g_world->GetMap(), trace.txtInd))
				{
					D3DXVECTOR3 spitLoc(pCre->GetLoc());
					spitLoc.y = pCre->GetLoc().y + (pt2.y - pCre->GetLoc().y)*trace.t;
					spitLoc.y -= m_meteorRadius;

					Spit2 *pSpit = new Spit2(GetID(), spitLoc, m_meteorSpd, m_ballMdl, 
								m_meteorFXTxt, m_meteorRadius, 127, 255, 127, false, 0, 36);

					GenerateBlast(pCre, TimeGetDelay(&m_meteorDelay));

					//play fire strike near tata
					pCre->CREPlaySound(45);
				}
			}
		}
		break;

	case CORRUPTA_STATE_METEOR_2:
		if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//set to casting animation
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_UPTHROW);

			TimeReset(&m_meteor2Delay);

			PARFXActivate(m_fxCharge, true);

			m_meteor2Cur = 0;
		}
		else
		{
			int state = OBJGetCurState(GetOBJ());

			switch(state)
			{
			case CORRUPTA_ANIM_UPTHROW:
				if(OBJIsStateEnd(GetOBJ()))
				{
					OBJSetState(GetOBJ(), CORRUPTA_ANIM_UPTHROWIDLE);
					PARFXActivate(m_fxCharge, false);
				}
				break;

			case CORRUPTA_ANIM_UPTHROWIDLE:
				if(m_meteor2Cur < m_meteor2Max)
				{
					//throw a meteor
					if(TimeElapse(&m_meteor2Delay))
					{
						Creature *pCre;
		
						pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

						if(pCre)
						{
							//throw a downward fireball
							SetDir(D3DXVECTOR3(0,-1,0));

							//set location near the cieling
							gfxTrace trace;
							D3DXVECTOR3 pt2(pCre->GetLoc());
							pt2.y += 100000.0f;

							QBSPCollision(g_world->GetMap(), 0, 0,
								  (float*)pCre->GetLoc(), (float*)pt2, &trace,
								  1, -1);

							//also make sure that the ceilling has no texture
							if(!QBSPGetTexture(g_world->GetMap(), trace.txtInd))
							{
								D3DXVECTOR3 spitLoc(pCre->GetLoc());
								spitLoc.y = pCre->GetLoc().y + (pt2.y - pCre->GetLoc().y)*trace.t;
								spitLoc.y -= m_meteor2Radius;

								Spit2 *pSpit = new Spit2(GetID(), spitLoc, m_meteor2Spd, m_ballMdl, 
											m_meteorFXTxt, m_meteor2Radius, 127, 255, 127, false, 0, 36);

								GenerateBlast(pCre, TimeGetDelay(&m_meteorDelay));

								//play fire strike near tata
								pCre->CREPlaySound(45);
							}

							m_meteor2Cur++;
						}
					}
				}
				else //done, return to idle
					ChangeBehavior(CORRUPTA_STATE_IDLE_2);
				break;
			}
		}
		break;

	case CORRUPTA_STATE_TATAMAHATTA:
		if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//set to casting animation
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_UPTHROWIDLE);

			TimeReset(&m_mahattaDelay);
		}
		else
		{
			double t = TimeGetTime(&m_mahattaDelay)/TimeGetDelay(&m_mahattaDelay);

			if(t > 1) t = 1;

			float scale = 1.0f + t*(m_mahattaScale - 1.0f);

			float s[eMaxPt] = {scale,scale,scale};
			
			OBJScale(GetOBJ(), s, false);

			if(t == 1)
			{
				BoundSetMin(BoundGetMin()*scale);
				BoundSetMax(BoundGetMax()*scale);
				BoundSetRadius(BoundGetRadius()*scale);
				BoundGeneratePlanes();

				float dbMin[eMaxPt]; memcpy(dbMin, (float*)(BoundGetMin()*scale*2), sizeof(dbMin));
				float dbMax[eMaxPt]; memcpy(dbMax, (float*)(BoundGetMax()*scale*2), sizeof(dbMax));
				OBJSetDrawBound(GetOBJ(), dbMin, dbMax);

				//set to idle
				FFieldActivate(true);
				ChangeBehavior(CORRUPTA_STATE_IDLE_2);
			}
		}
		break;

	case CORRUPTA_STATE_TELEPORT:
		if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			//set to idle animation
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_IDLE3);
		}
		else
		{
			TeleportME();
			
			//set to pattern attack
			ChangeBehavior(CORRUPTA_STATE_PATTERN_ATTACK);
		}
		break;

	case CORRUPTA_STATE_PATTERN_ATTACK:
		if(bBegin)
		{
			//in case the object was paused
			OBJPauseOnStateEnd(GetOBJ(), false);

			TimeReset(&m_patternDelay);

			//check to see first if pattern is done
			if(m_patternCur >= m_patternAtk[m_patternInd].size())
			{
				//set to meteor attack!
				ChangeBehavior(CORRUPTA_STATE_METEOR_2);
			}
		}
		else if(TimeElapse(&m_patternDelay))
		{
			//change attack depending on pattern
			switch(m_patternAtk[m_patternInd][m_patternCur])
			{
			case 0: //Fireball attack!
				ChangeBehavior(CORRUPTA_STATE_FIREBALL);
				break;

			case 1: //Sucker Ball
				ChangeBehavior(CORRUPTA_STATE_SUCKERBALL);
				break;
			}

			m_patternCur++;
		}
		break;

	case CORRUPTA_STATE_FIREBALL:
		if(bBegin)
		{
			//set to charging state
			OBJPauseOnStateEnd(GetOBJ(), true);
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_CHARGE);
			
			PARFXActivate(m_fxCharge, true);

			TimeReset(&m_ballChargeDelay);

			m_ballCur = 0;

			//play charge sound
			CREPlaySound(40);
		}
		else
		{
			int state = OBJGetCurState(GetOBJ());

			//set direction towards current Ta-Ta
			Creature *pCre;
		
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				D3DXVECTOR3 vec(pCre->GetLoc()-GetLoc());
				vec.y = 0;
				D3DXVec3Normalize(&vec, &vec);
				
				SetDir(vec);

				switch(state)
				{
				case CORRUPTA_ANIM_CHARGE:
					if(TimeElapse(&m_ballChargeDelay))
					{
						OBJPause(GetOBJ(), false);
						OBJSetState(GetOBJ(), CORRUPTA_ANIM_FIREBALL);
						PARFXActivate(m_fxCharge, false);

						TimeReset(&m_ballDelay);
					}
					break;

				case CORRUPTA_ANIM_FIREBALL:
					if(OBJIsStateEnd(GetOBJ()))
					{
						if(TimeElapse(&m_ballDelay))
						{
							D3DXVECTOR3 dir(GetDir());
							D3DXMATRIX theMtx; D3DXMatrixRotationY(&theMtx, TataRand(m_ballAngleMin, m_ballAngleMax));
							D3DXVec3TransformNormal(&dir, &dir, &theMtx);
							SetDir(dir);

							//throw a fireball
							D3DXVECTOR3 spitLoc;
							OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "r_palm"), (float*)spitLoc);

							new Spit2(GetID(), spitLoc, m_ballSpd, m_ballMdl, 
											m_ballFXTxt, m_ballRadius, 
											0, 200, 0, true, m_ballNumBounce, 36);

							m_ballCur++;

							//play fball launch
							CREPlaySound(41);
						}
					}
					break;
				}
			}

			//done, let's teleport
			if(m_ballCur >= m_ballMax)
				ChangeBehavior(CORRUPTA_STATE_TELEPORT);
		}
		break;

	case CORRUPTA_STATE_SUCKERBALL:
		if(bBegin)
		{
			//set to charging state
			OBJPauseOnStateEnd(GetOBJ(), true);
			OBJSetState(GetOBJ(), CORRUPTA_ANIM_CHARGE);
			
			PARFXActivate(m_fxCharge, true);

			TimeReset(&m_pullDelay);
			
			GasActivate(true);

			//play suck-in sound
			m_suckSnd = CREPlaySound(39);
		}
		else
		{
			//set direction towards current Ta-Ta
			Creature *pCre;
		
			pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
			{
				D3DXVECTOR3 vec(pCre->GetLoc()-GetLoc());
				D3DXVec3Normalize(&vec, &vec);
				SetDir(vec);

				if(TimeElapse(&m_pullDelay))
				{
					OBJPause(GetOBJ(), false);
					OBJSetState(GetOBJ(), CORRUPTA_ANIM_FIREBALL);
					PARFXActivate(m_fxCharge, false);

					GasActivate(false);
				}
				else
				{
					int state = OBJGetCurState(GetOBJ());

					//throw that damn fireball already!
					if(state == CORRUPTA_ANIM_FIREBALL
						&& OBJIsStateEnd(GetOBJ()))
					{
						//throw a fireball
						D3DXVECTOR3 spitLoc;
						OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "r_palm"), (float*)spitLoc);

						new Spit2(GetID(), spitLoc, m_ballSpd, m_ballMdl, 
										m_ballFXTxt, m_ballRadius, 
										0, 200, 0, false, 0, 36);

						if(m_suckSnd)
						{ BASS_ChannelStop(m_suckSnd); m_suckSnd = 0; }

						//play fball launch
						CREPlaySound(41);

						ChangeBehavior(CORRUPTA_STATE_TELEPORT);
					}
					else
						pCre->SetVel(pCre->GetVel() + (-GetDir())*m_pullMov.MoveUpdate(g_timeElapse));
				}
			}
		}
		break;
	}
}

int CorrupTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			char buff[MAXCHARBUFF]={0};
			hCFG cfg = (hCFG)wParam;

			///////////////////////////////////////////////////////////////
			// Fireball

			//Load the model for fireball
			if(CfgGetItemStr(cfg, "fireball", "model", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_ballMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "fireball", "trail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_ballFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load attack stuff
			m_ballSpd = CfgGetItemFloat(cfg, "fireball", "spd");

			m_ballRadius = CfgGetItemFloat(cfg, "fireball", "radius");

			m_ballMax = CfgGetItemInt(cfg, "fireball", "max");

			m_ballAngleMin = D3DXToRadian(CfgGetItemFloat(cfg, "fireball", "angleMin"));
			m_ballAngleMax = D3DXToRadian(CfgGetItemFloat(cfg, "fireball", "angleMax"));

			m_ballNumBounce = CfgGetItemInt(cfg, "fireball", "bounceMax");

			TimeInit(&m_ballDelay, CfgGetItemFloat(cfg, "fireball", "delay"));
			TimeInit(&m_ballChargeDelay, CfgGetItemFloat(cfg, "fireball", "chargeDelay"));
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Sucker Ball
			m_pullMov.MoveSetSpeed(CfgGetItemFloat(cfg, "suckerball", "spd"));

			TimeInit(&m_pullDelay, CfgGetItemFloat(cfg, "suckerball", "delay"));
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Meteor 1 & 2
			
			//Load the texture for meteor trail
			if(CfgGetItemStr(cfg, "meteor", "trail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_meteorFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			m_meteorSpd = CfgGetItemFloat(cfg, "meteor", "spd1");
			m_meteorRadius = CfgGetItemFloat(cfg, "meteor", "radius1");

			TimeInit(&m_meteorDelay, CfgGetItemFloat(cfg, "meteor", "delay1"));

			m_meteor2Spd = CfgGetItemFloat(cfg, "meteor", "spd2");
			m_meteor2Radius = CfgGetItemFloat(cfg, "meteor", "radius2");

			TimeInit(&m_meteor2Delay, CfgGetItemFloat(cfg, "meteor", "delay2"));

			m_meteor2Max = CfgGetItemInt(cfg, "meteor", "max2");
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

				if(ffieldMDL)
				{
					char mdlANMPath[MAXCHARBUFF];

					strcpy(mdlANMPath, mdlPath.c_str());
					strcpy(GetExtension(mdlANMPath), ".anm");

					m_ffieldOBJ = OBJCreate(0, ffieldMDL,0,0,0,eOBJ_ANIMATE, mdlANMPath);

					if(m_ffieldOBJ)
					{
						OBJActivate(m_ffieldOBJ, false);
					}
				}				
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

			TimeInit(&m_teleActivateDelay, CfgGetItemFloat(cfg, "forcefield", "activateDelay"));
			TimeInit(&m_teleGrowDelay, CfgGetItemFloat(cfg, "forcefield", "growDelay"));
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

			if(CfgGetItemStr(cfg, "gas", "bound", buff))
				sscanf(buff, "%f,%f,%f", &m_gasBound.x, &m_gasBound.y, &m_gasBound.z);
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Teleport stuff
			D3DXVECTOR3 tgt;

			if(CfgGetItemStr(cfg, "teleport", "equatorTarget", buff))
			{
				if(g_world->TargetGet(buff, tgt))
					m_teleEquatorY = tgt.y;
			}

			// get the top target stuff
			int numTeleTop = CfgGetItemInt(cfg, "teleport", "topNum");
			char tgtName[MAXCHARBUFF];

			if(numTeleTop > 0)
			{
				m_teleTop.resize(numTeleTop);

				for(int i = 0; i < m_teleTop.size(); i++)
				{
					sprintf(buff, "teleTop_%d", i);

					if(CfgGetItemStr(cfg, "teleport", buff, tgtName))
						g_world->TargetGet(tgtName, m_teleTop[i]);
				}
			}

			// get the bottom target stuff
			int numTeleBtm = CfgGetItemInt(cfg, "teleport", "bottomNum");

			if(numTeleBtm > 0)
			{
				m_teleBottom.resize(numTeleBtm);

				for(int i = 0; i < m_teleBottom.size(); i++)
				{
					sprintf(buff, "teleBtm_%d", i);

					if(CfgGetItemStr(cfg, "teleport", buff, tgtName))
						g_world->TargetGet(tgtName, m_teleBottom[i]);
				}
			}

			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Misc. stuff

			//load blast FX texture
			if(CfgGetItemStr(cfg, "special", "blastFXTxt", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_blastFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			TimeInit(&m_idleDelay, CfgGetItemFloat(cfg, "special", "idleDelay"));
			TimeInit(&m_patternDelay, CfgGetItemFloat(cfg, "special", "patternDelay"));

			//set pattern attack size to maximum health points
			m_patternAtk.resize((int)GetMaxHit());

			//get patterns
			int numFball, numSucker, total;

			for(int i = 0; i < m_patternAtk.size(); i++)
			{
				sprintf(buff, "pattern_%d", i);

				numFball  = CfgGetItemInt(cfg, buff, "fireball");
				numSucker = CfgGetItemInt(cfg, buff, "suckerball");

				total = numFball + numSucker;

				//fill in the patterns
				if(total > 0)
				{
					m_patternAtk[i].resize(total);

					for(int s = 0; s < numFball; s++)
						m_patternAtk[i][s] = 0;

					for(int m = numFball; m < total; m++)
						m_patternAtk[i][m] = 1;
				}
			}
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Ta-Ta Mahatta
			
			m_mahattaScale = CfgGetItemFloat(cfg, "tatamahatta", "scale");
			TimeInit(&m_mahattaDelay, CfgGetItemFloat(cfg, "tatamahatta", "delay"));
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			// Charge FX
			fxGather_init m_fxChargeDat;
			
			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "chargeFX", "texture", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_fxChargeDat.explodeTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load color
			if(CfgGetItemStr(cfg, "chargeFX", "attackClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_fxChargeDat.r=r; m_fxChargeDat.g=g; m_fxChargeDat.b=b;
			}

			m_fxChargeDat.scale = CfgGetItemFloat(cfg, "chargeFX", "scale");
			m_fxChargeDat.radius = CfgGetItemFloat(cfg, "chargeFX", "radius");
			m_fxChargeDat.maxParticle = CfgGetItemInt(cfg, "chargeFX", "max");

			m_fxChargeDat.delayGather = CfgGetItemFloat(cfg, "chargeFX", "delayGather");
			m_fxChargeDat.delayFade = CfgGetItemFloat(cfg, "chargeFX", "delayFade");
			m_fxChargeDat.delaySpawn = CfgGetItemFloat(cfg, "chargeFX", "delaySpawn");

			//create the FX
			int chargeJointInd = OBJJointGetIndex(GetOBJ(), "r_palm");
			m_fxCharge = PARFXCreate(ePARFX_GATHER, &m_fxChargeDat, -1, 
						  GetOBJ(), chargeJointInd, 0);

			//deactivate for now
			PARFXActivate(m_fxCharge, false);
			///////////////////////////////////////////////////////////////

			//set to idle
			ChangeBehavior(CORRUPTA_STATE_IDLE_1);
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			FFieldUpdate();

			BehaviorUpdate(false);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
			{
				EntityCommon *pOwner = (EntityCommon *)IDPageQuery(((Projectile*)pEntity)->GetOwner());

				if(pOwner)
				{
					
					//check to see if it is a spit attack
					//and is from Ta-Ta
					if(pOwner->GetEntityType() == ENTITY_TYPE_TATA
						&& pEntity->GetSubType() == PROJ_SPIT)
					{
						if(!FFieldIsActive())
						{
							//get hit
							Hit();

							GasActivate(false);

							FFieldActivate(true);

							//set back to idle
							ChangeBehavior(CORRUPTA_STATE_IDLE_2);
						}
						else //bounce back
						{
							//change the projectile's owner 
							((Projectile*)pEntity)->SetOwner(GetID());

							float nd = D3DXVec3Dot(&GetDir(), &pEntity->GetDir());

							D3DXVECTOR3 refl;

							refl = pEntity->GetDir() - (2*nd*GetDir());

							pEntity->SetDir(refl);
						}
					}
				}		
			}
			else if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
				&& pEntity->GetSubType() == OBJECT_LIGHTNING
				&& !pEntity->CheckFlag(OBJ_FLAG_INACTIVE)
				&& FFieldIsActive())
			{
				///lightning strike, remove shield
				FFieldActivate(false);

				//teleport to a safe place
				TeleportME();
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity if state is 'Ta-Ta Mahatta'
		if(m_actionState == CORRUPTA_STATE_TATAMAHATTA)
			return 0;
		break;

	case ENTITYMSG_CHANGEBEHAVIOR:
		ChangeBehavior(wParam);
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 44;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}

int CorrupTa::GetDeathState()
{
	return CORRUPTA_ANIM_DEATH;
}

int CorrupTa::GetOuchState()
{
	return CORRUPTA_ANIM_OUCH;
}