#include "tata_main.h"
#include "tata_world.h"

//All displays happen here

/////////////////////////////////////
// Purpose:	displays everything in the
//			world.
// Output:	stuff put on screen
// Return:	success if success
/////////////////////////////////////
RETCODE World::Display()
{
	//set view
	ViewSetScene();

	//GFXBegin();

	//skybox
	SkyBoxRender();

	//RENDER MAP
	QBSPRender(m_map, ViewGetCam());

	GFXDisplayScene();	 //displays all objects

	//display shadows
	CollisionBltShadows();

	PARFXDisplayAll(); //particles

	GFXBltModeEnable(0);

	//render HUD
	HUDDisplay(m_TaTa, MAXTATAWORLD, m_curTaTa);

	//////////////////////////////////////////////////
	//display the game images
	list<Id>::iterator IDit;
	
	GameImage *pGameImg;
	for(IDit = m_images.begin(); IDit != m_images.end(); IDit++)
	{
		pGameImg = (GameImage *)IDPageQuery(*IDit);

		if(pGameImg)
			pGameImg->Display();
	}

	GameDialog *pGameDlg;
	for(IDit = m_dialogs.begin(); IDit != m_dialogs.end(); IDit++)
	{
		pGameDlg = (GameDialog *)IDPageQuery(*IDit);

		if(pGameDlg)
			pGameDlg->Display();
	}

	GFXBltModeDisable();

	//////////////////////////////////////////////////

	//GFXEnd();

	return RETCODE_SUCCESS;
}