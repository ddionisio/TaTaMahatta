#include "tata_main.h"
#include "tata_world.h"

#include "tata_globals.h"

#include "tata_collision.h"

#include "tata_creature.h"

#include "tata_creature_tata.h"

double g_hurtInvulDelay;	//delay for invulnerability when hurt

/********************************************************************
*																	*
*						Ta-Ta Roster								*
*																	*
********************************************************************/
TaTaRoster::TaTaRoster() : m_tataPortrait(0)
{
}

TaTaRoster::TaTaRoster(const TaTaRoster & other) : m_tataPortrait(0)
{
	m_tataTypeName = other.m_tataTypeName;

	TextureDestroy(&m_tataPortrait);

	m_tataPortrait = other.m_tataPortrait;

	if(m_tataPortrait)
		TextureAddRef(m_tataPortrait);
}

TaTaRoster & TaTaRoster::operator = (const TaTaRoster & other)
{
	m_tataTypeName = other.m_tataTypeName;

	TextureDestroy(&m_tataPortrait);

	m_tataPortrait = other.m_tataPortrait;

	if(m_tataPortrait)
		TextureAddRef(m_tataPortrait);

	return *this;
}

TaTaRoster & TaTaRoster::operator = (const char *tataTypeName)
{
	TextureDestroy(&m_tataPortrait);
	m_tataPortrait = 0;

	if(tataTypeName)
	{
		m_tataTypeName = tataTypeName;

		const char *tataFilePath = TataDataGetFilePath(tataTypeName);

		if(tataFilePath)
		{
			hCFG cfg = CfgFileLoad(tataFilePath);

			if(cfg)
			{
				m_tataPortrait = Creature::LoadPortrait(cfg);
				CfgFileDestroy(&cfg);
			}
		}
	}

	return *this;
}

TaTaRoster::~TaTaRoster()
{
	if(m_tataPortrait)
		TextureDestroy(&m_tataPortrait);
}

const char * TaTaRoster::GetTypeName()
{
	return m_tataTypeName.c_str();
}

hTXT TaTaRoster::GetPortrait()
{
	return m_tataPortrait;
}

/********************************************************************
*																	*
*							    Corpse								*
*																	*
********************************************************************/
#define CORPSE_FADE_DELAY		4500
#define CORPSE_PAUSE_DELAY		2000
#define CORPSE_FLY_SPD			15

Corpse::Corpse(Creature *pCre, int deathState) : m_obj(0), m_bFading(false)
{
	//duplicate the object of this creature.
	D3DXVECTOR3 creLoc = pCre->GetLoc();

	m_obj = pCre->GetOBJ();//OBJDuplicate(pCre->GetOBJ(), 0, creLoc.x, creLoc.y, creLoc.z);
	OBJAddRef(m_obj);
	OBJSetToLast(m_obj);

	if(!m_obj) { ErrorMsg("Corpse", "Unable to create object", true); }

	//set to death state
	OBJSetState(m_obj, deathState);
	OBJPauseOnStateEnd(m_obj, true);

	//initialize other stuff
	TimeInit(&m_delay, CORPSE_PAUSE_DELAY);
	
	m_upSpd.MoveSetSpeed(CORPSE_FLY_SPD);
}

Corpse::~Corpse()
{
	if(m_obj)
		OBJDestroy(&m_obj);
}

//returns true if this corpse is ready to be deleted
bool Corpse::Update()
{
	bool ret = false;

	//die first and then fade to the heavens
	if(!m_bFading && OBJIsStateEnd(m_obj) && TimeElapse(&m_delay))
	{
		m_bFading = true;

		TimeInit(&m_delay, CORPSE_FADE_DELAY);
	}
	else if(m_bFading)
	{
		//fade
		double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

		if(t >= 1)
			t = 1;

		OBJSetAlpha(m_obj, 1 - t);

		//move up
		float move[eMaxPt] = {0};
		move[eY] = m_upSpd.MoveUpdate(g_timeElapse);
		OBJTranslate(m_obj, move, true);

		//are we done?
		if(t == 1)
			ret = true;
	}

	return ret;
}

/********************************************************************
*																	*
*							Exit Object								*
*																	*
********************************************************************/
#define EXITOBJ_FADE_DELAY		2500
#define EXITOBJ_FLY_SPD			12
#define EXITOBJ_ROT_SPD			6

