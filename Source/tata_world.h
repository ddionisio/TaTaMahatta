#ifndef _tata_world_h
#define _tata_world_h

#include "tata_view.h"
#include "tata_waypoint.h"
#include "tata_creature.h"
#include "tata_object.h"
#include "tata_projectiles.h"
#include "tata_item.h"

#include "tata_basic.h"

#include "tata_entityparse.h"

//This is the mac daddy, everything happens here!

class TaTaRoster {
public:
	TaTaRoster();
	TaTaRoster(const TaTaRoster & other);
	TaTaRoster & operator = (const TaTaRoster & other);
	TaTaRoster & operator = (const char *tataTypeName);
	~TaTaRoster();

	const char *GetTypeName();
	hTXT		GetPortrait();

private:
	string m_tataTypeName;
	hTXT   m_tataPortrait;
};

class HUD {
public:
	HUD();
	~HUD();

	RETCODE HUDLoad(hCFG cfg);	//load the HUD data from given config.

	void HUDResize();			//resize to current resolution
	
	RETCODE HUDUpdate();		//update HUD

	RETCODE HUDDisplay(const Id *pIDs, int numTaTa, int curTaTa); //display HUD with given Ta-Tas

	void HUDActive(bool bYes);	//activate/deactivate

	void HUDSetGoalTxt(hTXT txt);

	void HUDSetGoalMinFixed(int n); //set permanent minimum goal
	void HUDSetGoalMin(int n);
	void HUDSetGoalMax(int n);

	int HUDGetGoalMinFixed();
	int HUDGetGoalMin();
	int HUDGetGoalMax();

	bool HUDIsGoalComplete();		//we got all we want?

	void HUDSetSelect(const Id & EntityID); //set the select indicator to given entity
	void HUDRemoveSelect(); //clears up select indicator

private:
	hTXT	m_blankTxt;			//texture for blank portrait.
	hTXT	m_selectTxt;		//texture for current selected.
	hTXT	m_ouchTxt;			//texture for OUCH!
	hTXT	m_healthTxt;		//tetxure for health points.

	hTXT	m_goalTxt;			//goal texture, varies for each level
	
	hFNT	m_fnt;				//font used for various stuff
	float	m_fntRelSize;		//font relative size

	float	m_goalSX,m_goalSY;	//the goal img. start loc. (0~1)
	float	m_goalEX,m_goalEY;	//the goal img. end loc. (0~1)

	float	m_goalFNTX,m_goalFNTY;	//the goal font loc. (0~1)

	int		m_goalMinFixed, m_goalMin, m_goalMax;	//the goal fixed/min/max

	float	m_startX,m_startY;	//the start loc. (0~1)
	float	m_endX,m_endY;		//the end loc. (0~1)
	float	m_space;			//the space between portrait. (0~1)
	
	float	m_alphaCur;			//the current alpha value.
	float	m_alphaNext;		//the final alpha value.

	win32Time m_delay;			//used for fading.

	gfxID		m_selectOBJ;		//the select object on top of a Ta-Ta if selected
	win32Time	m_selectDelay;		//the select delay fade
	Move		m_selectRotSpd;		//the select speed of rotation
};

class Corpse {
public:
	Corpse(Creature *pCre, int deathState);
	~Corpse();

	bool Update(); //returns true if this corpse is ready to be deleted

private:
	hOBJ		m_obj;				//the dead.
	bool		m_bFading;			//are we fading and moving up?
	win32Time	m_delay;			//delay for fading and pause.
	Move		m_upSpd;			//the speed going up.
};

//used for Ta-Tas reaching the goal
class ExitObj {
public:
	ExitObj(hOBJ obj, int victoryState, int flyState);
	~ExitObj();

	bool Update(); //returns true if this obj is ready to be deleted

private:
	hOBJ		m_obj;				//the object exiting
	int			m_flyState;			//the state when flying up
	win32Time	m_delay;			//delay for fading and flying up
	Move		m_upSpd;			//the speed going up.
	Move		m_rotSpd;			//rotation speed while going up
};

class GameImage : public GameID {
public:
	enum {
		STATE_NORMAL,
		STATE_FADEIN,
		STATE_FADEOUT,
		STATE_DISABLE
	};

	GameImage(hTXT texture, float sX, float sY, float eX, float eY, BYTE r, BYTE g, BYTE b, BYTE a);
	~GameImage();

