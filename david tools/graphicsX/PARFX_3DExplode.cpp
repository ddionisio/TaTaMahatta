#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_3DEXPLODE_ALPHA		1

PROTECTED RETCODE fx3DExplodeFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fx3DExplode *thisData = (fx3DExplode *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			double tDelay = TimeGetTime(&thisData->delayTimer)/TimeGetDelay(&thisData->delayTimer);

			if(tDelay <= 1)
			{
				float pt[eMaxPt], vec[eMaxPt], fDir[eMaxPt], normal[eMaxPt], t = 1;

				//update alpha
				float alpha = MAX_3DEXPLODE_ALPHA - tDelay;

				for(int i = 0; i < thisData->maxParticle; i++)
				{
					//set the diffuse material
					GFXMATERIAL mat;

					for(int iMat = 0; iMat < OBJGetNumMaterial(thisData->particles[i].obj); iMat++)
					{
						OBJGetMaterial(thisData->particles[i].obj, iMat, &mat);

						mat.Diffuse.a = alpha;

						OBJSetMaterial(thisData->particles[i].obj, iMat, &mat);
					}

					//update particle 'X' direction
					thisData->particles[i].xDir[eX] += thisData->dirAcc[eX];
					thisData->particles[i].xDir[eY] += thisData->dirAcc[eY];
					thisData->particles[i].xDir[eZ] += thisData->dirAcc[eZ];

					//move the object
					fDir[eX] = thisData->particles[i].dir[eX]*thisData->spd + thisData->particles[i].xDir[eX];
					fDir[eY] = thisData->particles[i].dir[eY]*thisData->spd + thisData->particles[i].xDir[eY];
					fDir[eZ] = thisData->particles[i].dir[eZ]*thisData->spd + thisData->particles[i].xDir[eZ];

					OBJTranslate(thisData->particles[i].obj, fDir, true);

					OBJGetLoc(thisData->particles[i].obj, pt);

					vec[eX] = pt[eX] - thisData->particles[i].orgLoc[eX];
					vec[eY] = pt[eY] - thisData->particles[i].orgLoc[eY];
					vec[eZ] = pt[eZ] - thisData->particles[i].orgLoc[eZ];

					if(PARFXCollision(thisPARFX, thisData->particles[i].orgLoc, pt, &t, normal))
					{
						//bounce off
						float nd = normal[eX]*fDir[eX] + normal[eY]*fDir[eY] + normal[eZ]*fDir[eZ];

						D3DXVECTOR3 refl;
						refl.x = fDir[eX] - (2*nd*normal[eX]);
						refl.y = fDir[eY] - (2*nd*normal[eY]);
						refl.z = fDir[eZ] - (2*nd*normal[eZ]);

						D3DXVec3Normalize(&refl, &refl);

						thisData->particles[i].dir[eX] = refl.x;
						thisData->particles[i].dir[eY] = refl.y;
						thisData->particles[i].dir[eZ] = refl.z;

						//set the object location
						pt[eX] = thisData->particles[i].orgLoc[eX] + t*vec[eX];
						pt[eY] = thisData->particles[i].orgLoc[eY] + t*vec[eY];
						pt[eZ] = thisData->particles[i].orgLoc[eZ] + t*vec[eZ];

						OBJTranslate(thisData->particles[i].obj, pt, false);

						memcpy(thisData->particles[i].xDir, thisData->dir, sizeof(thisData->dir));
					}	
				}
			}
			else
				return RETCODE_BREAK;
		}
		break;

	case PARFXM_DISPLAY:
		{
			PARFXUnPrepare();

			for(int i = 0; i < thisData->maxParticle; i++)
			{
				OBJUpdateFrame(thisData->particles[i].obj);
				OBJDisplay(thisData->particles[i].obj);
			}

			PARFXPrepare();
		}
		break;

	case PARFXM_CREATE:
		{
			fx3DExplode_init *initDat = (fx3DExplode_init *)dumbParam;

			fx3DExplode *newData = (fx3DExplode *)GFX_MALLOC(sizeof(fx3DExplode));

			if(!newData)
				return RETCODE_FAILURE;

			//fill-in stuff
			newData->spd    = initDat->spd;
			
			memcpy(newData->dir, initDat->dir, sizeof(initDat->dir));
			memcpy(newData->dirAcc, initDat->dirAcc, sizeof(initDat->dirAcc));

			TimeInit(&newData->delayTimer, initDat->delay);

			newData->maxParticle = initDat->maxParticle;

			//allocate particles
			newData->particles = (fx3DExplodePar *)GFX_MALLOC(sizeof(fx3DExplodePar)*newData->maxParticle);

			if(!newData)
				return RETCODE_FAILURE;

			float center[eMaxPt] = {0};
			
			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
				PARFXGetOBJLoc(thisPARFX, obj, center);
			else
				memcpy(center, initDat->center, sizeof(center));

			D3DXVECTOR3 dir;

			//initialize particles
			for(int i = 0; i < newData->maxParticle; i++)
			{
				newData->particles[i].orgLoc[eX] = _GFXMathRand(initDat->min[eX], initDat->max[eX])+center[eX];
				newData->particles[i].orgLoc[eY] = _GFXMathRand(initDat->min[eY], initDat->max[eY])+center[eY];
				newData->particles[i].orgLoc[eZ] = _GFXMathRand(initDat->min[eZ], initDat->max[eZ])+center[eZ];

				newData->particles[i].obj = OBJCreate(0, initDat->mdl,
					newData->particles[i].orgLoc[eX],
					newData->particles[i].orgLoc[eY],
					newData->particles[i].orgLoc[eZ],
					eOBJ_STILL, 0);

				OBJActivate(newData->particles[i].obj, false);

				dir.x = _GFXMathRand(-1, 1);
				dir.y = _GFXMathRand(-1, 1);
				dir.z = _GFXMathRand(-1, 1);

				D3DXVec3Normalize(&dir, &dir);

				newData->particles[i].dir[eX] = dir.x;
				newData->particles[i].dir[eY] = dir.y;
				newData->particles[i].dir[eZ] = dir.z;

				memcpy(newData->particles[i].xDir, newData->dir, sizeof(newData->dir));
			}

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
		}
		break;
	}

	return RETCODE_SUCCESS;
}