ExitObj::ExitObj(hOBJ obj, int victoryState, int flyState) : m_flyState(flyState)
{
	//duplicate the object of this creature.
	D3DXVECTOR3 objLoc; OBJGetLoc(obj, (float*)objLoc);

	m_obj = obj;//OBJDuplicate(obj, 0, objLoc.x, objLoc.y, objLoc.z);
	OBJAddRef(obj);
	OBJSetToLast(obj);

	if(!m_obj) { ErrorMsg("ExitObj", "Unable to create object", true); }

	//set to victory state
	OBJSetState(m_obj, victoryState);
	
	m_upSpd.MoveSetSpeed(EXITOBJ_FLY_SPD);
	m_rotSpd.MoveSetSpeed(EXITOBJ_ROT_SPD);
}

ExitObj::~ExitObj()
{
	if(m_obj)
		OBJDestroy(&m_obj);
}

bool ExitObj::Update()
{
	bool ret = false;

	//finish up the victory animation
	if(OBJIsStateEnd(m_obj) 
		&& OBJGetCurState(m_obj) != m_flyState)
	{
		OBJSetState(m_obj, m_flyState);
		TimeInit(&m_delay, EXITOBJ_FADE_DELAY);
	}
	//flying
	else if(OBJGetCurState(m_obj) == m_flyState)
	{
		//fade
		double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

		if(t >= 1)
			t = 1;

		OBJSetAlpha(m_obj, 1 - t);

		//move up
		float move[eMaxPt] = {0};
		move[eY] = m_upSpd.MoveUpdate(g_timeElapse);
		OBJTranslate(m_obj, move, true);

		//rotate
		float rot[eMaxPt] = {0};
		rot[eY] = m_rotSpd.MoveUpdate(g_timeElapse);
		OBJRot(m_obj, rot, true);

		//are we done?
		if(t == 1)
			ret = true;
	}

	return ret;
}

/********************************************************************
*																	*
*							Game Image								*
*																	*
********************************************************************/
#define IMAGE_FADE_DELAY		1000

GameImage::GameImage(hTXT texture, float sX, float sY, float eX, float eY, BYTE r, BYTE g, BYTE b, BYTE a)
: GameID(this), m_clr(D3DCOLOR_RGBA(r,g,b,a)), m_r(r),m_g(g),m_b(b),m_a(a),
  m_state(STATE_DISABLE)
{
	//set location
	m_x = sX;
	m_y = sY;

	m_w = eX-sX;
	m_h = eY-sY;

	//set texture
	m_txt = texture;
	TextureAddRef(m_txt);
}

GameImage::~GameImage()
{
	if(m_txt)
		TextureDestroy(&m_txt);
}

//STATE_NORMAL,
//STATE_FADEIN,
//STATE_FADEOUT
void GameImage::SetState(int state)
{
	if(m_state != state)
	{
		m_state = state;

		switch(m_state)
		{
		case STATE_NORMAL:
			m_clr = D3DCOLOR_RGBA(m_r,m_g,m_b,m_a);
			break;
		case STATE_FADEIN:
			m_rS=0; m_rE=m_r;
			m_gS=0; m_gE=m_g;
			m_bS=0; m_bE=m_b;
			m_aS=0; m_aE=m_a;


			TimeInit(&m_delay, IMAGE_FADE_DELAY);
			break;
		case STATE_FADEOUT:
			m_rS=m_r; m_rE=0;
			m_gS=m_g; m_gE=0;
			m_bS=m_b; m_bE=0;
			m_aS=m_a; m_aE=0;

			TimeInit(&m_delay, IMAGE_FADE_DELAY);
			break;
		}
	}
}

