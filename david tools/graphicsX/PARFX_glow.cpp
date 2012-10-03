#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_GLOW_ALPHA	255

PROTECTED RETCODE fxGlowFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxGlow *thisData = (fxGlow *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			//set center location
			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
				PARFXGetOBJLoc(thisPARFX, obj, thisData->center);

			//update timer
			double t = TimeGetTime(&thisData->delayTimer)/TimeGetDelay(&thisData->delayTimer);

			if(t <= 1)
			{
				thisData->clr[eA] = thisData->alphaStart + t*(thisData->alphaEnd - thisData->alphaStart);

				thisData->scale = thisData->scaleStart + t*(thisData->scaleEnd - thisData->scaleStart);
			}
			//Is the glowing over?
			else
			{
				TimeReset(&thisData->delayTimer);

				//switch the scale start/end
				float temp = thisData->scaleEnd;
				thisData->scaleEnd = thisData->scaleStart;
				thisData->scaleStart = temp;

				//switch the alpha start/end
				//if there is no repeat or that we are ending, then we are done
				if(thisData->alphaEnd == 1)
				{ 
					thisData->alphaEnd = 0; thisData->alphaStart = 1; 
				}
				else if(thisData->alphaEnd == 0)
				{
					if(!thisData->bRepeat || dumbParam == PARFX_UPDATE_DEAD)
						return RETCODE_BREAK;

					thisData->alphaStart = 0; thisData->alphaEnd = 1;
				}
			}
		}
		break;

	case PARFXM_DISPLAY:
		TextureBltBillboard(thisData->glowTxt, thisData->center, thisData->scale, thisData->clr);
		break;

	case PARFXM_CREATE:
		{
			fxGlow_init *initDat = (fxGlow_init *)dumbParam;

			fxGlow *newData = (fxGlow *)GFX_MALLOC(sizeof(fxGlow));

			if(!newData)
				return RETCODE_FAILURE;

			newData->glowTxt = initDat->glowTxt; TextureAddRef(newData->glowTxt);

			memcpy(newData->center, initDat->center, sizeof(initDat->center));

			newData->clr[eR] = initDat->r/255.0f;
			newData->clr[eG] = initDat->g/255.0f;
			newData->clr[eB] = initDat->b/255.0f;

			//the particle will start with 0, then later with MAX_GLOW_ALPHA
			newData->alphaStart = 0;
			newData->alphaEnd   = 1;

			newData->scaleStart = initDat->scaleStart;
			newData->scaleEnd   = initDat->scaleEnd;

			newData->bRepeat    = initDat->bRepeat;

			//set timer
			TimeInit(&newData->delayTimer, initDat->delay);

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			TextureDestroy(&thisData->glowTxt);
		}
		break;
	}

	return RETCODE_SUCCESS;
}