#ifndef _tata_menu_game_h
#define _tata_menu_game_h

//These are IDs used by all menus in the game

////////////////////////////////////////////
//Main Menu
#define	MAINMENU_TITLE		0
#define MAINMENU_NEW		1
#define MAINMENU_LOAD		2
#define MAINMENU_OPTIONS	3
#define MAINMENU_CREDITS	4
#define MAINMENU_EXIT		5

////////////////////////////////////////////
//Pause Cloud
#define	PAUSECLOUD_TITLE			0
#define PAUSECLOUD_RESUME			1
#define PAUSECLOUD_OPTIONS			2
#define PAUSECLOUD_RETURNMENU		3

////////////////////////////////////////////
//Level Select
#define LEVELSELECT_LVLTXT			1000
#define LEVELSELECT_BESTTIME		2000
#define LEVELSELECT_ARTAFACTS		3000

#define LEVELSELECT_OK				1
#define LEVELSELECT_CANCEL			2

////////////////////////////////////////////
//Pause In-Game
#define	PAUSEINGAME_TITLE			0
#define PAUSEINGAME_RESUME			1
#define PAUSEINGAME_OPTIONS			2
#define PAUSEINGAME_RESTART			3
#define PAUSEINGAME_RETURNCLOUD		4

////////////////////////////////////////////
//Game Over
#define	GAMEOVER_TITLE				0
#define GAMEOVER_RESTART			1
#define GAMEOVER_RETURNCLOUD		2

////////////////////////////////////////////
//Victory
#define	VICTORY_TIME				1000
#define VICTORY_ARTAFACT			2000

////////////////////////////////////////////
//New
#define NEW_A						1001
#define NEW_B						1002
#define NEW_C						1003
#define NEW_D						1004
#define NEW_E						1005
#define NEW_F						1006
#define NEW_G						1007
#define NEW_H						1008
#define NEW_I						1009
#define NEW_J						1010
#define NEW_K						1011
#define NEW_L						1012
#define NEW_M						1013
#define NEW_N						1014
#define NEW_O						1015
#define NEW_P						1016
#define NEW_Q						1017
#define NEW_R						1018
#define NEW_S						1019
#define NEW_T						1020
#define NEW_U						1021
#define NEW_V						1022
#define NEW_W						1023
#define NEW_X						1024
#define NEW_Y						1025
#define NEW_Z						1026
#define NEW_DEL						1027
#define NEW_DONE					1028
#define NEW_CANCEL					1029
#define NEW_TEXT					1030

////////////////////////////////////////////
//Load
#define LOAD_CONFIRM				2
#define LOAD_CANCEL					3

////////////////////////////////////////////
//Options
#define OPTIONS_TITLE				0

#define OPTIONS_GRAPHIC				1000
#define OPTIONS_AUDIO				2000
#define OPTIONS_CONTROL				3000
#define OPTIONS_CFG_SAVE			4000
#define OPTIONS_CFG_NOSAVE			5000

#define OPTIONS_BACK				666

//gfx
#define OPTIONS_RESOLUTION			1
#define OPTIONS_RESOLUTION_TEXT		1002
#define OPTIONS_RESOLUTION_APPLY	2

#define OPTIONS_LIGHTING			3
#define OPTIONS_LIGHTING_TEXT		3002

//audio
#define OPTIONS_SOUND				1
#define OPTIONS_SOUND_SCALE			1001
#define OPTIONS_SOUND_BAR			1002

#define OPTIONS_MUSIC				2
#define OPTIONS_MUSIC_SCALE			2001
#define OPTIONS_MUSIC_BAR			2002

//input
#define OPTIONS_JOYSTICK			1
#define OPTIONS_JOYSTICK_LR			1001
#define OPTIONS_JOYSTICK_TEXT		1002

#define OPTIONS_CFG_KEYBOARD		2
#define OPTIONS_CFG_JOYSTICK		3

