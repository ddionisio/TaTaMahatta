#ifndef _tata_types_h
#define _tata_types_h

#include "common.h"

//other STL stuff
#include <queue>

#include "mywin.h"

#include <time.h>

#include "graphicsX\gdiX.h"
#include "graphicsX\gdix_particlefx.h"
#include "graphicsX\gdix_bkgrndfx.h"
#include "Parser\parser.h"

#include "ScriptTease\script.h"
#include "ScriptTease\cfg.h"

#include "inputx\inputx.h"
#include "timer\timer.h"

#include "bass.h"

//disable these stupid warnings!
#pragma warning ( disable : 4355 ) //'this' : used in base member initializer list

//constants!
#define WINNAME			"Ta-Ta Mahatta" //windows class name

#define SCRN_W			640	//default screen width
#define SCRN_H			480 //default screen height

#define MAXRES	8

#define JOYSTICK_MIN	-1000
#define JOYSTICK_MAX	1000

#define INPUTCOOPLVL	(DISCL_EXCLUSIVE | DISCL_FOREGROUND)

#define GAMEFOLDER		"Main"
#define MSAVEFOLDER		"TaTaMahatta"
#define SAVEFOLDER		"Save"
#define GAMECFGFILE		"Main\\game.cfg"

//game config sections
#define GAME_SECTION	"game"

//profile sections
#define PROFILE_COLLECT_SECTION "collection"

#define SCENE_EXT		".scn"
#define SAVE_EXT		".ini"

#define GAME_VIEW_DELAY 500

#define GAME_LIGHT_MAX	3

#define MAXTATAWORLD	4	//max ta-ta in one world

#define MAX_SAMPLE_PLAY	1

//Flags

//Game flags

//command lines
#define GAME_FLAG_SETUP					1	//call setup thingy
#define GAME_FLAG_MAP					2	//load a map
#define GAME_FLAG_SKIPINTRO				4	//skip intro
#define GAME_FLAG_QUIT					8	//get the hell out
#define GAME_FLAG_NOSOUND				16	//no sound and music?

//World flags
#define WORLD_FLAG_CLOUDLEVEL			1	//are we in the cloud level
#define WORLD_FLAG_PAUSE				2	//no update
#define WORLD_FLAG_INPUT_DISABLE		4	//no input
#define WORLD_FLAG_RELOAD_GAME			8	//reload the proc. game
#define WORLD_FLAG_POST_LOAD			16	//load the posted map after update
#define WORLD_FLAG_GOAL_HOME			32  //special flag for goal home
#define WORLD_FLAG_ENDING				64	//is ending scene being played?
#define WORLD_FLAG_CREDITS				128	//are we gonna go to credits? 
											//if so, then go! (saves game)

//Entity flags
#define ENTITY_FLAG_NOWORLDCOLLISION	1	//don't collide with world

#define ENTITY_FLAG_NONSOLID			2   //if this is set, then this
											//entity will be ignored by
											//solid entities on collision.
											//But this entity can collide
											//with other entities.
											//Non-Solid entites cannot
											//collide with each other...

#define ENTITY_FLAG_POLLDEATH			4	//We are going to die on the next
											//update.

#define ENTITY_FLAG_PULLABLE			8	//entity can be pulled

#define ENTITY_FLAG_NONSOLID_COLLIDE	16	//if this is set, non-solid
											//can collide with another 
											//non-solid
#define ENTITY_FLAG_SHADOW				32	//entity casts shadow

#define ENTITY_FLAG_GOAL				64	//entity is on goal

//Creature flags
#define CRE_FLAG_NOFREEZE				128	 //this creature cannot be frozen
#define CRE_FLAG_GASIMMUNE				256  //immune to gas
#define CRE_FLAG_STUNIMMUNE				512  //immune to stun
#define CRE_FLAG_MELEEIMMUNE			1024 //immune to melee attacks
#define CRE_FLAG_SPITIMMUNE				2048 //immune to spit attacks
#define CRE_FLAG_INVUL					4096 //invulnerable
#define CRE_FLAG_TAG					8192 //creature is tagged,
											 //tag flag is removed after
											 //creature has reached a waypoint node

//Ta-Ta flags
#define TATA_FLAG_RUN					16384
#define TATA_FLAG_BACKWALK				32768  //Ta-Ta will backwalk
#define TATA_FLAG_DISABLE_ROTATE		65536  //Ta-Ta cannot rotate
#define TATA_FLAG_ENEMY_IGNORE			131072 //Ta-Ta will be ignored by enemies

//Projectile flags
#define PROJ_FLAG_PUSHBUTTON			128	//Projectile can push buttons
#define PROJ_FLAG_OBJINVUL				256	//Projectile cannot be destroyed 
											//by object...unless specified

//Level Object Flags (Object)
#define OBJ_FLAG_PUSHABLE		128		//can this object be pushed (as button or block)
#define OBJ_FLAG_ON				256		//is object in 'on' switch. (used by levers...etc)
#define OBJ_FLAG_ONCE_ONLY		512		//object can only be switched once
#define OBJ_FLAG_DOOR			1024	//is this object a door?  Only theif can activate doors
#define OBJ_FLAG_INACTIVE		2048	//is object in-active (cannot run script and waypoint)

