#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

PRIVATE void _fxGatherAppend(fxGather *mainDat, float center[eMaxPt])
{
	fxGatherPar newPar = {0};

	memcpy(newPar.clr, mainDat->clr, sizeof(newPar.clr));
	newPar.clr[eA] = 1;

	memcpy(newPar.eLoc, center, sizeof(newPar.eLoc));

	//create a random normal direction
	D3DXVECTOR3 dir;

	do	//I'll eat my own shit if this loop ever takes more than a day!
	{
		dir.x = _GFXMathRand(-1, 1);
		dir.y = _GFXMathRand(-1, 1);
		dir.z = _GFXMathRand(-1, 1);
	} while(dir.x == 0 && dir.y == 0 && dir.z == 0);

	D3DXVec3Normalize(&dir, &dir);

	newPar.sLoc[eX] = center[eX] + mainDat->radius*dir.x;
	newPar.sLoc[eY] = center[eY] + mainDat->radius*dir.y;
	newPar.sLoc[eZ] = center[eZ] + mainDat->radius*dir.z;

	newPar.bFading = false;

	TimeInit(&newPar.delayTimer, mainDat->delayGather);

	//push
	mainDat->particles->push_back(newPar);
}

//returns true if ready to be removed
PRIVATE bool _fxGatherUpdate(fxGather *mainDat, fxGatherPar *par)
{
	double t = TimeGetTime(&par->delayTimer)/TimeGetDelay(&par->delayTimer);

	//move to center
	if(!par->bFading)
	{
		if(t <= 1)
		{
			par->loc[eX] = par->sLoc[eX] + t*(par->eLoc[eX]-par->sLoc[eX]);
			par->loc[eY] = par->sLoc[eY] + t*(par->eLoc[eY]-par->sLoc[eY]);
			par->loc[eZ] = par->sLoc[eZ] + t*(par->eLoc[eZ]-par->sLoc[eZ]);

			par->clr[eA] = t;
			par->scale = t*mainDat->scale;
		}
		else
		{
			//fade away
			par->bFading = true;
			TimeInit(&par->delayTimer, mainDat->delayFade);
			TimeReset(&par->delayTimer);
		}
	}
	else
	{
		if(t <= 1)
			par->clr[eA] = 1.0f - t;
		else //we are done
			return true;
	}

	return false;
}

PROTECTED RETCODE fxGatherFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxGather *thisData = (fxGather *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			//create a particle if there are not enough smokes
			if(thisData->particles->size() < thisData->maxParticle 
				&& dumbParam != PARFX_UPDATE_DEAD)
			{
				if(TimeElapse(&thisData->delaySpawn))
				{
					float center[eMaxPt] = {thisData->center[eX], thisData->center[eY], thisData->center[eZ]};

					hOBJ obj = PARFXGetOBJ(thisPARFX);

					if(obj)
					{
						PARFXGetOBJLoc(thisPARFX, obj, center);
					}
					
					_fxGatherAppend(thisData, center);
				}
			}

			//update particles from list
			fxGatherList::iterator nextIt;
			for(fxGatherList::iterator i = thisData->particles->begin(); i != thisData->particles->end();)
			{
				nextIt = i;
				nextIt++;

				//update this puff and see if it needs to be removed
				if(_fxGatherUpdate(thisData, &(*i)))
					thisData->particles->erase(i);

				i = nextIt;
			}

			if(thisData->particles->size() == 0 && dumbParam == PARFX_UPDATE_DEAD)
				return RETCODE_BREAK;
		}
		break;

	case PARFXM_DISPLAY:
		{
			for(fxGatherList::iterator i = thisData->particles->begin(); i != thisData->particles->end(); i++)
				TextureBltBillboard(thisData->explodeTxt, (*i).loc, (*i).scale, (*i).clr);
		}
		break;

	case PARFXM_CREATE:
		{
			fxGather_init *initDat = (fxGather_init *)dumbParam;

			fxGather *newData = (fxGather *)GFX_MALLOC(sizeof(fxGather));

			if(!newData)
				return RETCODE_FAILURE;

			newData->explodeTxt = initDat->explodeTxt; TextureAddRef(newData->explodeTxt);

			newData->clr[eR] = initDat->r/255.0f;
			newData->clr[eG] = initDat->g/255.0f;
			newData->clr[eB] = initDat->b/255.0f;

			newData->scale = initDat->scale;
			newData->radius = initDat->radius;
			newData->delayFade = initDat->delayFade;
			newData->delayGather = initDat->delayGather;

			newData->maxParticle = initDat->maxParticle;

			TimeInit(&newData->delaySpawn, initDat->delaySpawn);

			memcpy(newData->center, initDat->center, sizeof(newData->center));

			newData->particles = new fxGatherList;

			if(!newData->particles)
				return RETCODE_FAILURE;

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			if(thisData->particles)
				delete thisData->particles;

			if(thisData->explodeTxt)
				TextureDestroy(&thisData->explodeTxt);
		}
		break;
	}

	return RETCODE_SUCCESS;
}