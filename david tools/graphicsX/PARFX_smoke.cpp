#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_SMOKE_ALPHA		255

PRIVATE void _fxSmokeAppend(fxSmoke *mainDat, float trans[eMaxPt], float eTrans[eMaxPt])
{
	fxSmoke_par newSmokePar={0};

	//set start location
	newSmokePar.sLoc[eX] = _GFXMathRand(mainDat->min[eX], mainDat->max[eX]) + trans[eX];
	newSmokePar.sLoc[eY] = _GFXMathRand(mainDat->min[eY], mainDat->max[eY]) + trans[eY];
	newSmokePar.sLoc[eZ] = _GFXMathRand(mainDat->min[eZ], mainDat->max[eZ]) + trans[eZ];

	//set color
	newSmokePar.clr[eR] = mainDat->r;
	newSmokePar.clr[eG] = mainDat->g;
	newSmokePar.clr[eB] = mainDat->b;
	newSmokePar.clr[eA] = 1;

	//set end location
	memcpy(newSmokePar.eLoc, eTrans, sizeof(newSmokePar.eLoc));

	switch(mainDat->axis)
	{
	case 0:
		newSmokePar.eLoc[eX] = newSmokePar.sLoc[eX];
		break;

	case 1:
		newSmokePar.eLoc[eY] = newSmokePar.sLoc[eY];
		break;

	case 2:
		newSmokePar.eLoc[eZ] = newSmokePar.sLoc[eZ];
		break;
	}

	//set delay
	TimeInit(&newSmokePar.delayTimer, mainDat->spdDelay);

	//the smoke will move and fade in first
	newSmokePar.bFading = false;

	//append
	mainDat->smokes->push_back(newSmokePar);
}

//returns true if ready to be removed
PRIVATE bool _fxSmokeUpdate(fxSmoke *mainDat, fxSmoke_par *theSmoke)
{
	double t = TimeGetTime(&theSmoke->delayTimer)/TimeGetDelay(&theSmoke->delayTimer);

	//moving
	if(!theSmoke->bFading)
	{
		if(t <= 1)
		{
			theSmoke->loc[eX] = theSmoke->sLoc[eX] + t*(theSmoke->eLoc[eX]-theSmoke->sLoc[eX]);
			theSmoke->loc[eY] = theSmoke->sLoc[eY] + t*(theSmoke->eLoc[eY]-theSmoke->sLoc[eY]);
			theSmoke->loc[eZ] = theSmoke->sLoc[eZ] + t*(theSmoke->eLoc[eZ]-theSmoke->sLoc[eZ]);

			theSmoke->clr[eA] = t;
		}
		else
		{
			//initialize fading
			theSmoke->bFading = true;

			TimeInit(&theSmoke->delayTimer, mainDat->fadeDelay);
			TimeReset(&theSmoke->delayTimer);
		}
	}
	//fading
	else
	{
		if(t <= 1)
			theSmoke->clr[eA] = 1.0f - t;
		else //we are done
			return true;
	}

	return false;
}

PROTECTED RETCODE fxSmokeFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxSmoke *thisData = (fxSmoke *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			//create a particle if there are not enough smokes
			if(thisData->smokes->size() < thisData->maxParticle 
				&& thisData->bRepeat
				&& dumbParam != PARFX_UPDATE_DEAD)
			{
				if(TimeElapse(&thisData->spawnDelay))
				{
					float center[eMaxPt] = {thisData->sLoc[eX], thisData->sLoc[eY], thisData->sLoc[eZ]};
					float end[eMaxPt] = {thisData->eLoc[eX], thisData->eLoc[eY], thisData->eLoc[eZ]};

					hOBJ obj = PARFXGetOBJ(thisPARFX);

					if(obj)
					{
						PARFXGetOBJLoc(thisPARFX, obj, center);

						if(thisData->eJointInd != -1)
						{
							OBJJointGetWorldLoc(obj, thisData->eJointInd, end);
						}
					}
					
					_fxSmokeAppend(thisData, center, end);
				}
			}

			//update particles from list
			fxSmokeList::iterator nextIt;
			for(fxSmokeList::iterator i = thisData->smokes->begin(); i != thisData->smokes->end();)
			{
				nextIt = i;
				nextIt++;

				//update this puff and see if it needs to be removed
				if(_fxSmokeUpdate(thisData, &(*i)))
					thisData->smokes->erase(i);

				i = nextIt;
			}

			if(thisData->smokes->size() == 0 && dumbParam == PARFX_UPDATE_DEAD)
				return RETCODE_BREAK;
		}
		break;

	case PARFXM_DISPLAY:
		{
			for(fxSmokeList::iterator i = thisData->smokes->begin(); i != thisData->smokes->end(); i++)
					TextureBltBillboard(thisData->smokeTxt, (*i).loc, thisData->size, (*i).clr);
		}
		break;

	case PARFXM_CREATE:
		{
			fxSmoke_init *initDat = (fxSmoke_init *)dumbParam;

			fxSmoke *newData = (fxSmoke *)GFX_MALLOC(sizeof(fxSmoke));

			if(!newData)
				return RETCODE_FAILURE;

			newData->smokeTxt = initDat->smokeTxt; TextureAddRef(newData->smokeTxt);

			newData->r = initDat->r/255.0f;
			newData->g = initDat->g/255.0f;
			newData->b = initDat->b/255.0f;

			TimeInit(&newData->spawnDelay, initDat->spawnDelay);

			newData->spdDelay = initDat->spdDelay;
			newData->fadeDelay = initDat->fadeDelay;

			newData->size = initDat->size;

			memcpy(newData->min, initDat->min, sizeof(initDat->min));
			memcpy(newData->max, initDat->max, sizeof(initDat->max));

			memcpy(newData->sLoc, initDat->sLoc, sizeof(initDat->sLoc));
			memcpy(newData->eLoc, initDat->eLoc, sizeof(initDat->eLoc));

			newData->eJointInd = initDat->eJointInd;

			newData->maxParticle = initDat->maxParticle;

			newData->axis = initDat->axis;

			newData->bRepeat = initDat->bRepeat;

			newData->smokes = new fxSmokeList;

			if(!newData->smokes)
				return RETCODE_FAILURE;

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			TextureDestroy(&thisData->smokeTxt);

			if(thisData->smokes)
				delete thisData->smokes;
		}
		break;
	}

	return RETCODE_SUCCESS;
}