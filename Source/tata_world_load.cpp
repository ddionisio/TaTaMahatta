#include "tata_main.h"
#include "tata_world.h"

#include "tata_globals.h"

//Loads the map and initialize everything!!!

//type, this is sorted in priority of loading.
typedef enum {
	ENTITY_UNKNOWN,
	ENTITY_INFO_NULL,
	ENTITY_WAYPOINT,
	ENTITY_LIGHT,
	ENTITY_AMBIENTSOUND,
	ENTITY_TATA,
	ENTITY_ENEMY,
	ENTITY_ITEM,
	ENTITY_BLOCK,
	ENTITY_LEVER,
	ENTITY_BUTTON,
	ENTITY_PLATFORM,
	ENTITY_TURRET,
	ENTITY_GEYSER,
	ENTITY_TELEPORT,
	ENTITY_STEAK,
	ENTITY_SIGN,
	ENTITY_TRIGGER,
	ENTITY_GOAL,
	ENTITY_GAS,
	ENTITY_LIGHTNING,
	ENTITY_DOODAD,
	ENTITY_START,
	ENTITY_WORLDSPAWN,
	ENTITY_MAX
} eEntityParseType;

char *g_EntityStr[ENTITY_MAX] = {0, "Info_Null", "Waypoint", "Light", "AmbientSound", "TaTa", 
"Enemy", "Item", "Block", "Lever", "Button", "Platform", "Turret", "Geyser", 
"Teleport", "Steak", "Sign", "Trigger", "Goal", "OuchField", "Lightning", "Doodad", "Start",
"worldspawn"
};

//loaded from map and sorted
vector<EntityParse> g_entity;

PRIVATE eEntityParseType _EntityGetType(const char *str)
{
	for(int i = 1; i < ENTITY_MAX; i++)
	{
		if(stricmp(g_EntityStr[i], str) == 0)
			return (eEntityParseType)i;
	}

	return ENTITY_UNKNOWN;
}

/********************************************************************
*																	*
*							World Load								*
*																	*
********************************************************************/

//Callback for loading entities inside the map
PROTECTED void WorldEntityParse(char *entityStr, hQBSP qbsp, void *userDat)
{
	//add new entity to global entity
	EntityParse newEntity;
	newEntity.Parse(entityStr);

	g_entity.push_back(newEntity);

	//sort the entity based on it's type
	int type1, type2;

	//simple bubble sort
	for(int i = 0; i < g_entity.size(); i++)
	{
		for(int j = g_entity.size()-1; j > i; j--)
		{
			type1 = _EntityGetType(g_entity[j-1].GetVal("classname"));
			type2 = _EntityGetType(g_entity[j].GetVal("classname"));

			if(type1 > type2)
			{
				EntityParse tmp = g_entity[j-1];
				g_entity[j-1] = g_entity[j];
				g_entity[j] = tmp;
			}
		}
	}
}