	//STATE_NORMAL,
	//STATE_FADEIN,
	//STATE_FADEOUT
	void SetState(int state);

	int  GetState() { return m_state; }

	hTXT GetTexture() { return m_txt; } //grabs texture (DO NOT DESTROY TEXTURE!!!)

	bool Update();	//returns true when ready to die.

	void Display();
private:
	hTXT		m_txt;					//the texture
	float		m_x,m_y;				//location (in pixels)
	float		m_w,m_h;				//image size (in pixels)
	DWORD		m_clr;					//the current color
	float		m_rS,m_gS,m_bS,m_aS;	//start color on delay.
	float		m_rE,m_gE,m_bE,m_aE;	//end color on delay.
	BYTE		m_r,m_g,m_b,m_a;		//the given rgba values from creation

	win32Time	m_delay;				//used for fading in/out

	int			m_state;				//current state: STATE_NORMAL, STATE_FADEIN, STATE_FADEOUT
};

//graphical dialog
class GameDialog : public GameID {
public:
	//sX,sY,eX,eY = relative [0,1]
	GameDialog(hTXT texture, float sX, float sY, float eX, float eY);
	~GameDialog();

	void Open();
	void Close();

	void Resize();		//resize to current resolution

	//size = relative [0,1] based on screen width
	void SetFont(const char *typeName, float size, unsigned int fmtFlag);
	void SetString(const char *str);
	void SetStringClr(BYTE r, BYTE g, BYTE b);

	void Update();

	void Display();

	bool IsTextComplete();	//check to see if all the text is displayed

private:
	hTXT	m_texture;		//the border texture
	hFNT	m_font;
	float   m_fntRelSize;	//font relative size to res.
	float	m_x, m_y;		//border upper-left
	float	m_w, m_h;		//border size

	DWORD	m_textClr;		//the text color

	string	m_string;		//created text
	string  m_stringDisplay;//text to display 

	int		m_state;		//0 = do nothing, 1 = close, 2 = open
	
	float	m_sCH, m_sCY;	//constant height and y

	win32Time	m_delay;	//the delay for opening/closing
	win32Time	m_delayStr;	//the delay for opening/closing

	int		m_txtEnd;		//ending string

	DWORD	m_borderClr;	//the border color
};

class AmbientSound : public GameID, public Name {
public:
	AmbientSound(const char *name, const char *filename, 
		const D3DXVECTOR3 & loc, 
		const D3DXVECTOR3 & orient, 
		float minLength, float length, bool bLoop);
	~AmbientSound();

	bool Update();				//returns true if ambient sound is done
								//playing (always false if looping)

private:
	BASS_3DVECTOR m_loc;			//location of sound
	BASS_3DVECTOR m_orient;			//orientation of sound
	HSAMPLE		m_sound;		//the ambient sound
	bool		m_bLoop;		//is ambient looping?
};

class World : public Flag, public View, public HUD, public Name {
public:
	World();
	~World();

	//
	// Profile
	//

	/////////////////////////////////////
	// Purpose:	create new player profile
	//			this will set default stuff
	// Output:	player profile loaded
	// Return:	success if success
	/////////////////////////////////////
	RETCODE ProfileNew(const char *name);

	/////////////////////////////////////
	// Purpose:	loads the player profile
	// Output:	player profile loaded
	// Return:	success if success
	/////////////////////////////////////
	RETCODE ProfileLoad(const char *name);
	
	/////////////////////////////////////
	// Purpose:	saves the profile to the
	//			save directory
	// Output:	profile saved
	// Return:	none
	/////////////////////////////////////
	void    ProfileSave();

	/////////////////////////////////////
	// Purpose:	loads all stuff from the
	//			profile to the world
	// Output:	Ta-Tas spawned
	// Return:	none
	/////////////////////////////////////
	void    ProfilePrepare();

	/////////////////////////////////////
	// Purpose:	use this to add/set stuff
	//			inside the player profile.
	//			DON'T DESTROY THE PROFILE!!!
	// Output:	none
	// Return:	profile config
	/////////////////////////////////////
	hCFG ProfileGetCfg();

	//
	// Map
	//

	/////////////////////////////////////
	// Purpose:	loads up the world including 
	//			creatures, objects...etc.
	// Output:	everything initialized
	// Return:	success if success
	/////////////////////////////////////
	RETCODE MapLoad(const char *filename);

