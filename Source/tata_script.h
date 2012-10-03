#ifndef _tata_script_h
#define _tata_script_h

//this is where all the script types/callbacks are defined (variables/functions)

//
// Variable Callbacks
//

////////////////
// Vector 3
////////////////

//Vector3 (float, float, float)
PROTECTED long S_Vector3Callback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////
// Entity ID
////////////////

//EntityID (string entityname)
PROTECTED long S_EntityIDCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//////////////////
// Image Display
//////////////////

//Image (string filename, float sx, float sy, float ex, float ey, int r, int g, int b, int a)
//create an image display in the world, defaults to disable
PROTECTED long S_ImageCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//////////////////
// Dialog
//////////////////

//Dialog (string imgfile, float sx, float sy, float ex, float ey)
//a dialog, must set font before use
//make sure to open it to display
PROTECTED long S_DialogCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//
// Function Callbacks
//

////////////////
// Vector 3
////////////////

//Vector3GetX (float f, vector3 vec)
//f = vec.x
PROTECTED long S_Vector3GetX(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//Vector3GetY (float f, vector3 vec)
//f = vec.y
PROTECTED long S_Vector3GetY(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//Vector3GetZ (float f, vector3 vec)
//f = vec.z
PROTECTED long S_Vector3GetZ(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//Vector3Set (vector3 v1, vector3 v2)
//v1 = v2
PROTECTED long S_Vector3Set(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////
// Target
////////////////

//TargetGet (string targetname, vector3 v)
//v = target.loc
PROTECTED long S_TargetGet(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TargetSet (string targetname, vector3 v)
//target.loc = v
PROTECTED long S_TargetSet(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TargetAdd (string targetname, vector3 v)
//add a target with given name to list
PROTECTED long S_TargetAdd(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////
// View
////////////////

//ViewSetLoc (vector3 v)
//instant view location set
PROTECTED long S_ViewSetLoc(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//ViewSetTarget (vector3 v)
//instant view target set
PROTECTED long S_ViewSetTarget(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//ViewSetDelay (float delay)
//set view delay from getting to destination
PROTECTED long S_ViewSetDelay(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//ViewMoveLoc (vector3 v)
//move view to location
PROTECTED long S_ViewMoveLoc(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//ViewMoveTarget (vector3 v)
//move view target to dest
PROTECTED long S_ViewMoveTarget(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//ViewFollowWaypoint (string wayptname, int mode, float delay, int isDone)
//view will follow the waypoint with given mode.
//isDone = 1 if view still following, 0 otherwise
PROTECTED long S_ViewFollowWaypoint(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////////
// General Entity
////////////////////

//EntityAvailable (string name, int iOut)
//check to see if an entity by the given name exists, iOut = 1 if found,
//0 otherwise.
PROTECTED long S_EntityAvailable(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntityGetTypeName (string name, string typeOut)
//get the type of given entity
PROTECTED long S_EntityGetTypeName(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntityGetSubTypeName (string name, string typeOut)
//get the sub-type of given entity
PROTECTED long S_EntityGetSubTypeName(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntityPollDeath (string name)
//kill entity
PROTECTED long S_EntityPollDeath(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntitySetWP (string name, string wpName, string mode, int bBackward)
//Set entity waypoint
PROTECTED long S_EntitySetWP(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntityIsWPDone(string name, int iOut)
//check to see if wp of entity is done, iOut = 1 if done, 0 otherwise
PROTECTED long S_EntityIsWPDone(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntitySetOBJState (string name, int state)
//set the entity object state
PROTECTED long S_EntitySetOBJState(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntityIsOBJStateEnd (string name, int iOut)
//check to see if object state has ended
PROTECTED long S_EntityIsOBJStateEnd(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntitySetDir (string name, string target)
//set entity direction via target
PROTECTED long S_EntitySetDir(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//EntitySetBehavior(string name, int state)
//set entity behavior (only used by certain entities)
PROTECTED long S_EntitySetBehavior(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

///////////////////
// General Creature
///////////////////

//CreatureHit (string name, int amt)
//damage creature with given amount
PROTECTED long S_CreatureHit(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//CreatureSetHitPoint (string name, int amt)
//set creature's hit point
PROTECTED long S_CreatureSetHitPoint(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//CreatureTeleport (string name, string targetName)
//teleport given creature to target
PROTECTED long S_CreatureTeleport(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////
// Ta-Ta
////////////////

//TataOnPlayList (string tataTypeName, int iOut)
//check to see if a particular Ta-Ta is on the play list, iOut = 1 if it is,
//0 otherwise.
PROTECTED long S_TaTaOnPlayList(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TataCurrentTeleport (string targetName)
//teleport current Ta-Ta to a target destination.
PROTECTED long S_TataCurrentTeleport(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TataCurrentSet (string tataName)
//set current Ta-Ta to given Ta-Ta type name.
//the given Ta-Ta name must exist on the map.
PROTECTED long S_TataCurrentSet(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TataCurrentGetTypeName (string sOut)
//fill in the given string with the current Ta-Ta's type name.
PROTECTED long S_TataCurrentGetTypeName(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TataAddToPlayList (string tataName)
//add Ta-Ta to playlist if available in map
PROTECTED long S_TataAddToPlayList(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TataDetachAndSpawnBaby (string tataName)
//detaches and respawns the baby tata this ta is carrying
PROTECTED long S_TataDetachAndSpawnBaby(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////
// Object
////////////////

//ObjectActivate (string objname, int bActive)
//set the object active/inactive
PROTECTED long S_ObjectActivate(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//ObjectISActivate (string objname, int iOut)
//is the object active? iOut = 0 (no) iOut = 1 (yes)
PROTECTED long S_ObjectISActive(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//PlatformSetWPMode (string platformname, string mode)
//set the platform waypoint
PROTECTED long S_PlatformSetWPMode(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//PlatformWPBackward (string platformname, int bBackward)
//set the platform backward or forward
PROTECTED long S_PlatformWPBackward(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//PlatformISWPBackward (string platformname, int iOut)
//is the platform backward? iOut = 0 (no) iOut = 1 (yes)
PROTECTED long S_PlatformISWPBackward(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//TriggerGetEntity (string sOut)
//get the entity that collided with Trigger
//NOTE: this is only used in trigger scripts
PROTECTED long S_TriggerGetEntity(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

////////////////
// World/Global
////////////////

//MenuActivate (string menuName)
//activates a menu
PROTECTED long S_MenuActivate(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//MenuIsAvailable (string menuName, int iOut)
//check to see if the given menu is available
//iOut = 1 if available, 0 otherwise
PROTECTED long S_MenuIsAvailable(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//MapLoadCloud (string target, float angle, int bRemoveCutscene)
//Load the cloud level
//angle is in degree
//bRemoveCutscene == true, remove cutscene
PROTECTED long S_MapLoadCloud(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//MapLoad (string map, string target, float angle, int bRemoveCutscene)
//Load given map, relative to game folder
//angle is in degree
//bRemoveCutscene == true, remove cutscene
PROTECTED long S_MapLoad(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//SetStageLevel (int stage, int level)
//set the stage and level to play.
PROTECTED long S_SetStageLevel(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//SetWorldScene (string section)
//set the world scene from a section in world scene file
PROTECTED long S_SetWorldScene(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//Wait (float delay)
//wait for given milliseconds
PROTECTED long S_Wait(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//WaitBtn (float delay)
//wait for button with given milliseconds
PROTECTED long S_WaitBtn(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//GoalAdd (int amt)
//adds goal with given amount
PROTECTED long S_GoalAdd(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//LoadCreditsScene (int bSave)
//loads up the credits (after credits, the player will be brought back to the main menu)
PROTECTED long S_LoadCreditsScene(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//WorldSetMusic (string filename)
//sets the current music
PROTECTED long S_WorldSetMusic(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//////////////////
// Image Display
//////////////////

//ImageSetStatus (image img, int state)
//set the status of the given image
PROTECTED long S_ImageSetStatus(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//////////////////
// Dialog
//////////////////

//DialogSetFont (dialog dlg, string typeName, float size, int fmtFlag)
//set the dialog font
PROTECTED long S_DialogSetFont(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//DialogOpen (dialog dlg, int bOpen)
//open/close dialog (bOpen == 0 close, 1 open)
PROTECTED long S_DialogOpen(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//DialogSetText (dialog dlg, string text)
//set dialog text
PROTECTED long S_DialogSetText(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//DialogSetTextClr (dialog dlg, int r, int g, int b)
//set dialog text color
PROTECTED long S_DialogSetTextClr(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

#endif