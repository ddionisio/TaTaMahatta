#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_PUFF_ALPHA		255

PRIVATE RETCODE _puffAppend(fxPuffs *mainDat, float loc[eMaxPt])
{
	fxPuff_par newPuff={0};

	//initialize stuff
	memcpy(newPuff.loc, mainDat->center, sizeof(mainDat->center));

	newPuff.clr[eR] = mainDat->r;
	newPuff.clr[eG] = mainDat->g;
	newPuff.clr[eB] = mainDat->b;
	newPuff.clr[eA] = 1;
	
	newPuff.rot   = 0;

	TimeInit(&newPuff.delayTimer, mainDat->delay);

	mainDat->puffs->push_back(newPuff);

	return RETCODE_SUCCESS;
}

//returns true if ready to be removed
PRIVATE bool _puffUpdate(fxPuffs *mainDat, fxPuff_par *thePuff)
{
	double t = TimeGetTime(&thePuff->delayTimer)/TimeGetDelay(&thePuff->delayTimer);

	if(t <= 1)
	{
		thePuff->clr[eA] = 1.0f - t;
		
		thePuff->loc[eX] += mainDat->dir[eX];
		thePuff->loc[eY] += mainDat->dir[eY];
		thePuff->loc[eZ] += mainDat->dir[eZ];

		thePuff->scale = mainDat->scaleStart + t*(mainDat->scaleEnd - mainDat->scaleStart);

		return false;
	}

	return true;
}

PROTECTED RETCODE fxPuffFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxPuffs *thisData = (fxPuffs *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			//create a particle if there are not enough puffs
			if(thisData->puffs->size() < thisData->maxParticle && dumbParam != PARFX_UPDATE_DEAD)
			{
				hOBJ obj = PARFXGetOBJ(thisPARFX);

				if(obj)
					PARFXGetOBJLoc(thisPARFX, obj, thisData->center);
				
				_puffAppend(thisData, thisData->center);
			}

			//update particles from list
			fxPuffList::iterator nextIt;
			for(fxPuffList::iterator i = thisData->puffs->begin(); i != thisData->puffs->end();)
			{
				nextIt = i;
				nextIt++;

				//update this puff and see if it needs to be removed
				if(_puffUpdate(thisData, &(*i)))
					thisData->puffs->erase(i);

				i = nextIt;
			}

			if(thisData->puffs->size() == 0 && dumbParam == PARFX_UPDATE_DEAD)
				return RETCODE_BREAK;
		}
		break;

	case PARFXM_DISPLAY:
		{
			for(fxPuffList::iterator i = thisData->puffs->begin(); i != thisData->puffs->end(); i++)
			{
				TextureBltBillboard(thisData->puffTxt, (*i).loc, (*i).scale, (*i).clr);
			}
		}
		break;

	case PARFXM_CREATE:
		{
			fxPuffs_init *initDat = (fxPuffs_init *)dumbParam;

			//allocate the new puff data
			fxPuffs *newData = (fxPuffs *)GFX_MALLOC(sizeof(fxPuffs));

			if(!newData)
				return RETCODE_FAILURE;

			//fill-in stuff
			newData->puffTxt = initDat->puffTxt; TextureAddRef(newData->puffTxt);

			memcpy(newData->center, initDat->center, sizeof(initDat->center));
			memcpy(newData->dir, initDat->dir, sizeof(initDat->dir));

			newData->r           = initDat->r/255.0f;
			newData->g           = initDat->g/255.0f;
			newData->b           = initDat->b/255.0f;
			newData->maxParticle = initDat->maxParticle;
			newData->scaleStart	 = initDat->scaleStart;
			newData->scaleEnd    = initDat->scaleEnd;
			newData->rotSpd      = initDat->rotSpd;
			newData->delay       = initDat->delay;

			newData->puffs = new fxPuffList;

			if(!newData->puffs)
				return RETCODE_FAILURE;

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			TextureDestroy(&thisData->puffTxt);

			if(thisData->puffs)
				delete thisData->puffs;
		}
		break;
	}

	return RETCODE_SUCCESS;
}