	/////////////////////////////////////
	// Purpose:	reloads the map
	// Output:	everything re-initialized
	// Return:	success if success
	/////////////////////////////////////
	RETCODE MapReload();

	/////////////////////////////////////
	// Purpose:	post load the cloud map and
	//			set current Ta-Ta's location
	//			with given target.
	// Output:	cloud map will be loaded after
	//			the world update.
	// Return:	success
	/////////////////////////////////////
	RETCODE MapPostLoadCloud(const char *targetName, float angleRad, bool bCutsceneRemove);

	/////////////////////////////////////
	// Purpose:	post load given map and
	//			set current Ta-Ta's location
	//			with given target.
	// Output:	map will be loaded after
	//			the world update.
	// Return:	success
	/////////////////////////////////////
	RETCODE MapPostLoad(const char *filename, const char *targetName, float angleRad, bool bCutsceneRemove);

	/////////////////////////////////////
	// Purpose:	this will set the current
	//			map as the 'previous' on
	//			use of MapPrevLoad
	// Output:	prev. stuff set
	// Return:	none
	/////////////////////////////////////
	void MapSetPrev();

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
	RETCODE MapPrevLoad(bool bSetLoc, bool bSetDir, bool bCutsceneRemove);

	/////////////////////////////////////
	// Purpose:	get map description
	// Output:	none
	// Return:	description
	/////////////////////////////////////
	const char * MapGetDesc();

	//
	// Cutscene
	//

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
	RETCODE CutsceneSet(const char *filename, const char *section);
	
	//section inside the world scene file
	RETCODE CutsceneSet(const char *section);

	/////////////////////////////////////
	// Purpose:	remove current cutscene
	// Output:	cutscene removed and game
	//			returns to normal.
	// Return:	none
	/////////////////////////////////////
	void CutsceneRemove();

	//same as CutsceneSet, except for ending
	RETCODE CutsceneSetEnding(const char *filename, const char *section);
	RETCODE CutsceneSetEnding(const char *section);

	void CutscenePrepareEnding();	//prepares ending,
									//after ending scene is done,
									//the player is brougt back
									//to cloud land

	bool CutsceneUpdate();		//update cutscene (returns true if scene played)

	//
	// Main Functions
	//

	/////////////////////////////////////
	// Purpose:	Updates the world.
	// Output:	stuff happens
	// Return:	success if nothing went wrong
	/////////////////////////////////////
	RETCODE Update();

	/////////////////////////////////////
	// Purpose:	displays everything in the
	//			world.
	// Output:	stuff put on screen
	// Return:	success if success
	/////////////////////////////////////
	RETCODE Display();

	//
	// Object thing-a-magig stuff
	//

	/////////////////////////////////////
	// Purpose:	add a corpse to the world
	//			that will evaporate...or something
	// Output:	corpse added
	// Return:	none
	/////////////////////////////////////
	void AddCorpse(Creature *pCre, int deathState);

	/////////////////////////////////////
	// Purpose:	add an object ready for exit
	// Output:	exit object added
	// Return:	none
	/////////////////////////////////////
	void AddExitObj(hOBJ obj, int victoryState, int flyState);

	//
	// Graphical stuff
	//

	/////////////////////////////////////
	// Purpose:	add an image to be displayed
	//			on screen.
	//			Coordinates are in relative
	//			screen space. ie. values from [0,1]
	// Output:	image added
	// Return:	the ID of image display
	/////////////////////////////////////
	Id ImageDisplayAdd(hTXT texture, float sX, float sY, float eX, float eY, BYTE r, BYTE g, BYTE b, BYTE a);

	/////////////////////////////////////
	// Purpose:	fade-in/out given image
	// Output:	image will show up on screen
	// Return:	none
	/////////////////////////////////////
	void ImageDisplayFadeIn(const Id & ID, bool bIn);

	/////////////////////////////////////
	// Purpose:	add a dialog to be displayed
	//			coordinates are relative [0,1]
	// Output:	dialog added
	// Return:	the ID of dialog
	/////////////////////////////////////
	Id DialogAdd(hTXT texture, float sX, float sY, float eX, float eY);

	//resize dialogs based on current resolution
	void DialogResize();

