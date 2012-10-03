#include "tata_main.h"
#include "tata_world.h"

#include "tata_globals.h"

#include "tata_collision.h"

#include "tata_stage_select.h"

/////////////////////////////////////
// Purpose:	create new player profile
//			this will set default stuff
// Output:	player profile loaded
// Return:	success if success
/////////////////////////////////////
RETCODE World::ProfileNew(const char *name)
{
	RETCODE ret = ProfileLoad(name);

	//unlock first level
	StageSelectUnlock(m_profile, 0, 0);

	return ret;
}

/////////////////////////////////////
// Purpose:	loads the player profile
// Output:	player profile loaded
// Return:	success if success
/////////////////////////////////////
RETCODE World::ProfileLoad(const char *name)
{
	TataDisplayLoad();

	TataDisplayLoadText("Loading Profile: %s", name);

	string savePath;

	char sPath[DMAXCHARBUFF]={0};
	TataGetSaveDir(sPath);

	strcat(sPath, "\\");
	strcat(sPath, name);
	strcat(sPath, SAVE_EXT);

	//load cfg
	if(m_profile)
		CfgFileDestroy(&m_profile);

	m_profile = CfgFileLoad(sPath);

	if(m_profile)
	{
		//destroy all Ta-Ta's in the world
		/*TataDestroyAll();

		//add the first 4 available Ta-Ta in profile to the roster
		for(int i = 0; i < TATA_MAX; i++)
		{
			if(CfgGetItemInt(m_profile, "TaTa", g_tataTypeName[i]) == 1)
			{
				if(!TataAddToRoster(g_tataTypeName[i]))
					break;
			}
		}*/

		//set the resources
		for(int i = 0; i < RC_MAX; i++)
			m_resources[i] = CfgGetItemInt(m_profile, "Resource", g_rcStr[i]);

		//load stages
		StageSelectInit(g_levelListPath.c_str(), m_profile);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Purpose:	saves the profile to the
//			save directory
// Output:	profile saved
// Return:	none
/////////////////////////////////////
void World::ProfileSave()
{
	if(m_profile)
		CfgFileSave(m_profile);
}

/////////////////////////////////////
// Purpose:	loads all stuff from the
//			profile to the world
// Output:	Ta-Tas spawned
// Return:	none
/////////////////////////////////////
void World::ProfilePrepare()
{
	if(m_profile)
	{
		Creature *pTata;
		D3DXVECTOR3 spawnPt;

		for(int i = 0; i < TATA_MAX; i++)
		{
			if(CfgGetItemInt(m_profile, "TaTa", g_tataTypeName[i]) == 1)
			{
				//make sure this Ta-Ta is unique
				if(!GAME_IDVALID(CollisionGetEntity(g_tataTypeName[i])))
				{
					//get spawn point
					if(TargetGet(g_tataTypeName[i], spawnPt))
					{
						//spawn Ta-Ta
						pTata = TataSpawn(g_tataTypeName[i], spawnPt);

						//add to playlist if playlist is not empty
						//TataAddToPlayList(pTata);
					}
				}
			}
		}
	}

	CollisionPrepare(m_profile);
}

/////////////////////////////////////
// Purpose:	use this to add/set stuff
//			inside the player profile.
//			DON'T DESTROY THE PROFILE!!!
// Output:	none
// Return:	profile config
/////////////////////////////////////
hCFG World::ProfileGetCfg()
{
	return m_profile;
}