//returns true when ready to die.
bool GameImage::Update()
{
	bool ret = false;

	switch(m_state)
	{
	case STATE_NORMAL:
		break;

	case STATE_DISABLE:
		break;

	case STATE_FADEIN:
		{
			double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

			if(t >= 1)
			{ t = 1; SetState(STATE_NORMAL); }

			BYTE r,g,b,a;

			r = (BYTE)(m_rS + t*(m_rE-m_rS));
			g = (BYTE)(m_gS + t*(m_gE-m_gS));
			b = (BYTE)(m_bS + t*(m_bE-m_bS));
			a = (BYTE)(m_aS + t*(m_aE-m_aS));

			m_clr = D3DCOLOR_RGBA(r,g,b,a);
		}
		break;

	case STATE_FADEOUT:
		{
			double t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

			if(t >= 1)
			{ t = 1; ret = true; SetState(STATE_DISABLE); }//

			BYTE r,g,b,a;

			r = (BYTE)(m_rS + t*(m_rE-m_rS));
			g = (BYTE)(m_gS + t*(m_gE-m_gS));
			b = (BYTE)(m_bS + t*(m_bE-m_bS));
			a = (BYTE)(m_aS + t*(m_aE-m_aS));

			m_clr = D3DCOLOR_RGBA(r,g,b,a);
		}
		break;
	}

	return ret;
}

void GameImage::Display()
{
	if(m_state != STATE_DISABLE)
	{
		//set location
		SIZE scrnSize = GFXGetScrnSize();

		float x = m_x*scrnSize.cx;
		float y = m_y*scrnSize.cy;

		float w = m_w*scrnSize.cx;
		float h = m_h*scrnSize.cy;

		TextureStretchBlt(m_txt, x, y, w, h, 0, &m_clr, 0);
	}
}

//graphical dialog
#define GFX_DLG_DELAY	1000
#define GFX_DLG_STR_DELAY	50

/********************************************************************
*																	*
*							Graphical Dialog						*
*																	*
********************************************************************/
GameDialog::GameDialog(hTXT texture, float sX, float sY, float eX, float eY)
: GameID(this), m_texture(texture), m_h(0), m_textClr(0xffffffff),
m_borderClr(D3DCOLOR_RGBA(255,255,255,0)), m_state(0), m_string("\0"), 
m_stringDisplay("\0"), m_txtEnd(0), m_font(0)
{
	TextureAddRef(m_texture);

	//set location
	m_x = sX;
	m_sCY = sY;

	m_w = eX-m_x;
	m_sCH = eY-m_sCY;

	m_y = m_sCY + m_sCH/2;

	TimeInit(&m_delay, GFX_DLG_DELAY);

	TimeInit(&m_delayStr, GFX_DLG_STR_DELAY);
}

GameDialog::~GameDialog()
{
	if(m_font)
		FontDestroy(&m_font);

	if(m_texture)
		TextureDestroy(&m_texture);
}

void GameDialog::Open()
{
	m_state = 2;

	TimeReset(&m_delay);
}

void GameDialog::Close()
{
	m_state = 1;

	TimeReset(&m_delay);

	m_txtEnd = 0;
	m_stringDisplay = "\0";
}

void GameDialog::Resize()
{
	FontResize(m_font, (unsigned int)(m_fntRelSize*GFXGetScrnSize().cx));
}

void GameDialog::SetFont(const char *typeName, float size, unsigned int fmtFlag)
{
	m_fntRelSize = size;

	if(m_font)
		FontDestroy(&m_font);

	m_font = FontCreate(typeName, (unsigned int)(size*GFXGetScrnSize().cx), fmtFlag);
}

void GameDialog::SetString(const char *str)
{
	if(str)
		m_string = str;
	else
		m_string = "\0";

	m_txtEnd = 0;

	m_stringDisplay = "\0";

	TimeReset(&m_delayStr);
}

void GameDialog::SetStringClr(BYTE r, BYTE g, BYTE b)
{
	m_textClr = D3DCOLOR_RGBA(r,g,b,255);
}

void GameDialog::Update()
{
	if(m_state != 0)
	{
		float t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

		if(t > 1)
			t = 1;

		switch(m_state)
		{
		case 1: //closing
			m_y = m_sCY + t*(m_sCH/2);
			m_h = m_sCH - t*m_sCH;
			m_borderClr = D3DCOLOR_RGBA(255,255,255, (BYTE)(255.0f - (t*255.0f)));
			break;

		case 2: //opening
			m_y = (m_sCY + (m_sCH/2)) - t*(m_sCH/2);
			m_h = t*m_sCH;
			m_borderClr = D3DCOLOR_RGBA(255,255,255, (BYTE)(t*255.0f));
			break;
		}

		if(t == 1)
			m_state = 0;
	}
	//update string
	else if(TimeElapse(&m_delayStr) && !IsTextComplete())
	{
		char str[2] = {0};
		str[0] = m_string.c_str()[m_txtEnd];

		m_stringDisplay += str;

		m_txtEnd++;
	}
	
	//set to full text if a btn released
	if(InputAnyBtnReleased()
		|| INPXKbIsReleased(DIK_RETURN)
		|| INPXKbIsReleased(DIK_ESCAPE))
	{
		m_txtEnd = m_string.size();
		m_stringDisplay = m_string;
	}
}