////////////////////////////////////////////
//Keyboard Config
#define KB_CFG_TITLE				0
#define KB_CFG_UP					1
#define KB_CFG_UP_STR				1001
#define KB_CFG_DOWN					2
#define KB_CFG_DOWN_STR				2001
#define KB_CFG_LEFT					3
#define KB_CFG_LEFT_STR				3001
#define KB_CFG_RIGHT				4
#define KB_CFG_RIGHT_STR			4001
#define KB_CFG_A					5
#define KB_CFG_A_STR				5001
#define KB_CFG_B					6
#define KB_CFG_B_STR				6001
#define KB_CFG_C					7
#define KB_CFG_C_STR				7001
#define KB_CFG_D					8
#define KB_CFG_D_STR				8001
#define KB_CFG_E					9
#define KB_CFG_E_STR				9001
#define KB_CFG_START				10
#define KB_CFG_START_STR			10001
#define KB_CFG_OK					11
#define KB_CFG_DEFAULT				12
#define KB_CFG_CANCEL				13

//////////////////////////////////////////////
//Joystick Config
#define JSTICK_CFG_A					1
#define JSTICK_CFG_A_STR				1001
#define JSTICK_CFG_B					2
#define JSTICK_CFG_B_STR				2001
#define JSTICK_CFG_C					3
#define JSTICK_CFG_C_STR				3001
#define JSTICK_CFG_D					4
#define JSTICK_CFG_D_STR				4001
#define JSTICK_CFG_E					5
#define JSTICK_CFG_E_STR				5001
#define JSTICK_CFG_START				6
#define JSTICK_CFG_START_STR			6001
#define JSTICK_CFG_OK					7
#define JSTICK_CFG_DEFAULT				8
#define JSTICK_CFG_CANCEL				9

//Play Tutorial
#define PLAYTUTORIAL_YES				1
#define PLAYTUTORIAL_NO					2

//exit
#define EXIT_RETURN_TO_GAME				1
#define EXIT_SAVE_RETURN_MENU			2	//save and return to menu
#define EXIT_SAVE_QUIT_GAME				3	//save and exit game

//The game menu types
typedef enum {
	GAMEMENU_MAINMENU,
	GAMEMENU_PAUSECLOUD,
	GAMEMENU_LEVELSELECT,
	GAMEMENU_PAUSEINGAME,
	GAMEMENU_GAMEOVER,
	GAMEMENU_VICTORY,
	GAMEMENU_LEVELSTART,	//when starting a level
	GAMEMENU_NEW,
	GAMEMENU_LOAD,
	GAMEMENU_OPTIONS,
	GAMEMENU_OPTIONS_GFX,
	GAMEMENU_OPTIONS_AUDIO,
	GAMEMENU_OPTIONS_INPUT,
	GAMEMENU_KB_CFG,
	GAMEMENU_JSTICK_CFG,
	GAMEMENU_PLAYTUTORIAL,
	GAMEMENU_EXIT,
	GAMEMENU_MAX
} eGameMenuType;

//These are the callbacks used by the game (MCB = Menu Callback)

//Main Menu
RETCODE MCB_MainMenu(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Pause in Cloud Level
RETCODE MCB_PauseCloud(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Level Select
RETCODE MCB_LevelSelect(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Pause in Game
RETCODE MCB_PauseInGame(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Gameover
RETCODE MCB_GameOver(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Victory
RETCODE MCB_Victory(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Level Start
RETCODE MCB_LevelStart(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//New Game
RETCODE MCB_New(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Load Game
RETCODE MCB_Load(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Options
RETCODE MCB_Options(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

RETCODE MCB_Options_GFX(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);
RETCODE MCB_Options_Audio(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);
RETCODE MCB_Options_Input(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Keyboard Config
RETCODE MCB_Kb_Cfg(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Joystick Config
RETCODE MCB_Jstick_Cfg(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Tutorial Stuff
RETCODE MCB_PlayTutorial(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

//Exit
RETCODE MCB_Exit(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam);

/********************************************************************
*																	*
*							Functions								*
*																	*
********************************************************************/

/////////////////////////////////////
// Name:	MenuGameGetType
// Purpose:	get the menu type from given
//			name.
// Output:	none
// Return:	(eGameMenuType), -1 if no
//			match.
/////////////////////////////////////
PUBLIC int MenuGameGetType(const char *name);

/////////////////////////////////////
// Name:	MenuGameAdd
// Purpose:	add a menu specified by
//			type.  Use this instead of
//			MenuAdd!
// Output:	menu added
// Return:	see MenuAdd
/////////////////////////////////////
PUBLIC RETCODE MenuGameAdd(eGameMenuType type);

#endif