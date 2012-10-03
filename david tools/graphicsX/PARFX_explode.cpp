#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_EXPLODE_ALPHA		255

PROTECTED RETCODE fxExplodeFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxExplode *thisData = (fxExplode *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			double t = TimeGetTime(&thisData->delayTimer)/TimeGetDelay(&thisData->delayTimer);

			if(t <= 1)
			{
				//set color
				thisData->clr[eA] = 1.0f - t;

				//set particle locations
				for(int i = 0; i < thisData->maxParticle; i++)
				{
					thisData->particles[i].loc[eX] = thisData->center[eX] + t*(thisData->particles[i].endLoc[eX]-thisData->center[eX]);
					thisData->particles[i].loc[eY] = thisData->center[eY] + t*(thisData->particles[i].endLoc[eY]-thisData->center[eY]);
					thisData->particles[i].loc[eZ] = thisData->center[eZ] + t*(thisData->particles[i].endLoc[eZ]-thisData->center[eZ]);
				}
			}
			else
				return RETCODE_BREAK;
		}
		break;

	case PARFXM_DISPLAY:
		{
			for(int i = 0; i < thisData->maxParticle; i++)
			{
				TextureBltBillboard(thisData->explodeTxt, thisData->particles[i].loc, thisData->scale, thisData->clr);
			}
		}
		break;

	case PARFXM_CREATE:
		{
			fxExplode_init *initDat = (fxExplode_init *)dumbParam;

			fxExplode *newData = (fxExplode *)GFX_MALLOC(sizeof(fxExplode));

			if(!newData)
				return RETCODE_FAILURE;

			newData->explodeTxt = initDat->explodeTxt; TextureAddRef(newData->explodeTxt);

			memcpy(newData->center, initDat->center, sizeof(initDat->center));

			//get the location based on obj, if attached...
			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
				PARFXGetOBJLoc(thisPARFX, obj, newData->center);

			newData->clr[eR] = initDat->r/255.0f;
			newData->clr[eG] = initDat->g/255.0f;
			newData->clr[eB] = initDat->b/255.0f;

			newData->scale = initDat->scale;

			newData->maxParticle = initDat->maxParticle;

			TimeInit(&newData->delayTimer, initDat->delay);

			//create the particles
			newData->particles = (fxExplodePar *)GFX_MALLOC(sizeof(fxExplodePar)*newData->maxParticle);

			if(!newData->particles)
				return RETCODE_FAILURE;

			//initialize each
			D3DXVECTOR3 dir;

			for(int i = 0; i < newData->maxParticle; i++)
			{
				do	//I'll eat my own shit if this loop ever takes more than a day!
				{
					dir.x = _GFXMathRand(-1, 1);
					dir.y = _GFXMathRand(-1, 1);
					dir.z = _GFXMathRand(-1, 1);
				} while(dir.x == 0 && dir.y == 0 && dir.z == 0);

				D3DXVec3Normalize(&dir, &dir);

				newData->particles[i].endLoc[eX] = newData->center[eX] + initDat->radius*dir.x;
				newData->particles[i].endLoc[eY] = newData->center[eY] + initDat->radius*dir.y;
				newData->particles[i].endLoc[eZ] = newData->center[eZ] + initDat->radius*dir.z;
			}

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			TextureDestroy(&thisData->explodeTxt);

			if(thisData->particles)
				GFX_FREE(thisData->particles);
		}
		break;
	}

	return RETCODE_SUCCESS;
}