//This will load all the entities in the global entity loaded from map
void World::WorldEntityLoadAll()
{
	RETCODE ret = RETCODE_SUCCESS;

	for(int i = 0; i < g_entity.size(); i++)
	{
		TataDisplayLoadText("Loading Object: %s", g_entity[i].GetVal("classname"));

		//determine what type of object by its 'classname'
		switch(_EntityGetType(g_entity[i].GetVal("classname")))
		{
		case ENTITY_START:
			ret = EntityLoad_Start(m_map, g_entity[i]);
			break;
		case ENTITY_WAYPOINT:
			ret = EntityLoad_Waypoint(m_map, g_entity[i]);
			break;
		case ENTITY_AMBIENTSOUND:
			ret = EntityLoad_AmbientSound(m_map, g_entity[i]);
			break;
		case ENTITY_TATA:
			ret = EntityLoad_TaTa(m_map, g_entity[i]);
			break;
		case ENTITY_ENEMY:
			ret = EntityLoad_Enemy(m_map, g_entity[i]);
			break;
		case ENTITY_ITEM:
			ret = EntityLoad_Item(m_map, g_entity[i]);
			break;
		case ENTITY_LIGHT:
			ret = EntityLoad_Light(m_map, g_entity[i]);
			break;
		case ENTITY_BLOCK:
			ret = EntityLoad_Block(m_map, g_entity[i]);
			break;
		case ENTITY_LEVER:
			ret = EntityLoad_Lever(m_map, g_entity[i]);
			break;
		case ENTITY_BUTTON:
			ret = EntityLoad_Button(m_map, g_entity[i]);
			break;
		case ENTITY_PLATFORM:
			ret = EntityLoad_Platform(m_map, g_entity[i]);
			break;
		case ENTITY_TURRET:
			ret = EntityLoad_Turret(m_map, g_entity[i]);
			break;
		case ENTITY_GEYSER:
			ret = EntityLoad_Geyser(m_map, g_entity[i]);
			break;
		case ENTITY_TELEPORT:
			ret = EntityLoad_Teleport(m_map, g_entity[i]);
			break;
		case ENTITY_STEAK:
			ret = EntityLoad_Steak(m_map, g_entity[i]);
			break;
		case ENTITY_SIGN:
			ret = EntityLoad_Sign(m_map, g_entity[i]);
			break;
		case ENTITY_TRIGGER:
			ret = EntityLoad_Trigger(m_map, g_entity[i]);
			break;
		case ENTITY_GOAL:
			ret = EntityLoad_Goal(m_map, g_entity[i]);
			break;
		case ENTITY_GAS:
			ret = EntityLoad_OuchField(m_map, g_entity[i]);
			break;
		case ENTITY_INFO_NULL:
			ret = EntityLoad_Target(m_map, g_entity[i]);
			break;
		case ENTITY_LIGHTNING:
			ret = EntityLoad_Lightning(m_map, g_entity[i]);
			break;
		case ENTITY_DOODAD:
			ret = EntityLoad_Doodad(m_map, g_entity[i]);
			break;
		case ENTITY_WORLDSPAWN:
			break;
		case ENTITY_UNKNOWN:
			ErrorMsg("WorldEntityLoadAll Unknown Entity", g_entity[i].GetVal("classname"), false);
			break;
		}

		if(ret != RETCODE_SUCCESS)
			ErrorMsg("WorldEntityLoadAll Error Loading", g_entity[i].GetVal("classname"), false);
	}
}

