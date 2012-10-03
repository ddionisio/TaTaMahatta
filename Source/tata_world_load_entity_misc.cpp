#include "tata_main.h"
#include "tata_world.h"

#include "tata_object_common.h"

//All other entity junk

////////////////////////////////
//Target
RETCODE World::EntityLoad_Target(hQBSP qbsp, const EntityParse & entityDat)
{
	const char * pName = entityDat.GetVal("targetname");
	string theName = pName ? pName : "NULL";
	D3DXVECTOR3 theLoc;

	//bad target
	if(theName.c_str()[0] == 0)
		return RETCODE_FAILURE;

	//get location
	sscanf(entityDat.GetVal("origin"), "%f %f %f", 
		&theLoc.x, &theLoc.z, &theLoc.y);

	theLoc.z *= -1;

	//add to list
	TargetAdd(theName.c_str(), theLoc);

	return RETCODE_SUCCESS;
}

/////////////////////////////////
//Doodad
RETCODE World::EntityLoad_Doodad(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new doodad
	Doodad *newObj = new Doodad; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//get the waypoint
	pStr = entityDat.GetVal("waypointID");
	if(pStr)
		newObj->SetWayPt(WaypointGet(pStr));

	WayptNode  *pWayPt = newObj->GetWayPoint();

	if(pWayPt)
	{
		///////////////////////////////////////////////////////
		//get active mode
		pStr = entityDat.GetVal("bActive");
		if(pStr)
		{
			int isActive; sscanf(pStr, "%d", &isActive);

			if(!isActive)
				newObj->SetFlag(OBJ_FLAG_INACTIVE, true);
		}

		///////////////////////////////////////////////////////
		//get backward mode
		pStr = entityDat.GetVal("bBackward");
		if(pStr)
		{
			int isBackward; sscanf(pStr, "%d", &isBackward);

			if(isBackward)
				pWayPt->Reverse(true);
		}

		///////////////////////////////////////////////////////
		//get waypoint mode
		pStr = entityDat.GetVal("mode");
		if(pStr)
			pWayPt->SetMode(WaypointGetMode(pStr));
	}

	///////////////////////////////////////////////////////
	//get platform speed
	pStr = entityDat.GetVal("spdDelay");
	if(pStr)
	{
		float spd; sscanf(pStr, "%f", &spd);

		newObj->SetMoveSpd(spd);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////
//Goal
RETCODE World::EntityLoad_Goal(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new trigger
	Goal *newObj = new Goal; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//set the goal type
	pStr = entityDat.GetVal("type");
	if(pStr)
	{
		int goalType = newObj->GetGoalType(pStr);

		newObj->SetGoal(goalType);

		if(goalType == Goal::GOAL_TATAHOME)
			SetFlag(WORLD_FLAG_GOAL_HOME, true);
	}

	///////////////////////////////////////////////////////
	//set the ID entity
	string goalName = "goal";

	pStr = entityDat.GetVal("ID");
	if(pStr)
	{
		newObj->SetRequiredID(pStr);

		goalName += "_";
		goalName += pStr;
	}
	else
	{
		newObj->SetRequiredID("none");
	}

	newObj->SetName(goalName.c_str());

	return RETCODE_SUCCESS;
}

/////////////////////////////////
//Turret
RETCODE World::EntityLoad_Turret(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new trigger
	Turret *newObj = new Turret; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//get active mode
	pStr = entityDat.GetVal("bActive");
	if(pStr)
	{
		int isActive; sscanf(pStr, "%d", &isActive);

		if(!isActive)
			newObj->SetFlag(OBJ_FLAG_INACTIVE, true);
	}

	///////////////////////////////////////////////////////
	//load model
	pStr = entityDat.GetVal("ProjModel");

	if(pStr)
	{
		string mdlPath = GAMEFOLDER;
		mdlPath += "\\";
		mdlPath += pStr;

		newObj->SetProjMDL(MDLCreate(0, mdlPath.c_str()));
	}

	///////////////////////////////////////////////////////
	//load texture FX
	pStr = entityDat.GetVal("TextureFX");

	if(pStr)
	{
		string txtPath = GAMEFOLDER;
		txtPath += "\\";
		txtPath += pStr;

		newObj->SetProjTXT(TextureCreate(0, txtPath.c_str(), false, 0));
	}

	///////////////////////////////////////////////////////
	//load color FX
	int r=255,g=255,b=255;

	pStr = entityDat.GetVal("clrFX");

	if(pStr)
		sscanf(pStr, "%d,%d,%d", &r,&g,&b);

	newObj->SetProjClr(r,g,b);

	///////////////////////////////////////////////////////
	//load speed
	pStr = entityDat.GetVal("spd");

	if(pStr)
	{
		float spd;
		sscanf(pStr, "%f", &spd);
		newObj->SetProjSpd(spd);
	}

	///////////////////////////////////////////////////////
	//load delay
	pStr = entityDat.GetVal("delay");

	if(pStr)
	{
		double delay;
		sscanf(pStr, "%lf", &delay);
		newObj->SetProjDelay(delay);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////
//Geyser
RETCODE World::EntityLoad_Geyser(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new geyser
	Geyser *newObj = new Geyser; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	double val;

	///////////////////////////////////////////////////////
	//get active delay
	pStr = entityDat.GetVal("onDelay");
	if(pStr)
	{
		sscanf(pStr, "%lf", &val);
		newObj->GeyserSetActivateDelay(val);
	}

	///////////////////////////////////////////////////////
	//get inactive delay
	pStr = entityDat.GetVal("offDelay");
	if(pStr)
	{
		sscanf(pStr, "%lf", &val);
		newObj->GeyserSetInActivateDelay(val);
	}

	///////////////////////////////////////////////////////
	//get 'up' spd
	pStr = entityDat.GetVal("upSpd");
	if(pStr)
	{
		sscanf(pStr, "%lf", &val);
		newObj->GeySerSetUpSpd(val);
	}

	hTXT txt=0;
	int r=255,g=255,b=255;

	///////////////////////////////////////////////////////
	//get gas img.
	pStr = entityDat.GetVal("TextureFX");
	if(pStr)
	{
		string txtPath = GAMEFOLDER;
		txtPath += "\\";
		txtPath += pStr;

		txt = TextureCreate(0, txtPath.c_str(), false, 0);
	}

	///////////////////////////////////////////////////////
	//get gas clr
	pStr = entityDat.GetVal("clrFX");
	if(pStr)
		sscanf(pStr, "%d,%d,%d", &r,&g,&b);

	if(txt)
	{
		newObj->GeyserInitParticle(txt, (BYTE)r, (BYTE)g, (BYTE)b);
		TextureDestroy(&txt);
	}

	return RETCODE_SUCCESS;
}

RETCODE World::EntityLoad_Teleport(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new teleport
	Teleport *newObj = new Teleport; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//get target
	pStr = entityDat.GetVal("target");
	if(pStr)
	{
		D3DXVECTOR3 dest; TargetGet(pStr, dest);
		newObj->TeleportSetDest(dest);
	}

	hTXT txt=0;
	int r=255,g=255,b=255;

	///////////////////////////////////////////////////////
	//get tele. img.
	pStr = entityDat.GetVal("TextureFX");
	if(pStr)
	{
		string txtPath = GAMEFOLDER;
		txtPath += "\\";
		txtPath += pStr;

		txt = TextureCreate(0, txtPath.c_str(), false, 0);
	}

	///////////////////////////////////////////////////////
	//get tele. clr
	pStr = entityDat.GetVal("clrFX");
	if(pStr)
		sscanf(pStr, "%d,%d,%d", &r,&g,&b);

	if(txt)
	{
		newObj->TeleportSetFX(txt, (BYTE)r, (BYTE)g, (BYTE)b);
		TextureDestroy(&txt);
	}

	return RETCODE_SUCCESS;
}

RETCODE World::EntityLoad_Lightning(hQBSP qbsp, const EntityParse & entityDat)
{
	//create new teleport
	LightningArea *newObj = new LightningArea; assert(newObj);

	///////////////////////////////////////////////////////
	//load up the common stuff
	EntityLoad_CommonObject(qbsp, entityDat, dynamic_cast<Object *>(newObj));

	const char *pStr;

	///////////////////////////////////////////////////////
	//get deactivate delay
	pStr = entityDat.GetVal("deactivateDelay");
	if(pStr)
	{
		double d;
		sscanf(pStr, "%lf", &d);
		newObj->LightningSetDeactivateDelay(d);
	}

	fxLightningY_init tFX={0};

	///////////////////////////////////////////////////////
	//Get the FX stuff
	pStr = entityDat.GetVal("FXTexture");
	if(pStr)
	{
		string txtPath = GAMEFOLDER;
		txtPath += "\\";
		txtPath += pStr;

		tFX.lightningTxt = TextureCreate(0, txtPath.c_str(), false, 0);
	}

	pStr = entityDat.GetVal("FXColor");
	if(pStr)
	{
		int r,g,b;
		sscanf(pStr, "%d,%d,%d", &r,&g,&b);
		tFX.r = r; tFX.g = g; tFX.b = b;
	}

	pStr = entityDat.GetVal("FXScale");
	if(pStr)
		sscanf(pStr, "%f", &tFX.scale);

	pStr = entityDat.GetVal("FXMaxParticle");
	if(pStr)
		sscanf(pStr, "%d", &tFX.maxParticle);

	pStr = entityDat.GetVal("FXMaxPoint");
	if(pStr)
		sscanf(pStr, "%d", &tFX.maxPoint);

	pStr = entityDat.GetVal("FXDelayON");
	if(pStr)
		sscanf(pStr, "%lf", &tFX.delayOn);

	pStr = entityDat.GetVal("FXDelayMove");
	if(pStr)
		sscanf(pStr, "%lf", &tFX.delayMove);

	memcpy(tFX.min, (float*)(newObj->BoundGetMin()+newObj->GetLoc()), sizeof(tFX.min));
	memcpy(tFX.max, (float*)(newObj->BoundGetMax()+newObj->GetLoc()), sizeof(tFX.max));

	newObj->LightningInitFX(&tFX);

	return RETCODE_SUCCESS;
}

RETCODE World::EntityLoad_AmbientSound(hQBSP qbsp, const EntityParse & entityDat)
{
	/*AmbientSndAdd(const char *name, const char *filename, 
						const D3DXVECTOR3 & loc, const D3DXVECTOR3 & orient, 
						bool bLoop);*/
	string name("huh"), soundPath("none");
	D3DXVECTOR3 loc, orient(0,0,0);
	bool bLoop=false;
	float minLen=1, len=10;

	const char *pStr;

	//get name
	pStr = entityDat.GetVal("ID");
	if(pStr)
		name = pStr;

	//get sound path
	pStr = entityDat.GetVal("sound");
	if(pStr)
	{
		soundPath = GAMEFOLDER;
		soundPath += "\\";
		soundPath += pStr;
	}

	//get the location
	//swap at the same time, the y and z
	sscanf(entityDat.GetVal("origin"), "%f %f %f", 
		&loc.x, &loc.z, &loc.y);
	loc.z *= -1;

	//get the orientation
	pStr = entityDat.GetVal("target");
	if(pStr)
	{
		if(TargetGet(pStr, orient))
		{
			orient -= loc;

			len = D3DXVec3Length(&orient);

			if(len > 0)
			{
				orient /= len;
			}
		}
	}

	//get minimum length
	pStr = entityDat.GetVal("minLength");
	if(pStr)
	{
		sscanf(pStr, "%f", &minLen);
	}

	//do we want to loop?
	pStr = entityDat.GetVal("bLoop");
	if(pStr)
	{
		int iLoop;
		sscanf(pStr, "%d", &iLoop);
		bLoop = iLoop ? true : false;
	}

	AmbientSndAdd(name.c_str(), soundPath.c_str(), loc, orient, minLen, len, bLoop);

	return RETCODE_SUCCESS;
}