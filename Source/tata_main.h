#ifndef _tata_main_h
#define _tata_main_h

#include "tata_types.h"

//General functions used by the game

//
// Error functions
//

/////////////////////////////////////
// Name:	ErrorMsg
// Purpose:	add/display error
// Output:	error added
// Return:	none
/////////////////////////////////////
PUBLIC void ErrorMsg(const char *header, const char *msg, bool bAssert=true);

/////////////////////////////////////
// Name:	ErrorDump
// Purpose:	dump error to file
// Output:	error dump to file
// Return:	none
/////////////////////////////////////
PUBLIC void ErrorDump(const char *filename);

//
// Main Game Functions
//

PUBLIC void TataGenerateGameCfg(const char *configFile);

/////////////////////////////////////
// Name:	TataInit
// Purpose:	this will load the game
//			config and initialize the game
//			engine
// Output:	stuff initialized
// Return:	success if all goes well
/////////////////////////////////////
PUBLIC RETCODE TataInit(const char *configFile, HWND *hwnd, HINSTANCE hinst);

/////////////////////////////////////
// Name:	TataDestroy
// Purpose:	destroy engine and what-not
// Output:	death
// Return:	none
/////////////////////////////////////
PUBLIC void TataDestroy();

//
// Proc Functions
//

/////////////////////////////////////
// Name:	ProcInit
// Purpose:	initialize given proc state
// Output:	stuff initialized
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ProcInit(eGameProcType type);

/////////////////////////////////////
// Name:	ProcDestroy
// Purpose:	destroys the current proc
// Output:	stuff destroyed
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ProcDestroy();

/////////////////////////////////////
// Name:	ProcChangeState
// Purpose:	change current game state
//			This will destroy the
//			old proc and
//			initialize the new proc
// Output:	stuff destroyed and
//			initialized
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ProcChangeState(eGameProcType type);

/////////////////////////////////////
// Name:	ProcUpdate
// Purpose:	update current proc
// Output:	update
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE ProcUpdate();

/////////////////////////////////////
// Name:	ProcDisplay
// Purpose:	display current proc
// Output:	display
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE ProcDisplay();

//
// Input Functions
//

/////////////////////////////////////
// Name:	InputGetJoystick
// Purpose:	only used in certain places,
//			like configuration.  DONT
//			DESTROY!!!
// Output:	none
// Return:	the joystick
/////////////////////////////////////
PUBLIC hJOYSTICK InputGetJoystick();

/////////////////////////////////////
// Name:	InputTemp
// Purpose:	back-up current button
//			config
// Output:	input button config backup
// Return:	none
/////////////////////////////////////
PUBLIC void InputTemp();

/////////////////////////////////////
// Name:	InputRestore
// Purpose:	restore input button
//			config based on stuff
//			before InputTemp() was
//			called.
// Output:	input buttons set
// Return:	none
/////////////////////////////////////
PUBLIC void InputRestore();

/////////////////////////////////////
// Name:	InputDefault
// Purpose:	set to default input
// Output:	input buttons set to
//			default
// Return:	none
/////////////////////////////////////
PUBLIC void InputDefault();

/////////////////////////////////////
// Name:	InputGetCode
// Purpose:	get input code
// Output:	none
// Return:	the input code
/////////////////////////////////////
PUBLIC int InputGetCode(bool bKeyboard, eGameInput type);

/////////////////////////////////////
// Name:	InputQuery
// Purpose:	query the given button
//			with given code(from INPX)
//			bKeyboard==true if you want
//			code for keyboard = DIK_<something>
//			code for joystick = <btn. number>
//			to set keyboard, otherwise...
// Output:	button set
// Return:	none
/////////////////////////////////////
PUBLIC void InputQuery(bool bKeyboard, eGameInput type, int code);

/////////////////////////////////////
// Name:	InputLoad
// Purpose:	loads the input from given
//			config
// Output:	input initialized
// Return:	success if all goes well
/////////////////////////////////////
PUBLIC RETCODE InputLoad(hCFG cfg);

/////////////////////////////////////
// Name:	InputSave
// Purpose:	save the input config.
// Output:	cfg set
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE InputSave(hCFG cfg);

/////////////////////////////////////
// Name:	InputIsDown
// Purpose:	check for given input state
// Output:	none
// Return:	true if given input is down
/////////////////////////////////////
PUBLIC bool InputIsDown(eGameInput type);

/////////////////////////////////////
// Name:	InputIsReleased
// Purpose:	check for given input state
// Output:	none
// Return:	true if given input was
//			released
/////////////////////////////////////
PUBLIC bool InputIsReleased(eGameInput type);

/////////////////////////////////////
// Name:	InputIsDoubleTap
// Purpose:	did we double tapped?
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool InputIsDoubleTap(eGameInput type);

