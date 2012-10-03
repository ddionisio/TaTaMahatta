#include "tata_main.h"

#include "tata_globals.h"

#include "tata_item.h"

#define ITM_BOYANT_SPD		15
#define ITM_ROT_SPD			6

Item::Item(int subType) 
: EntityCommon(this, ENTITY_TYPE_ITEM, subType), 
m_boyantDir(1), m_minBoyant(0), m_maxBoyant(0)
{ 
	SetFlag(ENTITY_FLAG_NONSOLID, true); 
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_PULLABLE, true);

	m_boyantSpd.MoveSetSpeed(ITM_BOYANT_SPD);
	m_rotSpd.MoveSetSpeed(ITM_ROT_SPD);
}

RETCODE Item::Load(const char *dataTypeName)
{
	hMDL model=0;

	char filename[MAXCHARBUFF];

	SetName(dataTypeName);

	const char *datafilename = TataDataGetFilePath(dataTypeName);

	hCFG cfg=0;
	
	if(datafilename)
		cfg = CfgFileLoad(datafilename);

	if(!cfg)
	{ ErrorMsg("Creature::Load", "Data File Not Found", false); return RETCODE_FAILURE; }

	//////////////////////////////////////
	//load model
	if(CfgGetItemStr(cfg, "main", "model", filename))
	{
		string mdlFile = GAMEFOLDER;
		mdlFile += "\\";
		mdlFile += filename;

		model = MDLCreate(0, mdlFile.c_str());

		if(!model)
		{ ErrorMsg("Creature::LoadModel", "Unable to load model", true); return 0; }
	}

	//create the object
	SetOBJ(OBJCreate(0, model, 0,0,0, eOBJ_STILL, 0));

	//////////////////////////////////////
	//set bounds
	gfxBound bound;
	OBJGetBounds(GetOBJ(), &bound, true);

	BoundSetMin(D3DXVECTOR3(bound.min));
	BoundSetMax(D3DXVECTOR3(bound.max));
	BoundSetRadius(bound.radius);
	BoundGeneratePlanes();

	//////////////////////////////////////
	//load flag
	SetFlag(CfgGetItemInt(cfg, "main", "flag"), true);

	//load other stuff
	Callback(ENTITYMSG_CFGLOAD, (WPARAM)cfg, 0);

	CfgFileDestroy(&cfg);

	return RETCODE_SUCCESS;
}

RETCODE Item::Update()
{
	/////////////////////////////////////////////////
	//Rotate the Item
	D3DXVECTOR3 dir = GetDir();

	D3DXMATRIX theMtx;
	D3DXMatrixRotationY(&theMtx, m_rotSpd.MoveUpdate(g_timeElapse));

	D3DXVec3TransformNormal(&dir, &dir, &theMtx);

	SetDir(dir);

	/////////////////////////////////////////////////
	//boyant thing-a-magig
	float y = GetLoc().y;
	if(y < m_minBoyant)
		m_boyantDir = 1;
	else if(y > m_maxBoyant)
		m_boyantDir = -1;

	SetVel(D3DXVECTOR3(0,m_boyantDir*m_boyantSpd.MoveUpdate(g_timeElapse),0));

	return RETCODE_SUCCESS;
}

#define BOYANT_LENGTH 4
void Item::SetBoyantMinMax()
{
	m_minBoyant = GetLoc().y - BOYANT_LENGTH;
	m_maxBoyant = GetLoc().y + BOYANT_LENGTH;
}

/********************************************************************
*																	*
*							Resource								*
*																	*
********************************************************************/
#define RC_EXPLODE_DELAY	1000

Resource::Resource() : Item(ITM_RESOURCE), m_particleTxt(0)
{ 
	memset(m_rcVal, 0, sizeof(m_rcVal)); 
}

Resource::~Resource()
{
	if(m_particleTxt)
		TextureDestroy(&m_particleTxt);
}

int Resource::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			for(int i = 0; i < RC_MAX; i++)
				m_rcVal[i] = CfgGetItemInt(cfg, "special", g_rcStr[i]);

			//load texture for particle
			char buff[MAXCHARBUFF]={0};

			if(CfgGetItemStr(cfg, "special", "particleImg", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_particleTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}
		}
		break;

	case ITMMSG_COLLECT:
		{
			//add resources to player profile
			for(int i = 0; i < RC_MAX; i++)
				g_world->ResourceAdd((eResourceType)i, m_rcVal[i]);

			//cool item particle FX
			if(m_particleTxt)
			{
				fxExplode_init explodeFX;
				
				explodeFX.explodeTxt  = m_particleTxt;
				explodeFX.r           = 255;
				explodeFX.g           = 255;
				explodeFX.b           = 255;
				explodeFX.scale       = BoundGetRadius()/2;
				explodeFX.center[eX]  = GetLoc().x;
				explodeFX.center[eY]  = GetLoc().y;
				explodeFX.center[eZ]  = GetLoc().z;
				explodeFX.radius      = BoundGetRadius()*4;
				explodeFX.maxParticle = 25;
				explodeFX.delay       = RC_EXPLODE_DELAY;

				PARFXCreate(ePARFX_EXPLODE, &explodeFX, -1, 0, -1, 0);
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}

//collectibles, for secret stuff and easter eggs
/*class Collectible : public Item
{
public:
	Collectible();
	~Collectible();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	hTXT	m_particleTxt;		//particle texture
};*/
/********************************************************************
*																	*
*							Ar-Ta-Fact								*
*																	*
********************************************************************/
#define COLLECT_EXPLODE_DELAY	1000

Collectible::Collectible() : Item(ITM_COLLECTION), m_particleTxt(0)
{
}

Collectible::~Collectible()
{
	if(m_particleTxt)
		TextureDestroy(&m_particleTxt);
}

int Collectible::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			//load texture for particle
			char buff[MAXCHARBUFF]={0};

			if(CfgGetItemStr(cfg, "special", "particleImg", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_particleTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}
		}
		break;

	case ITMMSG_COLLECT:
		{
			//add collection to player profile
			if(g_world && g_world->ProfileGetCfg())
			{
				CfgAddItemInt(g_world->ProfileGetCfg(), PROFILE_COLLECT_SECTION, GetName(), 1);

				g_world->SetArTaFactMin(g_world->GetArTaFactMin()+1);
			}
			
			//cool item particle FX
			if(m_particleTxt)
			{
				fxExplode_init explodeFX;
				
				explodeFX.explodeTxt  = m_particleTxt;
				explodeFX.r           = 255;
				explodeFX.g           = 255;
				explodeFX.b           = 255;
				explodeFX.scale       = BoundGetRadius()/2;
				explodeFX.center[eX]  = GetLoc().x;
				explodeFX.center[eY]  = GetLoc().y;
				explodeFX.center[eZ]  = GetLoc().z;
				explodeFX.radius      = BoundGetRadius()*4;
				explodeFX.maxParticle = 25;
				explodeFX.delay       = RC_EXPLODE_DELAY;

				PARFXCreate(ePARFX_EXPLODE, &explodeFX, -1, 0, -1, 0);
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}