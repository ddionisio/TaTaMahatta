#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature.h"

/////////////////////////////////////////////////////////////////////////////
// Status Teleport
/////////////////////////////////////////////////////////////////////////////
#define TELE_DELAY		250

StatusTeleport::StatusTeleport(const Id & owner, const char *targetName) 
: CreatureStatus(CRESTATUS_TELEPORT, owner), m_bFading(true), m_bGO(false)
{
	if(g_world)
	{
		g_world->TargetGet(targetName, m_dest);

		TimeInit(&m_delay, TELE_DELAY);
	}

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
		pCre->SetFlag(CRE_FLAG_INVUL, true);
}

StatusTeleport::StatusTeleport(const Id & owner, const D3DXVECTOR3 & loc)
: CreatureStatus(CRESTATUS_TELEPORT, owner), m_bFading(true), m_dest(loc)
{
	TimeInit(&m_delay, TELE_DELAY);

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
	{
		pCre->SetFlag(CRE_FLAG_INVUL, true);

		OBJSetToLast(pCre->GetOBJ());
	}
}

StatusTeleport::~StatusTeleport()
{
	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
		pCre->SetFlag(CRE_FLAG_INVUL, false);
}

void StatusTeleport::GO()
{
	m_bGO = true;

	//play teleport sound
	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
		pCre->CREPlaySound(37);
}

RETCODE StatusTeleport::Update()
{
	RETCODE ret = RETCODE_SUCCESS;

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(m_bGO)
	{
		if(pCre)
		{
			hOBJ obj = pCre->GetOBJ(); assert(obj);

			double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

			if(t >= 1)
			{
				t = 1;
				TimeReset(&m_delay);
			}

			OBJSetAlpha(obj, m_bFading ? 1-t : t);

			//are we done?
			if(t == 1 && !m_bFading)
				ret = RETCODE_STATUS_DONE;
			else if(t == 1 && m_bFading)
			{
				m_bFading = false;

				//set to new location
				pCre->SetLoc(m_dest);

				D3DXVECTOR3 zeroVel(0,0,0);
				pCre->SetVel(zeroVel);

				pCre->CREPlaySound(38);
			}
		}
		else
			ret = RETCODE_STATUS_DONE;
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Status Invulnerable
/////////////////////////////////////////////////////////////////////////////
#define INVUL_DELAY		500	//fade delay

StatusInvulnerable::StatusInvulnerable(const Id & owner, double delay)
: CreatureStatus(CRESTATUS_INVULNERABLE, owner), m_bFading(true)
{
	TimeInit(&m_fadeDelay, INVUL_DELAY);
	TimeInit(&m_invulDelay, delay);

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
	{
		pCre->SetFlag(CRE_FLAG_INVUL, true);

		OBJSetToLast(pCre->GetOBJ());
	}
}

StatusInvulnerable::~StatusInvulnerable()
{
	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
		pCre->SetFlag(CRE_FLAG_INVUL, false);
}

RETCODE StatusInvulnerable::Update()
{
	RETCODE ret = RETCODE_SUCCESS;

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
	{
		hOBJ obj = pCre->GetOBJ(); assert(obj);

		double t = TimeGetTime(&m_fadeDelay)/TimeGetDelay(&m_fadeDelay);

		if(t >= 1)
		{
			t = 1;
			TimeReset(&m_fadeDelay);

			m_bFading = !m_bFading;
		}

		OBJSetAlpha(obj, m_bFading ? 1-t : t);

		//invulnerable over?
		if(TimeElapse(&m_invulDelay))
		{
			OBJSetAlpha(obj, 1);
			
			ret = RETCODE_STATUS_DONE;
		}
	}
	else
		ret = RETCODE_STATUS_DONE;

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
//Stun
/////////////////////////////////////////////////////////////////////////////
StatusStun::StatusStun(const Id & owner, double delay)
: CreatureStatus(CRESTATUS_STUN, owner), m_parFX(0)
{
	TimeInit(&m_stunDelay, delay);

	string fxTxtFile = GAMEFOLDER;
	fxTxtFile += "\\";
	fxTxtFile += "Textures\\Particles\\star.tga";

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
	{
		fxGlow_init fx;

		fx.glowTxt = TextureCreate(0, fxTxtFile.c_str(), false, 0);

		if(fx.glowTxt)
		{
			fx.r = fx.g = fx.b = 255;

			fx.scaleStart = 0; fx.scaleEnd = pCre->BoundGetRadius()*2;

			fx.delay = delay*0.125;

			fx.bRepeat = true;

			m_parFX = PARFXCreate(ePARFX_GLOW, &fx, -1, pCre->GetOBJ(), -1, 0);
		}
	}
}

StatusStun::~StatusStun()
{
	if(m_parFX)
		PARFXDestroy(&m_parFX);
}

RETCODE StatusStun::Update()
{
	RETCODE ret = RETCODE_STATUS_NOUPDATE;

	Creature *pCre = (Creature *)IDPageQuery(m_owner);

	if(pCre)
	{
		OBJSetState(pCre->GetOBJ(), pCre->GetOuchState());

		//stun over?
		if(TimeElapse(&m_stunDelay))
			ret = RETCODE_STATUS_DONE;
	}
	else
		ret = RETCODE_STATUS_DONE;

	return ret;
}