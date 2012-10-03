#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_GAS_ALPHA	255

PRIVATE void _fxGasAppend(fxGas *mainDat, float trans[eMaxPt])
{
	fxGas_par newGasPar={0};

	//set location
	newGasPar.loc[eX] = _GFXMathRand(mainDat->min[eX], mainDat->max[eX]) + trans[eX];
	newGasPar.loc[eY] = _GFXMathRand(mainDat->min[eY], mainDat->max[eY]) + trans[eY];
	newGasPar.loc[eZ] = _GFXMathRand(mainDat->min[eZ], mainDat->max[eZ]) + trans[eZ];

	//set color
	newGasPar.clr[eR] = mainDat->r;
	newGasPar.clr[eG] = mainDat->g;
	newGasPar.clr[eB] = mainDat->b;
	newGasPar.clr[eA] = 1;

	//the gas will grow first
	newGasPar.bFading = false;

	//set timer for scale
	TimeInit(&newGasPar.delayTimer, _GFXMathRand(mainDat->scaleDelayMin, mainDat->scaleDelayMax));

	//append
	mainDat->gases->push_back(newGasPar);
}

//returns true if ready to be removed
PRIVATE bool _fxGasUpdate(fxGas *mainDat, fxGas_par *theGas)
{
	double t = TimeGetTime(&theGas->delayTimer)/TimeGetDelay(&theGas->delayTimer);

	//growing
	if(!theGas->bFading)
	{
		if(t <= 1)
			theGas->scale = t*mainDat->scaleMax;
		else
		{
			//initialize fading
			theGas->bFading = true;

			TimeInit(&theGas->delayTimer, _GFXMathRand(mainDat->alphaDelayMin, mainDat->alphaDelayMax));
			TimeReset(&theGas->delayTimer);

			theGas->scale = mainDat->scaleMax;
		}
	}
	//fading
	else
	{
		if(t <= 1)
			theGas->clr[eA] = 1.0f - t;
		else //we are done
			return true;
	}

	if(mainDat->spd != 0)
	{
		float vAmt = mainDat->spd*(GFX_TIME_TOLER*g_timeElapse);

		theGas->loc[eX] += vAmt*mainDat->dir[eX];
		theGas->loc[eY] += vAmt*mainDat->dir[eY];
		theGas->loc[eZ] += vAmt*mainDat->dir[eZ];
	}

	return false;
}

PROTECTED RETCODE fxGasFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxGas *thisData = (fxGas *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			//create a particle if there are not enough gas
			if(thisData->gases->size() < thisData->maxParticle 
				&& thisData->bRepeat
				&& dumbParam != PARFX_UPDATE_DEAD)
			{
				if(TimeElapse(&thisData->spawnDelay))
				{
					float center[eMaxPt] = {0};

					hOBJ obj = PARFXGetOBJ(thisPARFX);

					if(obj)
						PARFXGetOBJLoc(thisPARFX, obj, center);
					
					_fxGasAppend(thisData, center);
				}
			}

			//update particles from list
			fxGasList::iterator nextIt;
			for(fxGasList::iterator i = thisData->gases->begin(); i != thisData->gases->end();)
			{
				nextIt = i;
				nextIt++;

				//update this puff and see if it needs to be removed
				if(_fxGasUpdate(thisData, &(*i)))
					thisData->gases->erase(i);

				i = nextIt;
			}

			if(thisData->gases->size() == 0 && dumbParam == PARFX_UPDATE_DEAD)
				return RETCODE_BREAK;
		}
		break;

	case PARFXM_DISPLAY:
		{
			if(!TESTFLAGS(thisPARFX->status, PARFX_FLAG_OBJATTACHED)
				&& !FrustrumCheckBox(
					thisData->min[eX], thisData->min[eY], thisData->min[eZ],
					thisData->max[eX], thisData->max[eY], thisData->max[eZ]))
					break;
			
			for(fxGasList::iterator i = thisData->gases->begin(); i != thisData->gases->end(); i++)
				TextureBltBillboard(thisData->gasTxt, (*i).loc, (*i).scale, (*i).clr);
		}
		break;

	case PARFXM_CREATE:
		{
			fxGas_init *initDat = (fxGas_init *)dumbParam;

			fxGas *newData = (fxGas *)GFX_MALLOC(sizeof(fxGas));

			if(!newData)
				return RETCODE_FAILURE;

			newData->gasTxt = initDat->gasTxt; TextureAddRef(newData->gasTxt);

			newData->r = initDat->r/255.0f;
			newData->g = initDat->g/255.0f;
			newData->b = initDat->b/255.0f;

			memcpy(newData->dir, initDat->dir, sizeof(initDat->dir));
			memcpy(newData->min, initDat->min, sizeof(initDat->min));
			memcpy(newData->max, initDat->max, sizeof(initDat->max));

			newData->spd = initDat->spd;

			TimeInit(&newData->spawnDelay, initDat->spawnDelay);

			newData->maxParticle   = initDat->maxParticle;
			newData->scaleMax	   = initDat->scaleMax;
			newData->scaleDelayMin = initDat->scaleDelayMin;
			newData->scaleDelayMax = initDat->scaleDelayMax;
			newData->alphaDelayMin = initDat->alphaDelayMin;
			newData->alphaDelayMax = initDat->alphaDelayMax;
			newData->bRepeat	   = initDat->bRepeat;

			//initialize the gas particles
			newData->gases = new fxGasList;

			if(!newData->gases)
				return RETCODE_FAILURE;

			//fill in the gases
			/*float center[eMaxPt] = {0};
			
			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
				PARFXGetOBJLoc(thisPARFX, obj, center);
			
			while(newData->gases->size() < newData->maxParticle)
			{
				_fxGasAppend(newData, center);
			}*/

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			TextureDestroy(&thisData->gasTxt);

			if(thisData->gases)
				delete thisData->gases;
		}
		break;
	}

	return RETCODE_SUCCESS;
}