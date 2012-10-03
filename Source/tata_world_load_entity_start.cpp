#include "tata_main.h"
#include "tata_world.h"

RETCODE World::EntityLoad_Start(hQBSP qbsp, const EntityParse & entityDat)
{
	///////////////////////////////////////////////////////////////////////
	// Load Scene
	//not much to do...
	const char *pStr;
	const char * pFilename = entityDat.GetVal("sceneFile");

	pStr = entityDat.GetVal("sceneScript");

	if(pStr)
	{
		//get script file
		char scriptPath[MAXCHARBUFF];

		strcpy(scriptPath, m_filePath.c_str());
		strcpy(GetExtension(scriptPath), SCENE_EXT);

		//if pFilename exists, then use that
		CutsceneSet(pFilename ? pFilename : scriptPath, pStr);
	}

	//ending scene

	pFilename = entityDat.GetVal("endSceneFile");

	pStr = entityDat.GetVal("endSceneScript");

	if(pStr)
	{
		//get script file
		char scriptPath[MAXCHARBUFF];

		strcpy(scriptPath, m_filePath.c_str());
		strcpy(GetExtension(scriptPath), SCENE_EXT);

		//if pFilename exists, then use that
		CutsceneSetEnding(pFilename ? pFilename : scriptPath, pStr);
	}

	///////////////////////////////////////////////////////////////////////
	// Load Skybox
	if(entityDat.GetVal("skyTop")
		&& entityDat.GetVal("skyBottom")
		&& entityDat.GetVal("skyFront")
		&& entityDat.GetVal("skyBack")
		&& entityDat.GetVal("skyLeft")
		&& entityDat.GetVal("skyRight"))
	{
		char topStr[MAXCHARBUFF] = GAMEFOLDER,
			 bottomStr[MAXCHARBUFF] = GAMEFOLDER,
			 frontStr[MAXCHARBUFF] = GAMEFOLDER,
			 backStr[MAXCHARBUFF] = GAMEFOLDER,
			 leftStr[MAXCHARBUFF] = GAMEFOLDER,
			 rightStr[MAXCHARBUFF] = GAMEFOLDER;

		strcat(topStr, "\\"); strcat(topStr, entityDat.GetVal("skyTop"));
		strcat(bottomStr, "\\"); strcat(bottomStr, entityDat.GetVal("skyBottom"));
		strcat(frontStr, "\\"); strcat(frontStr, entityDat.GetVal("skyFront"));
		strcat(backStr, "\\"); strcat(backStr, entityDat.GetVal("skyBack"));
		strcat(leftStr, "\\"); strcat(leftStr, entityDat.GetVal("skyLeft"));
		strcat(rightStr, "\\"); strcat(rightStr, entityDat.GetVal("skyRight"));

		SkyBoxInit(500,
			TextureCreate(0, topStr, false, 0),
			TextureCreate(0, bottomStr, false, 0),
			TextureCreate(0, frontStr, false, 0),
			TextureCreate(0, backStr, false, 0),
			TextureCreate(0, leftStr, false, 0),
			TextureCreate(0, rightStr, false, 0));
	}

	///////////////////////////////////////////////////////////////////////
	// Get Goal Stuff
	pStr = entityDat.GetVal("goalImg");

	if(pStr)
	{
		string imgPath = GAMEFOLDER;
		imgPath += "\\";
		imgPath += pStr;

		HUDSetGoalTxt(TextureCreate(0, imgPath.c_str(), false, 0));
	}

	///////////////////////////////////////////////////////////////////////
	// Set Up Goal
	pStr = entityDat.GetVal("goalMax");

	if(pStr)
	{
		int gMax; sscanf(pStr, "%d", &gMax);
		HUDSetGoalMax(gMax);
	}

	///////////////////////////////////////////////////////////////////////
	// Get the level desc
	pStr = entityDat.GetVal("descFile");

	if(pStr)
	{
		string path = GAMEFOLDER;
		path += "\\";
		path += pStr;

		char *strBuff = GrabAllText((char*)path.c_str());

		if(strBuff)
		{
			m_desc = strBuff;

			free(strBuff);
		}
	}

	///////////////////////////////////////////////////////////////////////
	// Get the music
	pStr = entityDat.GetVal("musicFile");

	if(pStr)
	{
		string musPath = GAMEFOLDER;
		musPath += "\\";
		musPath += pStr;

		MusicSet(musPath.c_str());
	}

	return RETCODE_SUCCESS;
}