//return codes
#define RETCODE_STATUS_DONE		10001	//status is done, ready to be deleted
#define RETCODE_STATUS_NOUPDATE	10002	//status says there should be no further update

//enums

//Game Procs/States
typedef enum {
	PROC_NULL,			//Initial proc...change it real quick!
						//NEVER CHANGE TO THIS
	PROC_INTRO,			//The introduction, digipen, 2d...etc.
	PROC_GAME,			//Actual Game, includes: main menu, cloud level, all levels
	PROC_CREDITS,		//The game credits
	PROC_MAX
} eGameProcType;

//Game Procs Messages
typedef enum {
	GMSG_INIT,			//inclusive data proc initialization
	GMSG_UPDATE,		//game update for proc (includes input)
	GMSG_DISPLAY,		//game display everything
	GMSG_DESTROY		//inclusive data proc destruction
} eGameMsg;

//Game Input Types
typedef enum {
	INP_UP,
	INP_DOWN,
	INP_LEFT,
	INP_RIGHT,
	INP_A,		//OK, Use Ability.
	INP_B,		//NO, Hold for Free-Look using Direction pad.
	INP_C,		//Switch Ta-ta  Forward in List.
	INP_D,		//Switch Ta-ta, Back in List.
	INP_E,		//Strafe Button, Hold for strafing using left/right key
	INP_START,
	INP_MAX,
	INP_MAX_BTN = 6
} eGameInput;

//Game Input Status (used in Ta-Ta action)
typedef enum {
	INP_STATE_UP,
	INP_STATE_DOWN,
	INP_STATE_RELEASED
} eGameInputState;

//Waypoint Modes
typedef enum {
	WAYPT_STOPATEND,	//stop once end reached
	WAYPT_REPEAT,		//repeat to beginning
	WAYPT_BACKTRACE,	//go through backwards
	WAYPT_MAX
} eWayPtMode;

//Move Types
typedef enum {
	MOVE_FALL,			//we are falling down
	MOVE_NONE,			//hit wall or ground.
	MOVE_WALK,			//we are walking or something
} eMoveType;

//General animation states for all Ta-Tas
typedef enum {
	CREANIM_IDLE1,
	CREANIM_IDLE2,
	CREANIM_IDLE3,
	CREANIM_IDLE_MAX = CREANIM_IDLE3,
	CREANIM_WALK,
	CREANIM_BACKWALK,
	CREANIM_FALL,
	CREANIM_OUCH,
	CREANIM_DEATH,
	CREANIM_VICTORY,
	CREANIM_ACTION,
	CREANIM_MAX			//maximum general animation state
} eCreAnim;

//Creature status
typedef enum {
	CRESTATUS_NORMAL,
	CRESTATUS_INVULNERABLE,
	CRESTATUS_TELEPORT,
	CRESTATUS_STUN,
	CRESTATUS_MAX
} eCreStatus;

//Entity types (used for collision insertion and world insertion)
//The order they appear here is the order they are in the list
typedef enum {
	ENTITY_TYPE_PROJECTILE,
	ENTITY_TYPE_ITEM,
	ENTITY_TYPE_OBJECT,
	ENTITY_TYPE_TATA,
	ENTITY_TYPE_ENEMY,
	ENTITY_TYPE_MAXTYPE,
	ENTITY_TYPE_INVALID
} eEntityType;

//Ta-Ta's
typedef enum {
	TATA_CAPTAINTATA,
	TATA_TATATUG,
	TATA_TINKERTA,
	TATA_TATATRAMPLE,
	TATA_HOPTAHOP,
	TATA_FROSTTA,
	TATA_PARATATA,
	TATA_LOOPTALOOP,
	TATA_KEYKEYTA,
	TATA_CHITA,
	TATA_TAVATAR,

	TATA_BABYTATA,		//NPC Ta-Ta for rescuing
	TATA_MAX
} eTaTaType;

//Enemies
typedef enum {
	ENEMY_FUNGABARK,
	ENEMY_SHROOMGUARD,
	ENEMY_SHROOMSHOOTER,
	ENEMY_FUNGASPY,
	ENEMY_FUNGASMUG,
	ENEMY_FUNGAMUSKETEER,

	BOSS_SHROOMPA,
	BOSS_CATTERSHROOM,
	BOSS_CORRUPTA,

	ENEMY_MAX
} eEnemyType;

//Level Object's
//The order they appear here is the order they are in the list
typedef enum {
	OBJECT_PLATFORM,
	OBJECT_BUTTON,
	OBJECT_BLOCK,
	OBJECT_LEVER,
	OBJECT_STEAK,
	OBJECT_SIGN,
	OBJECT_DOODAD,		//these are not added to the collision list,
						//nobody collides with these crap
	OBJECT_TRIGGER,
	OBJECT_GOAL,

	OBJECT_TURRET,
	OBJECT_OUCHFIELD,
	OBJECT_GEYSER,
	OBJECT_TELEPORT,

	OBJECT_LIGHTNING,	//super ultra pop lightning hit baby!

	OBJECT_MAX
} eObjectType;