void GameDialog::Display()
{
	SIZE scrnSize = GFXGetScrnSize();

	float x = m_x*scrnSize.cx;
	float y = m_y*scrnSize.cy;
	float w = m_w*scrnSize.cx;
	float h = m_h*scrnSize.cy;

	//display border
	TextureStretchBlt(m_texture, x, y, w, h, 0, &m_borderClr, 0);

	//display text
	if(m_font && m_h == m_sCH)
		FontTextBox(m_font, x, y, x+w, y+h, m_textClr, m_stringDisplay.c_str());
}

bool GameDialog::IsTextComplete()
{
	if(m_txtEnd < m_string.size())
		return false;

	return true;
}

/********************************************************************
*																	*
*								World								*
*																	*
********************************************************************/
World::World() : m_map(0), m_scene(0), m_nextScene(0), m_endScene(0), m_profile(0), m_curTaTa(0),
m_viewDist(0), m_viewRotX(0), m_viewRotY(0), m_music(0)
{
	m_musicPath = "none";

	//set all TaTa to invalid
	for(int i = 0; i < MAXTATAWORLD; i++)
	{
		m_TaTa[i].ID = 0;
		m_TaTa[i].counter = -1;
	}

	//set resources to zero
	memset(m_resources, 0, sizeof(m_resources));

	//load game config and set some stuff
	char gamePath[DMAXCHARBUFF]={0};
	TataGetGameCfgPath(gamePath);

	hCFG cfg = CfgFileLoad(gamePath);

	if(cfg)
	{
		///////////////////////////////////////
		//load the view direction
		D3DXVECTOR3 vDir(0,0,1);
		float rotX, rotY, rotZ;

		rotX = CfgGetItemFloat(cfg, GAME_SECTION, "viewrotX");
		rotY = CfgGetItemFloat(cfg, GAME_SECTION, "viewrotY");
		rotZ = CfgGetItemFloat(cfg, GAME_SECTION, "viewrotZ");

		rotX = D3DXToRadian(rotX);
		rotY = D3DXToRadian(rotY);
		rotZ = D3DXToRadian(rotZ);

		D3DXMATRIX theMtx;
		D3DXMatrixRotationYawPitchRoll(&theMtx, rotY, rotX, rotZ);
		D3DXVec3TransformNormal(&vDir, &vDir, &theMtx);

		ViewMainSetDefaultDir(vDir);
		ViewMainSetDirToDefault();

		///////////////////////////////////////
		//load the view distance
		ViewMainSetDist(CfgGetItemFloat(cfg, GAME_SECTION, "viewdist"));

		///////////////////////////////////////
		//load the view move speed
		Move vMove;
		vMove.MoveSetSpeed(CfgGetItemFloat(cfg, GAME_SECTION, "viewspd"));

		ViewMainSetCtrlMove(vMove);

		///////////////////////////////////////
		//load the 'down' (or 'gravity' if you prefer) speed
		CollisionSetGravSpd(CfgGetItemFloat(cfg, GAME_SECTION, "downspd"));

		///////////////////////////////////////
		//load delay for invulnerability when hurt
		g_hurtInvulDelay = CfgGetItemFloat(cfg, GAME_SECTION, "hurtInvulDelay");

		///////////////////////////////////////
		//load HUD stuff
		HUDLoad(cfg);

		CfgFileDestroy(&cfg);
	}

	ViewSetDelay(GAME_VIEW_DELAY);
}

World::~World()
{
	WorldUnload();

	//destroy profile
	if(m_profile)
	{ CfgFileDestroy(&m_profile); m_profile=0; }

	///////////////////////////////////////////////////////
	//destroy music
	MusicFree();
}