	//
	// Sound stuff
	//

	/////////////////////////////////////
	// Purpose:	adds an ambient sound
	// Output:	ambient sound added and
	//			played
	// Return:	the ambient ID
	/////////////////////////////////////
	Id AmbientSndAdd(const char *name, const char *filename, 
		const D3DXVECTOR3 & loc, const D3DXVECTOR3 & orient, 
		float minLength, float length, bool bLoop);

	//
	// Target Functions
	//

	/////////////////////////////////////
	// Purpose:	add a target with given name(ID)
	// Output:	new target appended
	// Return:	none
	/////////////////////////////////////
	void TargetAdd(const char *name, const D3DXVECTOR3 & loc);

	/////////////////////////////////////
	// Purpose:	remove a target with given name(ID)
	// Output:	target removed
	// Return:	none
	/////////////////////////////////////
	void TargetRemove(const char *name);

	/////////////////////////////////////
	// Purpose:	get the location of target
	// Output:	locOut
	// Return:	true if target found
	/////////////////////////////////////
	bool TargetGet(const char *name, D3DXVECTOR3 & locOut);

	/////////////////////////////////////
	// Purpose:	set the location of target
	// Output:	target set if found
	// Return:	true if target found and set
	/////////////////////////////////////
	bool TargetSet(const char *name, D3DXVECTOR3 & newLoc);

	//
	// Waypoint functions
	//

	/////////////////////////////////////
	// Purpose:	grab the waypoint
	// Output:	none
	// Return:	the Id
	/////////////////////////////////////
	Id WaypointGet(const char *name);

	//Callback for loading entities inside the map
	friend void WorldEntityParse(char *entityStr, hQBSP qbsp, void *userDat);

	//
	// Ta-Ta interfaces
	//

	/////////////////////////////////////
	// Purpose:	TataAddRoster
	// Output:	add a Ta-Ta type to roster
	// Return:	true if added, false if
	//			roster is full
	/////////////////////////////////////
	bool TataAddToRoster(const char *tataTypeName);

	/////////////////////////////////////
	// Purpose:	TataRetrieveFromRoster
	// Output:	retrieves an available Ta-Ta
	//			in the roster, this will get
	//			the first Ta-Ta added
	//			given string is filled
	// Return:	true if string is filled,
	//			false if roster is empty
	/////////////////////////////////////
	bool TataRetrieveFromRoster(string & strOut);

	/////////////////////////////////////
	// Purpose:	remove given Ta-Ta from
	//			the roster
	// Output:	roster updated
	// Return:	true if removed, false if
	//			Ta-Ta is not in roster.
	/////////////////////////////////////
	bool TataRemoveFromRoster(const string & tataTypeName);

	/////////////////////////////////////
	// Purpose:	check to see if this Ta-Ta
	//			is in roster.
	// Output:	none
	// Return:	true if in roster
	/////////////////////////////////////
	bool TataInRoster(const string & tataTypeName);

	/////////////////////////////////////
	// Purpose:	check to see if play list
	//			is full.
	// Output:	none
	// Return:	true if full.
	/////////////////////////////////////
	bool TataIsPlayListFull();

	/////////////////////////////////////
	// Purpose:	check to see if play list
	//			is empty.
	// Output:	none
	// Return:	true if empty.
	/////////////////////////////////////
	bool TataIsPlayListEmpty();

	/////////////////////////////////////
	// Purpose:	add Ta-Ta to play list if
	//			a slot is available
	// Output:	creature added
	// Return:	true if added, false if 
	//			play list is full
	/////////////////////////////////////
	bool TataAddToPlayList(Creature *cre);

	/////////////////////////////////////
	// Purpose:	set the currently selected
	//			Ta-Ta to previous in play
	//			list
	// Output:	currently selected Ta-Ta set
	// Return:	none
	/////////////////////////////////////
	void TataSetCurrentPrev();

	/////////////////////////////////////
	// Purpose:	set the currently selected
	//			Ta-Ta to next in play
	//			list
	// Output:	currently selected Ta-Ta set
	// Return:	none
	/////////////////////////////////////
	void TataSetCurrentNext();

	/////////////////////////////////////
	// Purpose:	check to see if the given
	//			Ta-Ta type name is on the
	//			playlist.
	// Output:	none
	// Return:	true if found, false if not
	//			in playlist.
	/////////////////////////////////////
	bool TataOnPlayList(const char *tataTypeName);

