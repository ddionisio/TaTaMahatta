#include "tata_main.h"
#include "tata_world.h"

#include "tata_globals.h"

#include "tata_stage_select.h"

//Cutscene control, including load/unload/update

/////////////////////////////////////
// Purpose:	set the cutscene of the
//			world.
//			NOTE: cutscene takes away
//			the player control and disables
//			AI.
//			NOTE: if section is NULL, then
//				  the entire file is parsed.
//				  format of section: [<section>_begin]
//								     ... // code
//									 [<section>_end]
// Output:	cutscene set
// Return:	success if success
/////////////////////////////////////
RETCODE World::CutsceneSet(const char *filename, const char *section)
{
	if(m_nextScene)
	{ ScriptDestroy(this, &m_nextScene); m_nextScene = 0; }

	if(section && section[0] != 0)
	{
		char beginStr[MAXCHARBUFF], endStr[MAXCHARBUFF];

		strcpy(beginStr, "[");
		strcat(beginStr, section);
		strcat(beginStr, "_begin]");

		strcpy(endStr, "[");
		strcat(endStr, section);
		strcat(endStr, "_end]");

		if(!m_scene)
			m_scene = ScriptLoad(this, filename, beginStr, endStr);
		else
			m_nextScene = ScriptLoad(this, filename, beginStr, endStr);
	}
	else
	{
		if(!m_scene)
			m_scene = ScriptLoad(this, filename, 0, 0);
		else
			m_nextScene = ScriptLoad(this, filename, 0, 0);
	}

	if(!m_scene)
	{ ErrorMsg("World Cutscene", "Error loading scene", true); return RETCODE_FAILURE; }

	HUDActive(false);

	return RETCODE_SUCCESS;
}

//section inside the world scene file
RETCODE World::CutsceneSet(const char *section)
{
	//get script file
	char scriptPath[MAXCHARBUFF];

	strcpy(scriptPath, m_filePath.c_str());
	strcpy(GetExtension(scriptPath), SCENE_EXT);

	return CutsceneSet(scriptPath, section);
}

/////////////////////////////////////
// Purpose:	remove current cutscene
// Output:	cutscene removed and game
//			returns to normal.
// Return:	none
/////////////////////////////////////
void World::CutsceneRemove()
{
	if(m_scene)
	{ ScriptDestroy(this, &m_scene); m_scene = 0; }

	HUDActive(true);
}

//same as CutsceneSet, except for ending
RETCODE World::CutsceneSetEnding(const char *filename, const char *section)
{
	if(m_endScene)
		ScriptDestroy(this, &m_endScene);

	if(section && section[0] != 0)
	{
		char beginStr[MAXCHARBUFF], endStr[MAXCHARBUFF];

		strcpy(beginStr, "[");
		strcat(beginStr, section);
		strcat(beginStr, "_begin]");

		strcpy(endStr, "[");
		strcat(endStr, section);
		strcat(endStr, "_end]");

		m_endScene = ScriptLoad(this, filename, beginStr, endStr);
	}
	else
		m_endScene = ScriptLoad(this, filename, 0, 0);

	if(!m_endScene)
	{ ErrorMsg("World Cutscene", "Error loading scene", true); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

RETCODE World::CutsceneSetEnding(const char *section)
{
	//get script file
	char scriptPath[MAXCHARBUFF];

	strcpy(scriptPath, m_filePath.c_str());
	strcpy(GetExtension(scriptPath), SCENE_EXT);

	return CutsceneSetEnding(scriptPath, section);
}

void World::CutscenePrepareEnding()
{
	SetFlag(WORLD_FLAG_ENDING, true);

	CutsceneRemove();

	HUDActive(false);

	m_scene = m_endScene;
	m_endScene = 0;
}


bool World::CutsceneUpdate()
{
	bool ret = true;

	if(m_scene)
	{
		//update the script until it is done,
		//then remove it and resume the game
		if(ScriptUpdate(this, m_scene) == RETCODE_BREAK)
		{
			CutsceneRemove();

			ViewSetDelay(GAME_VIEW_DELAY);
		}
	}
	//check to see if we are playing the 'ending' for the level
	//if done, load prev. map (which is the level select)
	else if(CheckFlag(WORLD_FLAG_ENDING))
	{
		SetFlag(WORLD_FLAG_ENDING, false);

		//load prev. level, which is the level select
		if(!StageSelectAllLevelDone(g_stage))
			MapPrevLoad(true, true, true);
		else
		{
			g_level = 0;
			MapPrevLoad(false, false, true);
		}
	}
	else if(m_nextScene)
	{ 
		m_scene = m_nextScene; 
		m_nextScene = 0;
	}
	else
		ret = false;

	return ret;
}

void World::_CutsceneDestroy()
{
	CutsceneRemove();

	if(m_nextScene)
	{ ScriptDestroy(this, &m_nextScene); m_nextScene = 0; }

	if(m_endScene)
	{ ScriptDestroy(this, &m_endScene); m_endScene = 0; }
}