void World::WorldUnload()
{
	ProfileSave();

	///////////////////////////////////////////////////////
	//destroy scenes
	_CutsceneDestroy();
	///////////////////////////////////////////////////////

	//remove HUD select indicator
	//(so the select object won't be deleted by parent)
	HUDRemoveSelect();

	///////////////////////////////////////////////////////
	//clear the entity list
	CollisionDestroyAllEntities();
	///////////////////////////////////////////////////////

	//destroy map
	if(m_map)
	{ QBSPDestroy(&m_map); m_map=0; }

	////////////////////////////////////////////////////////////////////
	//destroy remaining corpses
	Corpse *pCorpse;
	for(list<Corpse *>::iterator i = m_corpses.begin(); i != m_corpses.end(); i++)
	{
		pCorpse = *i;

		if(pCorpse)
			delete pCorpse;
	}

	m_corpses.clear();
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	//destroy remaining exit objects
	//m_exitObjs
	ExitObj *pExitObj;
	for(list<ExitObj *>::iterator eoIt = m_exitObjs.begin(); eoIt != m_exitObjs.end(); eoIt++)
	{
		pExitObj = *eoIt;

		if(pExitObj)
			delete pExitObj;
	}

	m_exitObjs.clear();
	////////////////////////////////////////////////////////////////////

	list<Id>::iterator IDIt;

	////////////////////////////////////////////////////////////////////
	//destroy the images
	GameImage *pGameImg;
	for(IDIt = m_images.begin(); IDIt != m_images.end(); IDIt++)
	{
		pGameImg = (GameImage *)IDPageQuery(*IDIt);

		if(pGameImg)
			delete pGameImg;
	}

	m_images.clear();
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	//destroy the dialogs
	GameDialog *pGameDlg;
	for(IDIt = m_dialogs.begin(); IDIt != m_dialogs.end(); IDIt++)
	{
		pGameDlg = (GameDialog *)IDPageQuery(*IDIt);

		if(pGameDlg)
			delete pGameDlg;
	}

	m_dialogs.clear();
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	//destroy the ambients
	AmbientSound *pAmbient;
	for(IDIt = m_ambients.begin(); IDIt != m_ambients.end(); IDIt++)
	{
		pAmbient = (AmbientSound *)IDPageQuery(*IDIt);

		if(pAmbient)
			delete pAmbient;
	}

	m_ambients.clear();
	////////////////////////////////////////////////////////////////////

	//no goal
	HUDSetGoalTxt(0);
	HUDSetGoalMinFixed(0);
	HUDSetGoalMin(0);
	HUDSetGoalMax(0);

	//destroy the lights
	LightDestroyAll();

	//destroy the waypoints
	for(int wI = 0; wI < m_waypts.size(); wI++)
	{
		if(m_waypts[wI])
		{
			Waypoint *wp = m_waypts[wI];
			delete wp;
		}
	}

	m_waypts.clear();

	//destroy targets
	m_targets.clear();

	//remove the sky
	SkyBoxDestroy();

	//no description default
	m_desc = "none";

	//destroy all particles
	PARFXDestroyAll();

	//set current to 0
	m_curTaTa = 0;

	///////////////////////////////////////////////////////
}

/////////////////////////////////////
// Purpose:	get map description
// Output:	none
// Return:	description
/////////////////////////////////////
const char * World::MapGetDesc() 
{ 
	if(stricmp(m_desc.c_str(), "none") == 0)
		return 0;

	return m_desc.c_str(); 
}

/////////////////////////////////////
// Purpose:	add a corpse to the world
//			that will evaporate...or something
// Output:	corpse added
// Return:	none
/////////////////////////////////////
void World::AddCorpse(Creature *pCre, int deathState)
{
	Corpse *newCorpse = new Corpse(pCre, deathState);

	m_corpses.push_back(newCorpse);
}

/////////////////////////////////////
// Purpose:	add an object ready for exit
// Output:	exit object added
// Return:	none
/////////////////////////////////////
void World::AddExitObj(hOBJ obj, int victoryState, int flyState)
{
	ExitObj *newExitObj = new ExitObj(obj, victoryState, flyState);

	m_exitObjs.push_back(newExitObj);
}

