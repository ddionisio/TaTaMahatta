#include "tata_main.h"
#include "tata_world.h"

#include "tata_globals.h"

#include "tata_menu.h"
#include "tata_menu_game.h"

#include "tata_stage_select.h"

//All updates happen here

/////////////////////////////////////
// Purpose:	Updates the world.
// Output:	stuff happens
// Return:	success if nothing went wrong
/////////////////////////////////////
RETCODE World::Update()
{
	if(CheckFlag(WORLD_FLAG_PAUSE))
		return RETCODE_SUCCESS;

	if(!CheckFlag(WORLD_FLAG_INPUT_DISABLE))
		ClockUpdate(&m_clock);

	Creature *pTaTa=0; //selected Ta-Ta
	//D3DXVECTOR3 oldLoc;

	//set the velocity of the entities
	//this will apply push and gravity vel.
	CollisionEntitiesDoVelocity();

	//update view
	//this will also update the 3D sound listener
	ViewUpdate();

	//update scene if exist
	if(CutsceneUpdate()) 
	{
		//set listener to camera
		ViewSetListener();
	}
	//otherwise, update parts of the game
	//make sure there are Ta-Tas
	else if(!TataIsPlayListEmpty())
	{
		//assuming play list is not empty
		do
		{
			pTaTa = (Creature *)IDPageQuery(m_TaTa[m_curTaTa]);

			if(!pTaTa)
				TataSetCurrentNext();

		} while(!pTaTa);

		//if(pTaTa)
		//	oldLoc = pTaTa->GetLoc();

		///////////////////////////////////////////////////////
		//update input
		if(!CheckFlag(WORLD_FLAG_INPUT_DISABLE))
		{
			//check to see if we press the 'switch forward' btn.
			if(InputIsReleased(INP_C))
			{
				TataSetCurrentPrev();

				HUDSetSelect(m_TaTa[m_curTaTa]);
			}
			//otherwise,
			//check to see if we press the 'switch backward' btn.
			else if(InputIsReleased(INP_D))
			{
				TataSetCurrentNext();

				HUDSetSelect(m_TaTa[m_curTaTa]);
			}

			//check to see if the look-at button is down
			if(InputIsDown(INP_B))
			{
				ViewMainSetDirToDefault();

				m_viewDir.y = 0;

				if(InputIsDown(INP_UP))
				{
					m_viewRotX += m_viewControlSpd.MoveUpdate(g_timeElapse);

					//limit the rotation to 90
					if(m_viewRotX > DAVIDPI/3)
						m_viewRotX = DAVIDPI/3;
				}
				else if(InputIsDown(INP_DOWN))
				{
					m_viewRotX -= m_viewControlSpd.MoveUpdate(g_timeElapse);

					//limit the rotation to -90
					if(m_viewRotX < -DAVIDPI/3)
						m_viewRotX = -DAVIDPI/3;
				}

				if(InputIsDown(INP_LEFT))
				{
					m_viewRotY += m_viewControlSpd.MoveUpdate(g_timeElapse);
				}
				else if(InputIsDown(INP_RIGHT))
				{
					m_viewRotY -= m_viewControlSpd.MoveUpdate(g_timeElapse);
				}

				//create the matrix roation and rotate the view direction
				float rot[eMaxPt];
				OBJGetRotate(pTaTa->GetOBJ(), rot);

				D3DXMATRIX theMtx, rotMtx;

				D3DXMatrixRotationYawPitchRoll(&theMtx, m_viewRotY, m_viewRotX, 0);
				D3DXMatrixRotationY(&rotMtx, rot[eY]);

				theMtx = theMtx*rotMtx;

				D3DXVec3TransformNormal(&m_viewDir, &m_viewDir, &theMtx);

				D3DXVec3Normalize(&m_viewDir, &m_viewDir);
			}
			///////////////////////////////////
			//otherwise...
			//we control the current Ta-Ta
			else if(pTaTa)
			{
				if(InputIsDoubleTap(INP_UP))
				{
					pTaTa->SetFlag(TATA_FLAG_RUN, true);
					OBJSetAnimScale(pTaTa->GetOBJ(), g_runScale);
				}
				else if(InputIsReleased(INP_UP))
				{
					pTaTa->SetFlag(TATA_FLAG_RUN, false);
					OBJSetAnimScale(pTaTa->GetOBJ(), 1);
				}

				m_viewRotX = 0;
				m_viewRotY = 0;

				hOBJ tataOBJ = pTaTa->GetOBJ(); assert(tataOBJ);

				int tataOBJState = OBJGetCurState(tataOBJ);

				//determine if we can control the current Ta-Ta

				//rotate current Ta-Ta if left/right is down
				if(!pTaTa->CheckFlag(TATA_FLAG_DISABLE_ROTATE))
				{
					if(InputIsDown(INP_E))
						pTaTa->InputStrafe();
					else
						pTaTa->InputRotate();
				}

				//we can do anything if we are not falling
				if(tataOBJState != CREANIM_FALL)
				{
					//walk
					pTaTa->InputWalk();

					//check for action
					if(InputIsDown(INP_A))
						pTaTa->Callback(CREMSG_ACTION, INP_STATE_DOWN, 0);
					else if(InputIsReleased(INP_A))
						pTaTa->Callback(CREMSG_ACTION, INP_STATE_RELEASED, 0);
				}

				//
				float rot[eMaxPt];
				OBJGetRotate(pTaTa->GetOBJ(), rot);
				D3DXMATRIX rotMtx;
				D3DXMatrixRotationY(&rotMtx, rot[eY]);
				D3DXVECTOR3 vDir = ViewMainSetDefaultDir();
				D3DXVec3TransformNormal(&vDir, &vDir, &rotMtx);

				//set view direction to default, which is the back of the
				//selected Ta-Ta.
				ViewMainSetDir(vDir);
			}
		}

		//set the view to back of current Ta-Ta.
		if(pTaTa)
		{
			D3DXVECTOR3 tataLoc = pTaTa->GetLoc();
			ViewSetTarget(tataLoc);
			ViewSetPt(tataLoc + (m_viewDist*m_viewDir));

			//D3DXVECTOR3 vsPt = tataLoc + (m_viewDist*m_viewDir);
		}
	}
	//playlist is empty, determine if we want to
	//display game-over (There is no game-over in cloud level)
	else if(!MenuIsAvailable("menu_gameover") 
		&& !HUDIsGoalComplete())
	{
		MenuGameAdd(GAMEMENU_GAMEOVER);
		MenuEnableCurrent(true);
	}

	//update all the stuff in the level
	CollisionEntitiesUpdate();

	//allow air control when going up...
	if(pTaTa && pTaTa->GetVel().y > 0)
	{
		if(InputIsDown(INP_UP))
			pTaTa->MoveDir(1);
		else if(InputIsDown(INP_DOWN))
			pTaTa->MoveDir(-1);
	}

	//collide everything to the world (this will also set their locations)
	//set each Ta-Ta animation (falling, walking...etc)
	CollisionExecute(m_map);
	
	//collide the camera to the world
	if(!m_scene)
	{
		ViewCollideMap(m_map);
	}

	////////////////////////////////////////////////////////////
	//Have all the goals met?
	if(!MenuIsAvailable("menu_victory")
		&& HUDIsGoalComplete())
	{
		//victory dance for all Ta-Tas
		CollisionTaTaVictory();

		//activate victory scene?

		//add victory menu thing.
		MenuGameAdd(GAMEMENU_VICTORY);
		MenuEnableCurrent(true);

		//save data
		StageSelectSaveData(m_profile, m_stage, m_level);

		//unlock the next level
		StageSelectUnlockNext(m_profile, m_stage, m_level);

		/*{
			//if we get here, that means the player has finished
			//all the levels.  Activate ending or something...
		}*/
	}
	////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//update the corpses
	list<Corpse *>::iterator corpseIt = m_corpses.begin(), corpseNext;
	while(corpseIt != m_corpses.end())
	{
		corpseNext = corpseIt;
		corpseNext++;

		Corpse *pCorpse = *corpseIt;

		if(pCorpse->Update())
		{
			delete pCorpse;
			m_corpses.erase(corpseIt);
		}

		corpseIt = corpseNext;
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//update the exiting objects
	list<ExitObj *>::iterator eObjIt = m_exitObjs.begin(), eObjNext;
	while(eObjIt != m_exitObjs.end())
	{
		eObjNext = eObjIt;
		eObjNext++;

		ExitObj *pExitObj = *eObjIt;

		if(pExitObj->Update())
		{
			delete pExitObj;
			m_exitObjs.erase(eObjIt);
		}

		eObjIt = eObjNext;
	}
	//////////////////////////////////////////////////

	list<Id>::iterator IDIt, IDItNext;

	//////////////////////////////////////////////////
	//update the game images
	GameImage *pGameImg;
	IDIt = m_images.begin();

	while(IDIt != m_images.end())
	{
		IDItNext = IDIt;
		IDItNext++;

		pGameImg = (GameImage *)IDPageQuery(*IDIt);

		if(pGameImg)
			pGameImg->Update();
		else
			m_images.erase(IDIt);

		IDIt = IDItNext;
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//update the game dialogs
	GameDialog *pGameDlg;
	IDIt = m_dialogs.begin();

	while(IDIt != m_dialogs.end())
	{
		IDItNext = IDIt;
		IDItNext++;

		pGameDlg = (GameDialog *)IDPageQuery(*IDIt);

		if(pGameDlg)
			pGameDlg->Update();
		else
			m_dialogs.erase(IDIt);

		IDIt = IDItNext;
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//update the game dialogs
	AmbientSound *pAmbient;
	IDIt = m_ambients.begin();

	while(IDIt != m_ambients.end())
	{
		IDItNext = IDIt;
		IDItNext++;

		pAmbient = (AmbientSound *)IDPageQuery(*IDIt);

		if(pAmbient)
		{
			if(pAmbient->Update())
				m_ambients.erase(IDIt);
		}
		else
			m_ambients.erase(IDIt);

		IDIt = IDItNext;
	}
	//////////////////////////////////////////////////

	//update HUD
	HUDUpdate();

	//update objects animation
	GFXUpdateScene();

	//update particles
	PARFXUpdateAll();

	//update any 3D sound changes
	//update the listener
	if(!g_gameFlag.CheckFlag(GAME_FLAG_NOSOUND))
	{
		if(pTaTa)
		{
			BASS_3DVECTOR pos, front, top={0,1,0};//, vel;

			/*vel.x = pTaTa->GetLoc().x - oldLoc.x;
			vel.y = pTaTa->GetLoc().y - oldLoc.y;
			vel.z = -(pTaTa->GetLoc().z - oldLoc.z);*/

			memcpy(&pos, (float*)pTaTa->GetLoc(), sizeof(pos)); pos.z *= -1;
			memcpy(&front, (float*)pTaTa->GetDir(), sizeof(front)); front.z *= -1;

			BASS_Set3DPosition(&pos, /*&vel*/0, &front, &top);
		}		
			
		BASS_Apply3D();
	}

	//check to see if we want to load a map
	WorldPostLoadExec();

	return RETCODE_SUCCESS;
}