/////////////////////////////////////
// Name:	InputAnyBtnReleased
// Purpose:	check to see if any buttons
//			has been released.
//			excludes up/down/left/right
// Output:	none
// Return:	true if any btn. released
/////////////////////////////////////
PUBLIC bool InputAnyBtnReleased();

/////////////////////////////////////
// Name:	InputUpdate
// Purpose:	updates the input stuff
// Output:	stuff updated
// Return:	none
/////////////////////////////////////
PUBLIC void InputUpdate();

/////////////////////////////////////
// Name:	InputClear
// Purpose:	clear the input buffer
// Output:	buffer cleared
// Return:	none
/////////////////////////////////////
PUBLIC void InputClear();

/////////////////////////////////////
// Name:	InputDestroy
// Purpose:	destroy all input
//			components
//			NOTE: call this on game
//			exit.
// Output:	input stuff destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void InputDestroy();

/////////////////////////////////////
// Name:	InputChangeJoystickDevice
// Purpose:	change the joystick device
// Output:	joystick changed
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE InputChangeJoystickDevice(int enumInd);

/////////////////////////////////////
// Name:	InputGetJoystickEnumInd
// Purpose:	grab the enum. index of
//			joystick
// Output:	none
// Return:	the joystick enum. index,
//			-1 if no joystick.
/////////////////////////////////////
PUBLIC int InputGetJoystickEnumInd();

//
// Script functions
//

/////////////////////////////////////
// Name:	TataScriptQuery
// Purpose:	query all script variables
//			and functions
//			NOTE: only call this in
//			initialization.
// Output:	enables all new stuff in script
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE TataScriptQuery();

//
// Tools (found in tata_tool.cpp)
//

/////////////////////////////////////
// Name:	TataGetType
// Purpose:	get Ta-Ta type from name
// Output:	none
// Return:	the Ta-Ta type, -1 means 
//			unknown Ta-Ta
/////////////////////////////////////
PUBLIC int TataGetType(const char *name);

/////////////////////////////////////
// Name:	EnemyGetType
// Purpose:	get Enemy type from name
// Output:	none
// Return:	the Enemy type, -1 means 
//			unknown Enemy
/////////////////////////////////////
PUBLIC int EnemyGetType(const char *name);

/////////////////////////////////////
// Name:	ItemGetType
// Purpose:	get Item type from name
// Output:	none
// Return:	the item type
/////////////////////////////////////
PUBLIC int ItemGetType(const char *name);

/////////////////////////////////////
// Name:	WaypointGetMode
// Purpose:	get the waypoint mode type
//			from name.
// Output:	none
// Return:	the waypoint mode type, 
//			WAYPT_STOPATEND for unknown.
/////////////////////////////////////
PUBLIC eWayPtMode WaypointGetMode(const char *str);

/////////////////////////////////////
// Name:	TataDataGetFilePath
// Purpose:	get the filepath of given
//			data name
// Output:	none
// Return:	file path, 0 if not found
/////////////////////////////////////
PUBLIC const char *TataDataGetFilePath(const char *name);

/////////////////////////////////////
// Name:	TataDataFillList
// Purpose:	fill in the data
// Output:	data list updated
// Return:	none
/////////////////////////////////////
PUBLIC void TataDataFillList(char *dirPath, char *pattern);

/////////////////////////////////////
// Name:	TataSetBKFX
// Purpose:	set a new bkfx
// Output:	bkfx added
// Return:	none
/////////////////////////////////////
PUBLIC void TataSetBKFX(hBKFX bkfx);

/////////////////////////////////////
// Name:	TataUpdateBKFX
// Purpose:	update and display bkfx
// Output:	bkfx super special fx
// Return:	none
/////////////////////////////////////
PUBLIC void TataUpdateBKFX();

/////////////////////////////////////
// Name:	TataSetLoadImage
// Purpose:	set the load image
// Output:	load image set
// Return:	none
/////////////////////////////////////
PUBLIC void TataSetLoadImage(hTXT txt);

/////////////////////////////////////
// Name:	TataDisplayLoad()
// Purpose:	display load
// Output:	screen update
// Return:	none
/////////////////////////////////////
PUBLIC void TataDisplayLoad();

/////////////////////////////////////
// Name:	TataDisplayLoadText
// Purpose:	display a text message
//			instantly at the bottom
// Output:	screen update
// Return:	none
/////////////////////////////////////
PUBLIC void TataDisplayLoadText(const char *str, ...);

/////////////////////////////////////
// Name:	TataSaveGameCfg
// Purpose:	save the current config to
//			file
// Output:	config save
// Return:	none
/////////////////////////////////////
PUBLIC void TataSaveGameCfg();

PUBLIC void TataShadowDestroy();
PUBLIC void TataShadowInit();

PUBLIC float TataRand(float a, float b);

//construct the save game directory in My Documents folder
void TataCreateSaveDir();

//get the game.cfg filepath
void TataGetGameCfgPath(char *strOut);

//get the save game directory
void TataGetSaveDir(char *strOut);

#endif