//Projectile's
typedef enum {
	PROJ_PUSHMELEE,		//push melee used by Ta-Ta Tug
	PROJ_USEMELEE,		//use melee used by Tinker Ta
	PROJ_OPENMELEE,
	PROJ_ATTACKMELEE,	//attack melee used by Ta-Ta Trample
	PROJ_SPIT,			//a simple horizontal travelling spitwad.
	PROJ_SCANNER,		//only used by Funga Spy
	PROJ_WHIP,			//only used by Loop-Ta-Loop
	PROJ_FROST,			//a menacing frost attack freezing enemies
	PROJ_PULLPOWER,		//only used by Chi-Ta
	PROJ_GAS,			//gas attack
	PROJ_TELEPORT,		//teleportation
	PROJ_SPIKE,			//spike attack
	PROJ_MAX
} eProjType;

//Item's
typedef enum {
	ITM_RESOURCE,
	ITM_EGG,
	ITM_COLLECTION,
	ITM_MAX
} eItmType;

typedef enum {
	RC_LOVE,
	RC_CARE,
	RC_MAX
} eResourceType;

//sound requests
typedef enum {
	SND_REQ_PROJ_HIT_WALL,	//projectile hits a wall
	SND_REQ_PROJ_HIT_CRE,	//projectile hits a creature
	SND_REQ_PROJ_TRAVEL,	//projectile sound while traveling
	SND_REQ_PROJ_HIT_STAKE, //projectile hits a stake (used by whip)
	SND_REQ_PROJ_MAX
} eSndReqType;

//message callback used by all entities
typedef enum {
	ENTITYMSG_UPDATE,
	ENTITYMSG_ALLOWGRAVITY,		//return 1 if you want the entity 
								//to fall down, 0 if not.
	ENTITYMSG_WORLDCOLLIDE,		//wParam = hQBSP, lParam = gfxTrace *
	ENTITYMSG_ENTITYCOLLIDE,	//wParam = EntityCommon *, lParam = gfxTrace *
	ENTITYMSG_ENTITYNOCOLLIDE,	//wParam = EntityCommon *
	ENTITYMSG_DEATH,			//any last words?  Do not use this as destructor!
	ENTITYMSG_CFGLOAD,			//wParam = hCFG
	ENTITYMSG_PROFILEPREP,		//profile prepare wParam = 0 or 1 (enable)
	ENTITYMSG_CHANGEBEHAVIOR,	//wParam = int state
	ENTITYMSG_REQUESTSOUND,		//wParam = int requestType (type of request)
								//lParam = int * (fill this up)
	
	ENTITYMSG_OTHERSTART	//(DON'T USE THIS)
} eEntityMsg;

//message callback used by creatures
typedef enum {
	CREMSG_ACTION = ENTITYMSG_OTHERSTART,    //wParam = eGameInputState
	CREMSG_HIT								 //wParam = amt
} eCreMsg;

//message callback used by objects

//message callback used by projectiles

//message callback used by items
typedef enum {
	ITMMSG_COLLECT = ENTITYMSG_OTHERSTART    //what to do when this item is collected
} eItmMsg;

//function pointer defines
typedef RETCODE (* GAMEPROC) (DWORD msg, WPARAM wParam, LPARAM lParam);

//macros

//NOTE: DO NOT MIX WITH C++ STUFF, IE.: DON'T USE IT TO ALLOCATE/FREE CLASSES!!!!
#define GAME_MALLOC(s) calloc(1, (s))//malloc((s))
#define GAME_FREE(data) free((data))

#define GAME_IDVALID(id) ((id).counter != -1) //is ID valid
#define GAME_IDISEQUAL(id1, id2) ((id1).ID == (id2).ID && (id1).counter == (id2).counter)

#define GAME_GETSIGN(val) ((val) < 0 ? -1 : 1)

#define GAME_ABS(val) ((val) < 0 ? -(val) : (val))

/********************************************************************
*																	*
*							Structures								*
*																	*
********************************************************************/

//general ID used by all objects
typedef struct _Id {
	DWORD ID;
	LONGLONG counter; //-1 if Id invalid
} Id;

/********************************************************************
*																	*
*						Procs/Game States							*
*				   (DONT CALL THESE FUNCTIONS!!)					*
*																	*
********************************************************************/
PUBLIC RETCODE IntroProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PUBLIC RETCODE GameProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PUBLIC RETCODE CreditProc(DWORD msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam);

//parser callback for commandline
void CmdLineCallback(const char *cmdline, void *userData);

//This is used for all particles that collide with the world
//Function is defined in tata_tool.cpp
bool ParticleCollisionCallback(float pt1[eMaxPt], float pt2[eMaxPt], float *tOut, float nOut[eMaxPt]);

#endif
