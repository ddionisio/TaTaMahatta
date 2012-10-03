#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

/*
class FungaSmug : public EnemyCommon {
public:
	FungaSmug();
	~FungaSmug();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void MoveAnim(eMoveType type);

private:
	float			m_attackRadiusSQ;		//attack radius^2 (any Ta-Ta who gets within this radius will be attacked)
	D3DXVECTOR3		m_attackBound;
	Gasus		   *m_gas;					//gas attack

	hTXT			m_gasTxt;				//gas FX texture
	float			m_gasSize;				//gas particle size
	BYTE			m_r,m_g, m_b;			//gas clr.
};

Gasus(const Id & owner, const D3DXVECTOR3 & loc,
		const D3DXVECTOR3 & bound, const D3DXVECTOR3 & gasDir, 
		hTXT gasTxt, float gasSize, BYTE r, BYTE g, BYTE b, float duration);
*/

//animation
typedef enum {
	FUNGASMUG_ANIM_IDLE,
	FUNGASMUG_ANIM_WALK,
	FUNGASMUG_ANIM_OUCH,
	FUNGASMUG_ANIM_DEATH,
	FUNGASMUG_ANIM_ATTACK
} eFungaSmugAnim;

FungaSmug::FungaSmug() : EnemyCommon(ENEMY_FUNGASMUG),
m_attackRadiusSQ(0), m_attackBound(0,0,0), m_gasTxt(0),
m_gasSize(0), m_r(0), m_g(0), m_b(0)
{
	m_gasID.ID = 0; m_gasID.counter = -1;
}

FungaSmug::~FungaSmug()
{
	Gasus *pGas = (Gasus*)IDPageQuery(m_gasID);
	if(pGas)
		delete pGas;

	if(m_gasTxt)
		TextureDestroy(&m_gasTxt);
}

int FungaSmug::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_attackRadiusSQ = CfgGetItemFloat(cfg, "special", "attackRange");
			m_attackRadiusSQ *= m_attackRadiusSQ;

			//get the attack bound m_attackBound
			m_attackBound.x = CfgGetItemFloat(cfg, "special", "attackBoundX");
			m_attackBound.y = CfgGetItemFloat(cfg, "special", "attackBoundY");
			m_attackBound.z = CfgGetItemFloat(cfg, "special", "attackBoundZ");

			//get gas FX size
			m_gasSize = CfgGetItemFloat(cfg, "special", "gasFXSize");

			char buff[MAXCHARBUFF]={0};

			//get gas texture
			if(CfgGetItemStr(cfg, "special", "gasFXTxt", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_gasTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//get gas color
			if(CfgGetItemStr(cfg, "special", "gasFXClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_r = r; m_g = g; m_b = b;
			}

			//FX thing
			fxGas_init tFX;

			if(CfgGetItemStr(cfg, "FX", "txt", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				tFX.gasTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			if(CfgGetItemStr(cfg, "FX", "clr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				tFX.r = r; tFX.g = g; tFX.b = b;
			}

			if(CfgGetItemStr(cfg, "FX", "dir", buff))
				sscanf(buff, "%f,%f,%f", &tFX.dir[eX],&tFX.dir[eY],&tFX.dir[eZ]);

			tFX.spd = CfgGetItemFloat(cfg, "FX", "spd");

			tFX.spawnDelay = CfgGetItemFloat(cfg, "FX", "spawnDelay");

			memcpy(tFX.min, (float*)BoundGetMin(), sizeof(float)*eMaxPt);
			memcpy(tFX.max, (float*)BoundGetMax(), sizeof(float)*eMaxPt);

			tFX.maxParticle = CfgGetItemInt(cfg, "FX", "max");
			
			tFX.scaleMax = CfgGetItemFloat(cfg, "FX", "scale");

			tFX.scaleDelayMin = CfgGetItemFloat(cfg, "FX", "scaleDelayMin");
			tFX.scaleDelayMax = CfgGetItemFloat(cfg, "FX", "scaleDelayMax");

			tFX.alphaDelayMin = CfgGetItemFloat(cfg, "FX", "alphaDelayMin");
			tFX.alphaDelayMax = CfgGetItemFloat(cfg, "FX", "alphaDelayMax");

			tFX.bRepeat = true;

			PARFXCreate(ePARFX_GAS, &tFX, -1, GetOBJ(), -1, 0);
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			D3DXVECTOR3 vec, vecMin;
			float vecLenSq, vecLenMin = m_attackRadiusSQ;
			Creature *pCre, *pCreMin=0;
			
			for(int i = 0; i < MAXTATAWORLD; i++)
			{
				pCre = (Creature *)IDPageQuery(g_world->TataGet(i));

				if(pCre)
				{
					vec = pCre->GetLoc() - GetLoc();
					vecLenSq = D3DXVec3LengthSq(&vec);

					//uh oh, better run!
					//check to be sure that the player is 
					//within height range
					if(vecLenSq <= vecLenMin
						&& pCre->GetLoc().y >= (GetLoc().y + BoundGetMin().y + pCre->BoundGetMin().y)
						&& pCre->GetLoc().y <= (GetLoc().y + BoundGetMax().y + pCre->BoundGetMax().y))
					{
						vecMin    = vec;
						vecLenMin = vecLenSq;
						pCreMin   = pCre;
					}
				}
			}

			//are we currently attacking or hurt?
			if(OBJGetCurState(GetOBJ()) == FUNGASMUG_ANIM_ATTACK
				|| OBJGetCurState(GetOBJ()) == FUNGASMUG_ANIM_OUCH)
			{
				//don't move
				D3DXVECTOR3 vel =GetVel();
				vel.x = vel.z = 0;
				SetVel(vel);

				//is there no Ta-Ta near?
				if(!pCreMin)
				{
					//go back to normal
					OBJSetState(GetOBJ(), FUNGASMUG_ANIM_WALK);

					//destroy gas projectile
					Gasus *pGas = (Gasus*)IDPageQuery(m_gasID);
					if(pGas)
						delete pGas;
				}
			}
			//did we find anyone near?
			else if(pCreMin)
			{
				//set to attack
				OBJSetState(GetOBJ(), FUNGASMUG_ANIM_ATTACK);

				//create gas projectile
				Gasus *pGas = new Gasus(GetID(), GetLoc(), m_attackBound, 
					D3DXVECTOR3(0,1,0), m_gasTxt, m_gasSize, 
					m_r, m_g, m_b, -1);

				//play fart sound
				CREPlaySound(25);

				if(pGas)
					m_gasID = pGas->GetID();
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}

int FungaSmug::GetDeathState()
{
	return FUNGASMUG_ANIM_DEATH;
}

int FungaSmug::GetOuchState()
{
	return FUNGASMUG_ANIM_OUCH;
}

void FungaSmug::MoveAnim(eMoveType type)
{
	int OBJState = OBJGetCurState(GetOBJ());

	if(OBJState == FUNGASMUG_ANIM_IDLE
		|| OBJState == FUNGASMUG_ANIM_WALK)
	{
		switch(type)
		{
		case MOVE_FALL:
			OBJSetState(GetOBJ(), FUNGASMUG_ANIM_WALK);
			break;

		case MOVE_NONE:
			OBJSetState(GetOBJ(), FUNGASMUG_ANIM_IDLE);
			break;
		
		case MOVE_WALK:
			OBJSetState(GetOBJ(), FUNGASMUG_ANIM_WALK);
			break;
		}
	}
	else if(OBJState == FUNGASMUG_ANIM_OUCH && OBJIsStateEnd(GetOBJ()))
		OBJSetState(GetOBJ(), FUNGASMUG_ANIM_IDLE);
}