/////////////////////////////////////
// Purpose:	add an image to be displayed
//			on screen.
// Output:	image added
// Return:	the ID of image display
/////////////////////////////////////
Id World::ImageDisplayAdd(hTXT texture, float sX, float sY, float eX, float eY, BYTE r, BYTE g, BYTE b, BYTE a)
{
	GameImage *newImage = new GameImage(texture, sX, sY, eX, eY, r,g,b,a);

	m_images.push_back(newImage->GetID());

	return newImage->GetID();
}


/////////////////////////////////////
// Purpose:	fade-in/out given image
// Output:	image will show up on screen
// Return:	none
/////////////////////////////////////
void World::ImageDisplayFadeIn(const Id & ID, bool bIn)
{
	GameImage *pImage = (GameImage *)IDPageQuery(ID);

	if(pImage)
	{
		if(bIn && (pImage->GetState() == GameImage::STATE_DISABLE))
			pImage->SetState(GameImage::STATE_FADEIN);
		else if(!bIn && pImage->GetState() == GameImage::STATE_NORMAL)
			pImage->SetState(GameImage::STATE_FADEOUT);
	}
}

/////////////////////////////////////
// Purpose:	add a dialog to be displayed
//			coordinates are relative [0,1]
// Output:	dialog added
// Return:	the ID of dialog
/////////////////////////////////////
Id World::DialogAdd(hTXT texture, float sX, float sY, float eX, float eY)
{
	GameDialog *newDlg = new GameDialog(texture, sX, sY, eX, eY);

	m_dialogs.push_back(newDlg->GetID());

	return newDlg->GetID();
}

void World::DialogResize()
{
	GameDialog *dlg;

	for(list<Id>::iterator i = m_dialogs.begin(); i != m_dialogs.end(); i++)
	{
		dlg = (GameDialog *)IDPageQuery(*i);

		if(dlg)
			dlg->Resize();
	}
}

/////////////////////////////////////
// Purpose:	adds an ambient sound
// Output:	ambient sound added and
//			played
// Return:	the ambient ID
/////////////////////////////////////
Id World::AmbientSndAdd(const char *name, const char *filename, 
						const D3DXVECTOR3 & loc, const D3DXVECTOR3 & orient,
						float minLength, float length,
						bool bLoop)
{
	AmbientSound *newAmbient = new AmbientSound(name, filename, loc, orient, minLength, length, true);

	m_ambients.push_back(newAmbient->GetID());

	return newAmbient->GetID();
}

//
// Waypoint functions
//

/////////////////////////////////////
// Purpose:	grab the waypoint
// Output:	none
// Return:	the Id
/////////////////////////////////////
Id World::WaypointGet(const char *name)
{
	if(name)
	{
		for(int i = 0; i < m_waypts.size(); i++)
		{
			if(m_waypts[i])
			{
				if(strcmp(m_waypts[i]->GetName(), name) == 0)
					return m_waypts[i]->GetID();
			}
		}
	}

	Id badID={0,-1};

	return badID;
}

//
// Ta-Ta functions
//

/////////////////////////////////////
// Purpose:	TataAddRoster
// Output:	add a Ta-Ta type to roster
// Return:	true if added, false if
//			roster is full
/////////////////////////////////////
bool World::TataAddToRoster(const char *tataTypeName)
{
	if(m_TaTaRoster.size() < MAXTATAWORLD && tataTypeName != 0)
	{
		TaTaRoster newRoster;
		newRoster = tataTypeName;

		m_TaTaRoster.push_back(newRoster);

		return true;
	}

	return false;
}

/////////////////////////////////////
// Purpose:	TataRetrieveFromRoster
// Output:	retrieves an available Ta-Ta
//			in the roster, this will get
//			the first Ta-Ta added
//			given string is filled
// Return:	true if string is filled,
//			false if roster is empty
/////////////////////////////////////
bool World::TataRetrieveFromRoster(string & strOut)
{
	if(m_TaTaRoster.size() > 0)
	{
		strOut = m_TaTaRoster.front().GetTypeName();
		m_TaTaRoster.pop_front();

		return true;
	}

	return false;
}

