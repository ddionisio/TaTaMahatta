#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

//game related menu stuff

char *g_menuStr[GAMEMENU_MAX] = {"menu_main", "menu_pause_cloud",
"menu_level_select", "menu_pause_ingame", "menu_gameover",
"menu_victory", "menu_level_start", "menu_new", "menu_load",
"menu_options", "menu_options_gfx", "menu_options_audio",
"menu_options_input", "menu_kb_cfg", "menu_jstick_cfg",
"menu_playtutorial", "menu_exit"};

MENUPROC g_menuProc[GAMEMENU_MAX]={MCB_MainMenu, MCB_PauseCloud,
MCB_LevelSelect, MCB_PauseInGame, MCB_GameOver, MCB_Victory,
MCB_LevelStart, MCB_New, MCB_Load, MCB_Options, MCB_Options_GFX,
MCB_Options_Audio, MCB_Options_Input, MCB_Kb_Cfg, MCB_Jstick_Cfg,
MCB_PlayTutorial, MCB_Exit};

/////////////////////////////////////
// Name:	MenuGameGetType
// Purpose:	get the menu type from given
//			name.
// Output:	none
// Return:	(eGameMenuType), -1 if no
//			match.
/////////////////////////////////////
PUBLIC int MenuGameGetType(const char *name)
{
	for(int i = 0; i < GAMEMENU_MAX; i++)
	{
		if(stricmp(g_menuStr[i], name) == 0)
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	MenuGameAdd
// Purpose:	add a menu specified by
//			type.  Use this instead of
//			MenuAdd!
// Output:	menu added
// Return:	see MenuAdd
/////////////////////////////////////
PUBLIC RETCODE MenuGameAdd(eGameMenuType type)
{
	return MenuAdd(TataDataGetFilePath(g_menuStr[type]), g_menuProc[type]);
}