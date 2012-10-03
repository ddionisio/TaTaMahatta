#include "tata_main.h"

#include "tata_script.h"

//Query all script variables and functions

/////////////////////////////////////
// Name:	TataScriptQuery
// Purpose:	query all script variables
//			and functions
// Output:	enables all new stuff in script
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE TataScriptQuery()
{
	//Variables
	ScriptTeaseAddVarType("vector3", sizeof(D3DXVECTOR3), "float, float, float", S_Vector3Callback);

	ScriptTeaseAddVarType("entityid", sizeof(Id), "string", S_EntityIDCallback);

	ScriptTeaseAddVarType("image", sizeof(Id), "string, float, float, float, float, int, int, int, int", S_ImageCallback);

	ScriptTeaseAddVarType("dialog", sizeof(Id), "string, float, float, float, float", S_DialogCallback);

	//Functions
	ScriptTeaseAddFuncType("vector3getx", "float, vector3", S_Vector3GetX);
	ScriptTeaseAddFuncType("vector3gety", "float, vector3", S_Vector3GetY);
	ScriptTeaseAddFuncType("vector3getz", "float, vector3", S_Vector3GetZ);
	ScriptTeaseAddFuncType("vector3set", "vector3, vector3", S_Vector3Set);

	ScriptTeaseAddFuncType("targetget", "string, vector3", S_TargetGet);
	ScriptTeaseAddFuncType("targetset", "string, vector3", S_TargetSet);
	ScriptTeaseAddFuncType("targetadd", "string, vector3", S_TargetAdd);

	ScriptTeaseAddFuncType("viewsetloc", "vector3", S_ViewSetLoc);
	ScriptTeaseAddFuncType("viewsettarget", "vector3", S_ViewSetTarget);
	ScriptTeaseAddFuncType("viewsetdelay", "float", S_ViewSetDelay);
	ScriptTeaseAddFuncType("viewmoveloc", "vector3", S_ViewMoveLoc);
	ScriptTeaseAddFuncType("viewmovetarget", "vector3", S_ViewMoveTarget);
	ScriptTeaseAddFuncType("viewfollowwaypoint", "string, int, float, int", S_ViewFollowWaypoint);

	ScriptTeaseAddFuncType("entityavailable", "string, int", S_EntityAvailable);

	ScriptTeaseAddFuncType("entitygettypename", "string, string", S_EntityGetTypeName);
	ScriptTeaseAddFuncType("entitygetsubtypename", "string, string", S_EntityGetSubTypeName);
	ScriptTeaseAddFuncType("entitypolldeath", "string", S_EntityPollDeath);
	ScriptTeaseAddFuncType("entitysetwp", "string, string, string, int", S_EntitySetWP);
	ScriptTeaseAddFuncType("entityiswpdone", "string, int", S_EntityIsWPDone);
	ScriptTeaseAddFuncType("entitysetobjstate", "string, int", S_EntitySetOBJState);
	ScriptTeaseAddFuncType("entityisobjstateend", "string, int", S_EntityIsOBJStateEnd);
	ScriptTeaseAddFuncType("entitysetdir", "string, string", S_EntitySetDir);
	ScriptTeaseAddFuncType("entitysetbehavior", "string, int", S_EntitySetBehavior);

	ScriptTeaseAddFuncType("creaturehit", "string, int", S_CreatureHit);
	ScriptTeaseAddFuncType("creaturesethitpoint", "string, int", S_CreatureSetHitPoint);
	ScriptTeaseAddFuncType("creatureteleport", "string, string", S_CreatureTeleport);

	ScriptTeaseAddFuncType("tataonplaylist", "string, int", S_TaTaOnPlayList);
	ScriptTeaseAddFuncType("tatacurrentteleport", "string", S_TataCurrentTeleport);
	ScriptTeaseAddFuncType("tatacurrentset", "string", S_TataCurrentSet);
	ScriptTeaseAddFuncType("tatacurrentgettypename", "string", S_TataCurrentGetTypeName);
	ScriptTeaseAddFuncType("tataaddtoplaylist", "string", S_TataAddToPlayList);
	ScriptTeaseAddFuncType("tatadetachandspawnbaby", "string", S_TataDetachAndSpawnBaby);

	ScriptTeaseAddFuncType("objectactivate", "string, int", S_ObjectActivate);
	ScriptTeaseAddFuncType("objectisactive", "string, int", S_ObjectISActive);

	ScriptTeaseAddFuncType("platformsetwpmode", "string, string", S_PlatformSetWPMode);
	ScriptTeaseAddFuncType("platformwpbackward", "string, int", S_PlatformWPBackward);
	ScriptTeaseAddFuncType("platformiswpbackward", "string, int", S_PlatformISWPBackward);

	ScriptTeaseAddFuncType("triggergetentity", "string", S_TriggerGetEntity);

	ScriptTeaseAddFuncType("menuactivate", "string", S_MenuActivate);
	ScriptTeaseAddFuncType("menuisavailable", "string, int", S_MenuIsAvailable);
	ScriptTeaseAddFuncType("maploadcloud", "string, float, int", S_MapLoadCloud);
	ScriptTeaseAddFuncType("mapload", "string, string, float, int", S_MapLoad);
	ScriptTeaseAddFuncType("setstagelevel", "int, int", S_SetStageLevel);
	ScriptTeaseAddFuncType("setworldscene", "string", S_SetWorldScene);
	ScriptTeaseAddFuncType("wait", "float", S_Wait);
	ScriptTeaseAddFuncType("waitbtn", "float", S_WaitBtn);
	ScriptTeaseAddFuncType("goaladd", "int", S_GoalAdd);
	ScriptTeaseAddFuncType("loadcreditsscene", "int", S_LoadCreditsScene);
	ScriptTeaseAddFuncType("worldsetmusic", "string", S_WorldSetMusic);
	
	ScriptTeaseAddFuncType("imagesetstatus", "image, int", S_ImageSetStatus);

	ScriptTeaseAddFuncType("dialogsetfont", "dialog, string, float, int", S_DialogSetFont);
	ScriptTeaseAddFuncType("dialogopen", "dialog, int", S_DialogOpen);
	ScriptTeaseAddFuncType("dialogsettext", "dialog, string", S_DialogSetText);
	ScriptTeaseAddFuncType("dialogsettextclr", "dialog, int, int, int", S_DialogSetTextClr);

	return RETCODE_SUCCESS;
}