/////////////////////////////////////
// Purpose:	remove given Ta-Ta from
//			the roster
// Output:	roster updated
// Return:	true if removed, false if
//			Ta-Ta is not in roster.
/////////////////////////////////////
bool World::TataRemoveFromRoster(const string & tataTypeName)
{
	for(list<TaTaRoster>::iterator i = m_TaTaRoster.begin(); i != m_TaTaRoster.end(); i++)
	{
		if(stricmp((*i).GetTypeName(), tataTypeName.c_str()) == 0)
		{
			m_TaTaRoster.erase(i);
			return true;
		}
	}

	return false;
}

/////////////////////////////////////
// Purpose:	check to see if this Ta-Ta
//			is in roster.
// Output:	none
// Return:	true if in roster
/////////////////////////////////////
bool World::TataInRoster(const string & tataTypeName)
{
	for(list<TaTaRoster>::iterator i = m_TaTaRoster.begin(); i != m_TaTaRoster.end(); i++)
	{
		if(stricmp((*i).GetTypeName(), tataTypeName.c_str()) == 0)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Purpose:	check to see if play list
//			is full.
// Output:	none
// Return:	true if full.
/////////////////////////////////////
bool World::TataIsPlayListFull()
{
	Creature *pTata;

	for(int i = 0; i < MAXTATAWORLD; i++)
	{
		pTata = (Creature *)IDPageQuery(m_TaTa[i]);

		if(!pTata)
			return false;
	}

	return true;
}

/////////////////////////////////////
// Purpose:	check to see if play list
//			is empty.
// Output:	none
// Return:	true if empty.
/////////////////////////////////////
bool World::TataIsPlayListEmpty()
{
	Creature *pTata;

	for(int i = 0; i < MAXTATAWORLD; i++)
	{
		pTata = (Creature *)IDPageQuery(m_TaTa[i]);

		if(pTata)
			return false;
	}

	return true;
}

/////////////////////////////////////
// Purpose:	add Ta-Ta to play list if
//			a slot is available
// Output:	creature added
// Return:	true if added, false if 
//			play list is full
/////////////////////////////////////
bool World::TataAddToPlayList(Creature *cre)
{
	Creature *pTata;

	for(int i = 0; i < MAXTATAWORLD; i++)
	{
		pTata = (Creature *)IDPageQuery(m_TaTa[i]);

		//...erm...we already have this Ta-Ta...
		if(GAME_IDISEQUAL(cre->GetID(), m_TaTa[i]))
			return true;

		if(!pTata)
		{
			m_TaTa[i] = cre->GetID();
			return true;
		}
	}

	return false;
}

/////////////////////////////////////
// Purpose:	set the currently selected
//			Ta-Ta to previous in play
//			list
// Output:	currently selected Ta-Ta set
// Return:	none
/////////////////////////////////////
void World::TataSetCurrentPrev()
{
	Creature *pTaTa;

	if(TataIsPlayListEmpty())
		return;

	int prevInd = m_curTaTa;

	while(1)
	{
		prevInd--;

		if(prevInd < 0)
			prevInd = MAXTATAWORLD-1;

		pTaTa = (Creature *)IDPageQuery(m_TaTa[prevInd]);

		if(pTaTa != 0)
			break;
	}

	m_curTaTa = prevInd;
}

/////////////////////////////////////
// Purpose:	set the currently selected
//			Ta-Ta to next in play
//			list
// Output:	currently selected Ta-Ta set
// Return:	none
/////////////////////////////////////
void World::TataSetCurrentNext()
{
	Creature *pTaTa;

	if(TataIsPlayListEmpty())
		return;

	int nextInd = m_curTaTa;

	while(1)
	{
		nextInd++;

		if(nextInd >= MAXTATAWORLD)
			nextInd = 0;

		pTaTa = (Creature *)IDPageQuery(m_TaTa[nextInd]);

		if(pTaTa != 0)
			break;
	}

	m_curTaTa = nextInd;
}

/////////////////////////////////////
// Purpose:	check to see if the given
//			Ta-Ta type name is on the
//			playlist.
// Output:	none
// Return:	true if found, false if not
//			in playlist.
/////////////////////////////////////
bool World::TataOnPlayList(const char *tataTypeName)
{
	int tataType = TataGetType(tataTypeName);

	if(tataType != -1)
	{
		Creature *pTaTa;

		for(int i = 0; i < MAXTATAWORLD; i++)
		{
			pTaTa = (Creature *)IDPageQuery(m_TaTa[i]);

			if(pTaTa)
			{
				if(pTaTa->GetSubType() == tataType)
					return true;
			}
		}
	}

	return false;
}

/////////////////////////////////////
// Purpose:	get the current Ta-Ta the
//			player is controlling.
// Output:	none
// Return:	the Ta-Ta ID.
/////////////////////////////////////
Id	World::TataGetCurrent()
{
	return m_TaTa[m_curTaTa];
}

/////////////////////////////////////
// Purpose:	get a Ta-Ta in the play
//			list.
// Output:	none
// Return:	the Ta-Ta ID.
/////////////////////////////////////
Id World::TataGet(int ind)
{
	if(ind < MAXTATAWORLD)
		return m_TaTa[ind];

	Id badID = {0,-1};

	return badID;
}

/////////////////////////////////////
// Purpose:	change the current Ta-Ta
//			being controlled.
//			NOTE: this function ASSUMES
//			that the given ID is a Ta-Ta!
// Output:	set current Ta-Ta
// Return:	none
/////////////////////////////////////
void World::TataChangeCurrent(const Id & ID)
{
	if(GAME_IDVALID(ID))
		m_TaTa[m_curTaTa] = ID;
}

/////////////////////////////////////
// Purpose:	spawn a Ta-Ta with given
//			location.
// Output:	a Ta-Ta spawned.
// Return:	new Ta-Ta, NULL if something 
//			bad happened
/////////////////////////////////////
Creature * World::TataSpawn(const char *tataTypeName, const D3DXVECTOR3 & loc,
							const char *overrideDatType)
{
	Creature *newTata=0;

	//get type index
	int type = TataGetType(tataTypeName);

	if(type != -1)
	{
		//allocate to appropriate Ta-Ta
		switch(type)
		{
		case TATA_CAPTAINTATA:
			newTata = new CaptainTaTa;
			break;
		case TATA_TATATUG:
			newTata = new TaTaTug;
			break;
		case TATA_TINKERTA:
			newTata = new TinkerTa;
			break;
		case TATA_TATATRAMPLE:
			newTata = new TaTaTrample;
			break;
		case TATA_HOPTAHOP:
			newTata = new HopTaHop;
			break;
		case TATA_FROSTTA:
			newTata = new FrostTa;
			break;
		case TATA_PARATATA:
			newTata = new ParaTaTa;
			break;
		case TATA_LOOPTALOOP:
			newTata = new LoopTaLoop;
			break;
		case TATA_KEYKEYTA:
			newTata = new KeyKeyTa;
			break;
		case TATA_CHITA:
			newTata = new ChiTa;
			break;
		case TATA_TAVATAR:
			newTata = new TaVatar;
			break;

		case TATA_BABYTATA:
			newTata = new BabyTaTa;
			break;
		}

		RETCODE ret;

		newTata->SetName(tataTypeName);

		if(overrideDatType && overrideDatType[0] != 0)
			ret = newTata->Load(overrideDatType);
		else
			ret = newTata->Load(tataTypeName);

		//load the Ta-Ta
		if(ret == RETCODE_SUCCESS)
		{
			/////////////////////////////////
			//set location
			newTata->SetLoc(loc);
		}
		else
		{ delete newTata; newTata = 0; }
	}

	return newTata;
}

/////////////////////////////////////
// Purpose:	destroys all Ta-Ta in the
//			world.  Also includes the
//			Ta-Ta in the playlist.
// Output:	all Ta-Tas gone...
// Return:	none
/////////////////////////////////////
void World::TataDestroyAll()
{
	TaTaCommon *pTata;

	int i;

	for(i = 0; i < TATA_MAX; i++)
	{
		pTata = (TaTaCommon *)IDPageQuery(CollisionGetEntity(g_tataTypeName[i]));

		if(pTata)
			delete pTata;
	}

	//clear out the play list
	for(i = 0; i < MAXTATAWORLD; i++)
		m_TaTa[i].counter = -1;

	//clear the roster
	m_TaTaRoster.clear();
}

//
// Misc. Functions
//

hQBSP World::GetMap()
{
	return m_map;
}

void World::ResourceAdd(eResourceType type, short val)
{ 
	m_resources[type] += val;
}

void World::SetStageLevel(int stage, int level)
{
	m_stage = stage;
	m_level = level;
}