	/////////////////////////////////////
	// Purpose:	get the current Ta-Ta the
	//			player is controlling.
	// Output:	none
	// Return:	the Ta-Ta ID.
	/////////////////////////////////////
	Id	TataGetCurrent();

	/////////////////////////////////////
	// Purpose:	get a Ta-Ta in the play
	//			list.
	// Output:	none
	// Return:	the Ta-Ta ID.
	/////////////////////////////////////
	Id TataGet(int ind);

	/////////////////////////////////////
	// Purpose:	change the current Ta-Ta
	//			being controlled.
	//			NOTE: this function ASSUMES
	//			that the given ID is a Ta-Ta!
	// Output:	set current Ta-Ta
	// Return:	none
	/////////////////////////////////////
	void TataChangeCurrent(const Id & ID);

	/////////////////////////////////////
	// Purpose:	spawn a Ta-Ta with given
	//			location.
	// Output:	a Ta-Ta spawned.
	// Return:	new Ta-Ta, NULL if something 
	//			bad happened
	/////////////////////////////////////
	Creature *TataSpawn(const char *tataTypeName, const D3DXVECTOR3 & loc,
		const char *overrideDatType = 0);

	/////////////////////////////////////
	// Purpose:	destroys all Ta-Ta in the
	//			world.  Also includes the
	//			Ta-Ta in the playlist.
	// Output:	all Ta-Tas gone...
	// Return:	none
	/////////////////////////////////////
	void TataDestroyAll();

	//
	// View functions
	//

	/////////////////////////////////////
	// Purpose:	set the default view dir.
	// Output:	default dir. set
	// Return:	none
	/////////////////////////////////////
	void ViewMainSetDefaultDir(const D3DXVECTOR3 & dir) { m_viewDirDefault = dir; }

	/////////////////////////////////////
	// Purpose:	get the default view dir.
	// Output:	none
	// Return:	default dir.
	/////////////////////////////////////
	D3DXVECTOR3 ViewMainSetDefaultDir() { return m_viewDirDefault; }

	/////////////////////////////////////
	// Purpose:	set the view dir. to default
	// Output:	view dir. set
	// Return:	none
	/////////////////////////////////////
	void ViewMainSetDirToDefault() { m_viewDir = m_viewDirDefault; }

	/////////////////////////////////////
	// Purpose:	set view direction from
	//			player.  NORMALIZED!!!
	// Output:	view dir. set
	// Return:	none
	/////////////////////////////////////
	void ViewMainSetDir(const D3DXVECTOR3 & dir) { m_viewDir = dir; }

	/////////////////////////////////////
	// Purpose:	get view direction from
	//			player.
	// Output:	none
	// Return:	direction
	/////////////////////////////////////
	D3DXVECTOR3 ViewMainGetDir() { return m_viewDir; }

	/////////////////////////////////////
	// Purpose:	ViewMainSetCtrlMove
	// Output:	set move
	// Return:	none
	/////////////////////////////////////
	void ViewMainSetCtrlMove(const Move & mov) { m_viewControlSpd = mov; }

	/////////////////////////////////////
	// Purpose:	set the distance of view
	//			from player
	// Output:	distance set
	// Return:	none
	/////////////////////////////////////
	void ViewMainSetDist(float d) { m_viewDist = d; }

	/////////////////////////////////////
	// Purpose:	get the distance of view
	//			from player
	// Output:	none
	// Return:	distance
	/////////////////////////////////////
	float ViewMainGetDist() { return m_viewDist; }

	//
	// Music Functions
	//

	/////////////////////////////////////
	// Purpose:	set the background music
	//			this will also play the
	//			new music
	// Output:	destroys the old music and
	//			sets the new one
	// Return:	none
	/////////////////////////////////////
	void MusicSet(const char *filename);

	/////////////////////////////////////
	// Purpose:	play the music or not
	//			NOTE: music is always played
	//			in loop.
	// Output:	music is played or stop
	// Return:	none
	/////////////////////////////////////
	void MusicPlay(bool bYes);

	//frees up music
	void MusicFree();

	//
	// Misc. Functions
	//
	hQBSP GetMap();

	void ResourceAdd(eResourceType type, short val);

	void SetStageLevel(int stage, int level);