/////////////////////////////////////
// Purpose:	loads up the world including 
//			creatures, objects...etc.
// Output:	everything initialized
// Return:	success if success
/////////////////////////////////////
RETCODE World::MapLoad(const char *filename)
{
	TataDisplayLoad();

	//unload previous world
	WorldUnload();

	//set path
	m_filePath = filename;

	//reset artafact collection
	m_artafactMin = m_artafactMax = 0;

	TataDisplayLoadText("Loading Map: %s", filename);

	//load the map and parse/load the entities
	m_map = QBSPLoad(filename, GAMEFOLDER, WorldEntityParse, this);

	if(!m_map)
	{ ErrorMsg("World: MapLoad", "Unable to load Map!", true); return RETCODE_FAILURE; }

	QBSPUseVertexLighting(m_map, g_bUseVertexLighting);

	WorldEntityLoadAll();

	g_entity.clear(); //we don't need these junks anymore.

	ProfilePrepare(); //load all Ta-Tas in the world

	ClockInit(&m_clock);

	fadeaway_init tBKFX={255,255,255,2000};
	TataSetBKFX(BKFXCreate(eBKFX_FADEAWAY, &tBKFX));

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Purpose:	reloads the map
// Output:	everything re-initialized
// Return:	success if success
/////////////////////////////////////
RETCODE World::MapReload()
{
	return MapLoad(m_filePath.c_str());
}

/////////////////////////////////////
// Purpose:	post load the cloud map and
//			set current Ta-Ta's location
//			with given target.
// Output:	cloud map will be loaded after
//			the world update.
// Return:	success
/////////////////////////////////////
RETCODE World::MapPostLoadCloud(const char *targetName, float angleRad, 
								bool bCutsceneRemove)
{
	m_postMapFilename = g_cloudMapPath;

	if(targetName && targetName[0] != 0)
		m_postTarget = targetName;
	else
		m_postTarget = "none";

	m_postAngle = angleRad;

	m_postbCutsceneRemove = bCutsceneRemove;

	SetFlag(WORLD_FLAG_POST_LOAD, true);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Purpose:	post load given map and
//			set current Ta-Ta's location
//			with given target.
// Output:	map will be loaded after
//			the world update.
// Return:	success
/////////////////////////////////////
RETCODE World::MapPostLoad(const char *filename, const char *targetName, 
						   float angleRad, bool bCutsceneRemove)
{
	m_postMapFilename = GAMEFOLDER;
	m_postMapFilename += "\\";
	m_postMapFilename += filename;

	if(targetName && targetName[0] != 0)
		m_postTarget = targetName;
	else
		m_postTarget = "none";

	m_postAngle = angleRad;

	m_postbCutsceneRemove = bCutsceneRemove;

	SetFlag(WORLD_FLAG_POST_LOAD, true);

	return RETCODE_SUCCESS;
}

void World::WorldPostLoadExec()
{
	if(CheckFlag(WORLD_FLAG_POST_LOAD))
	{
		//load map
		MapLoad(m_postMapFilename.c_str());

		if(m_postbCutsceneRemove)
			CutsceneRemove();

		//get current Ta-Ta
		EntityCommon *pEntity = (EntityCommon *)IDPageQuery(TataGetCurrent());

		if(pEntity)
		{
			//get target and set the current Ta-Ta
			if(stricmp(m_postTarget.c_str(), "none") != 0)
			{
				D3DXVECTOR3 loc;
				if(TargetGet(m_postTarget.c_str(), loc))
					pEntity->SetLoc(loc);
			}

			//set the angle
			D3DXMATRIX rotMtx;
			D3DXMatrixRotationY(&rotMtx, m_postAngle);

			D3DXVECTOR3 dir = pEntity->GetDir();
			D3DXVec3TransformNormal(&dir, &dir, &rotMtx);
			pEntity->SetDir(dir);
		}

		SetFlag(WORLD_FLAG_POST_LOAD, false);
	}
}

/////////////////////////////////////
// Purpose:	this will set the current
//			map as the 'previous' on
//			use of MapPrevLoad
// Output:	prev. stuff set
// Return:	none
/////////////////////////////////////
void World::MapSetPrev()
{
	//set prev. stuff
	if(m_filePath.c_str() != 0 && m_filePath.c_str()[0] != 0)
		m_prevFilePath = m_filePath;
	else
		m_prevFilePath = "none";

	///////////////////////////////////////////////////////
	//Save prev. location of current Ta-Ta
	//get current Ta-Ta
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(TataGetCurrent());

	if(pEntity)
	{
		m_prevLoc = pEntity->GetLoc();
		m_prevDir = pEntity->GetDir();
	}
	///////////////////////////////////////////////////////
}

/////////////////////////////////////
// Purpose:	loads the previous map
//			bSetLoc = if true, then set
//			the current Ta-Ta to the
//			previous loc.
//			bSetDir = if true, then set
//			the current Ta-Ta to the
//			previous dir.
// Output:	loads the previous map.
// Return:	success
/////////////////////////////////////
RETCODE World::MapPrevLoad(bool bSetLoc, bool bSetDir, bool bCutsceneRemove)
{
	if(stricmp(m_prevFilePath.c_str(), "none") != 0)
	{
		//load map
		MapLoad(m_prevFilePath.c_str());

		if(bCutsceneRemove)
			CutsceneRemove();

		//get current Ta-Ta
		EntityCommon *pEntity = (EntityCommon *)IDPageQuery(TataGetCurrent());

		if(pEntity)
		{
			if(bSetLoc)
				pEntity->SetLoc(m_prevLoc);

			if(bSetDir)
				pEntity->SetDir(m_prevDir);
		}
	}

	return RETCODE_SUCCESS;
}

//Common Object Entity Load
RETCODE World::EntityLoad_CommonObject(hQBSP qbsp, const EntityParse & entityDat, Object *pObj)
{
	int ind = -1;

	const char *pStr;

	///////////////////////////////////////////////////////
	//Get Identifier
	pStr = entityDat.GetVal("ID");
	if(pStr && pStr[0] != 0)
	{
		EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(pStr));

		/*if(pEntity)
		{
			char buff[MAXCHARBUFF]={0};
			sprintf(buff, "%s_%d_%d", pStr, pObj->GetID().ID, pObj->GetID().counter);
			pObj->SetName(buff);
		}
		else*/
			pObj->SetName(pStr);
	}
	else
	{
		char buff[MAXCHARBUFF]={0};
		sprintf(buff, "obj_%d_%d", pObj->GetID().ID, pObj->GetID().counter);
		pObj->SetName(buff);
	}

	///////////////////////////////////////////////////////
	//get active mode
	pStr = entityDat.GetVal("bActive");
	if(pStr)
	{
		int isActive; sscanf(pStr, "%d", &isActive);

		if(!isActive)
			pObj->SetFlag(OBJ_FLAG_INACTIVE, true);
	}

	///////////////////////////////////////////////////////
	//Get Required Collection
	pStr = entityDat.GetVal("CollectionReq");
	if(pStr)
		pObj->ProfileSetReqCollection(pStr);

	///////////////////////////////////////////////////////
	//Get Required Stage and Level
	pStr = entityDat.GetVal("StageReq");
	if(pStr)
	{
		int stage,lvl;
		sscanf(pStr, "%d,%d", &stage, &lvl);
		pObj->ProfileSetReqStageLevel(stage, lvl);
	}

	///////////////////////////////////////////////////////
	//Get model index
	pStr = entityDat.GetVal("model");
	if(pStr)
	{
		sscanf(pStr, "*%d", &ind);

		pObj->BoundSetModelInd(ind);

		//also get the location
		//set the axis-aligned bound box
		D3DXVECTOR3 ptLoc;

		QBSPModelGetTranslate(qbsp, ind, (float*)ptLoc);
		pObj->SetLoc(ptLoc);

		QBSPRayModelGetBBoxMin(qbsp, ind, (float*)ptLoc);
		ptLoc -= pObj->GetLoc();
		pObj->BoundSetMin(ptLoc);

		QBSPRayModelGetBBoxMax(qbsp, ind, (float*)ptLoc);
		ptLoc -= pObj->GetLoc();
		pObj->BoundSetMax(ptLoc);
	}
	//otherwise, see if it is a model loaded from file
	else
	{
		string mdlAnimPath="none";

		pStr = entityDat.GetVal("modelFileAnim");

		if(pStr)
		{
			mdlAnimPath = GAMEFOLDER;
			mdlAnimPath += "\\";
			mdlAnimPath += pStr;
		}

		pStr = entityDat.GetVal("modelFile");

		if(pStr)
		{
			string mdlPath = GAMEFOLDER;
			mdlPath += "\\";
			mdlPath += pStr;

			hMDL theMdl = MDLCreate(0, mdlPath.c_str());

			if(theMdl)
			{
				if(stricmp(mdlAnimPath.c_str(), "none") == 0)
					pObj->SetOBJ(OBJCreate(0, theMdl, 0,0,0, eOBJ_STILL, 0));
				else
					pObj->SetOBJ(OBJCreate(0, theMdl, 0,0,0, eOBJ_ANIMATE, mdlAnimPath.c_str()));
			}
		}

		//get location
		if(entityDat.GetVal("origin"))
		{
			D3DXVECTOR3 theLoc;
			sscanf(entityDat.GetVal("origin"), "%f %f %f", 
				&theLoc.x, &theLoc.z, &theLoc.y);

			theLoc.z *= -1;

			pObj->SetLoc(theLoc);
		}

		//set direction
		if(entityDat.GetVal("angle"))
		{
			D3DXMATRIX mtx;
			D3DXVECTOR3 dir(1,0,0);
			float angle;
			sscanf(entityDat.GetVal("angle"), "%f", &angle);
			
			angle = D3DXToRadian(angle);

			D3DXMatrixRotationY(&mtx, angle);
			D3DXVec3TransformNormal(&dir, &dir, &mtx);

			pObj->SetDir(dir);
		}
	}

	//set spawn
	pObj->SetSpawnLoc(pObj->GetLoc());

	return RETCODE_SUCCESS;
}