#include "gdix.h"
#include "gdix_i.h"

//All object file loading are done here

//key frame loading
PROTECTED RETCODE _ObjLoadAnimationKey(hOBJ obj, const char *animationfile)
{
	assert(obj);

	RETCODE ret = RETCODE_SUCCESS;

	int i;

	//ready the file
	FILE *theFile;

	theFile = fopen(animationfile, "rt");
	if(!theFile)
	{ ASSERT_MSG(0, "Unable to open file", "Error in _ObjLoadAnimationKey"); return RETCODE_FAILURE; }

	//get delay
	int delay;
	fscanf(theFile, "delay=%d\n", &delay);

	//get number of states
	fscanf(theFile, "numStates=%d\n", &obj->numStates);

	//allocate states
	obj->states = (gfxObjMdlState*)GFX_MALLOC(sizeof(gfxObjMdlState)*obj->numStates);
	if(!obj->states)
	{ ASSERT_MSG(0, "Unable to allocate states", "Error in _ObjLoadAnimationKey"); ret = RETCODE_FAILURE; goto SUCKS; }

	//get all key frames
	for(i = 0; i < obj->numStates; i++)
	{
		fscanf(theFile, "s=%d e=%d d=%f\n", &obj->states[i].startFrame, &obj->states[i].endFrame, &obj->states[i].maxDelay);

		//calculate delay per frame
		float numFrame = obj->states[i].endFrame-obj->states[i].startFrame;

		if(numFrame > 0)
			obj->states[i].delay = obj->states[i].maxDelay/(numFrame+1);
		else
			obj->states[i].delay = 0;
	}

	obj->curFrame = obj->prevFrame = obj->states[obj->curState].startFrame;

	//set up timer
	TimeInit(&obj->ticker, obj->states[obj->curState].delay);

SUCKS:
	fclose(theFile);
	return ret;
}