	Clock GetClock() { return m_clock; }

	void SetArTaFactMin(int i) { m_artafactMin = i; }
	int  GetArTaFactMin() { return m_artafactMin; }

	void SetArTaFactMax(int i) { m_artafactMax = i; }
	int  GetArTaFactMax() { return m_artafactMax; }

protected:
	//entity loaders
	RETCODE EntityLoad_Start(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Waypoint(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_TaTa(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Enemy(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Item(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Light(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Block(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Lever(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Button(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Platform(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Steak(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Sign(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_OuchField(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Target(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_Doodad(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_Trigger(hQBSP qbsp, const EntityParse & entityDat);
	RETCODE EntityLoad_Goal(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_Turret(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_Geyser(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_Teleport(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_Lightning(hQBSP qbsp, const EntityParse & entityDat); //found in misc
	RETCODE EntityLoad_AmbientSound(hQBSP qbsp, const EntityParse & entityDat); //found in misc

	//This will load all the entities in the global entity loaded from map
	void WorldEntityLoadAll();

	//loads up some common data for the object
	RETCODE EntityLoad_CommonObject(hQBSP qbsp, const EntityParse & entityDat, Object *pObj);

private:
	//do not allow this
	World(const World & other) {}
	World & operator =(const World & other) { return *this; }

	//
	// Internal functions
	//

	void WorldUnload();					//unload everything...creatures, objects...etc.

	void WorldPostLoadExec();			//load up the post map if flag: WORLD_FLAG_POST_LOAD is set

	void _CutsceneDestroy();			//destroys all related cutscene stuff

	////////////////////////////////////////////
	//Main
	string	    m_filePath;				//the file path of the map
	hQBSP	    m_map;					//the map
	
	hSCRIPT	    m_scene;				//'Cutscene', this is updated when not NULL
	hSCRIPT	    m_nextScene;			//this is the next scene to be played after m_scene (used by SetWorldScene)
	hSCRIPT	    m_endScene;				//this is the ending scene for the level

	hCFG	    m_profile;				//the profile of player

	HSTREAM		m_music;				//the music background
	string		m_musicPath;			//the file path of the music

	Move		m_viewControlSpd;		//view control speed (when looking around)
	D3DXVECTOR3 m_viewDir;				//view direction from player
	D3DXVECTOR3 m_viewDirDefault;		//view default direction from player
	float	    m_viewDist;				//view distance from player
	float		m_viewRotX,m_viewRotY;	//view rotation (radians)

	int			m_stage,m_level;		//current stage-level

	short		m_resources[RC_MAX];	//accumulated resources

	Clock		m_clock;					//the time player is taking

	int			m_artafactMin,m_artafactMax;

	////////////////////////////////////////////
	//TaTa's
	Id				 m_TaTa[MAXTATAWORLD];	//the tata's
	list<TaTaRoster> m_TaTaRoster;			//The Ta-Ta roster (for selection)
	int				 m_curTaTa;				//current tata selected

	//list of crap
	list<Corpse *>	 m_corpses;				//list of dead things
	list<ExitObj *>	 m_exitObjs;			//list of objects exiting

	list<Id>         m_images;				//list of images to display. (GameImage)
	list<Id>         m_dialogs;				//list of dialogs to display. (GameDialog)
	list<Id>         m_ambients;			//list of ambient sounds to update. (AmbientSound)

	////////////////////////////////////////////
	//Waypoints
	vector<Waypoint *> m_waypts;		//lists of waypoint paths

	////////////////////////////////////////////
	//Targets
	struct Target {
		string		ID;		//ID name
		D3DXVECTOR3	loc;	//Target location
	};

	list<Target>	   m_targets;		//target list

	string				m_desc;				//the level description

	////////////////////////////////////////////
	//post map load data
	//these stuff will be used when the flag: WORLD_FLAG_POST_LOAD is set
	string				m_postMapFilename;
	string				m_postTarget;
	float				m_postAngle;
	bool				m_postbCutsceneRemove;

	////////////////////////////////////////////
	//prev. map data
	string				m_prevFilePath;		//'none' for no previous (only set by MapSetPrev())
	D3DXVECTOR3			m_prevLoc;			//(only set by MapSetPrev())
	D3DXVECTOR3			m_prevDir;			//(only set by MapSetPrev())
};

#endif