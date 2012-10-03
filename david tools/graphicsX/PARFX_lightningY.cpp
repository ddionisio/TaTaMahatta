#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

PROTECTED RETCODE fxLightningYFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxLightningY *thisData = (fxLightningY *)thisPARFX->data;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			double t = TimeGetTime(&thisData->onTimer)/TimeGetDelay(&thisData->onTimer);

			if(t > 1){ t = 1; TimeReset(&thisData->onTimer); }

			//update alpha
			if(thisData->clr[eA] < 1
				&& dumbParam != PARFX_UPDATE_DEAD)
			{
				thisData->clr[eA] = t;

				if(thisData->clr[eA] == 1)
				{
					TimeReset(&thisData->onTimer);
				}
			}
			else if(thisData->clr[eA] > 0
				&& dumbParam == PARFX_UPDATE_DEAD)
			{
				thisData->clr[eA] = 1.0f - t;

				if(thisData->clr[eA] == 0)
				{
					TimeReset(&thisData->onTimer);
				}
			}

			/////////////////////////////////////////////////////////////////
			//update nodes
			float trans[eMaxPt] = {0};

			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
			{
				PARFXGetOBJLoc(thisPARFX, obj, trans);
			}

			for(int i = 0; i < thisData->numNodes; i++)
			{
				t = TimeGetTime(&thisData->Nodes[i].timeMove)/TimeGetDelay(&thisData->Nodes[i].timeMove);

				if(t > 1) t = 1;

				thisData->Nodes[i].curLoc[eX] = thisData->Nodes[i].sLoc[eX] + t*(thisData->Nodes[i].eLoc[eX] - thisData->Nodes[i].sLoc[eX]);
				thisData->Nodes[i].curLoc[eZ] = thisData->Nodes[i].sLoc[eZ] + t*(thisData->Nodes[i].eLoc[eZ] - thisData->Nodes[i].sLoc[eZ]);

				if(t == 1)
				{
					TimeReset(&thisData->Nodes[i].timeMove);

					//our new start
					memcpy(thisData->Nodes[i].sLoc, thisData->Nodes[i].curLoc, sizeof(thisData->Nodes[i].sLoc));

					//our new end
					thisData->Nodes[i].eLoc[eX] = _GFXMathRand(thisData->min[eX], thisData->max[eX]) + trans[eX];
					thisData->Nodes[i].eLoc[eY] = thisData->min[eY] + ((float)i/(float)thisData->numNodes)*(thisData->max[eY] - thisData->min[eY]) + trans[eY];
					thisData->Nodes[i].eLoc[eZ] = _GFXMathRand(thisData->min[eZ], thisData->max[eZ]) + trans[eZ];
				}
			}

			if(thisData->clr[eA] == 0 && dumbParam == PARFX_UPDATE_DEAD)
			{
				return RETCODE_BREAK;
			}
		}
		break;

	case PARFXM_DISPLAY:
		if(FrustrumCheckBox(
					thisData->min[eX], thisData->min[eY], thisData->min[eZ],
					thisData->max[eX], thisData->max[eY], thisData->max[eZ]))
		{
			float t = 0, max=thisData->maxParticle;

			D3DXVECTOR3 ptOut, pt0, pt1, pt2, pt3;

			float particleCount=0;
			int prevInd=0, ind=0, nextInd=1, next2Ind=2;

			do
			{
				memcpy((float*)pt0, thisData->Nodes[prevInd].curLoc, sizeof(thisData->Nodes[prevInd].curLoc));
				memcpy((float*)pt1, thisData->Nodes[ind].curLoc, sizeof(thisData->Nodes[ind].curLoc));
				memcpy((float*)pt2, thisData->Nodes[nextInd].curLoc, sizeof(thisData->Nodes[nextInd].curLoc));
				memcpy((float*)pt3, thisData->Nodes[next2Ind].curLoc, sizeof(thisData->Nodes[next2Ind].curLoc));

				//cat-mull-rom curve magic
				D3DXVec3CatmullRom(&ptOut, &pt0, &pt1, &pt2, &pt3, t);

				//draw the darn thing
				TextureBltBillboard(thisData->lightningTxt, (float*)ptOut, thisData->scale, thisData->clr);

				particleCount++;

				t = particleCount/max;

				if(t > 1)
				{
					ind++;
					particleCount = 0;
					t = 0;
				}

				prevInd  = ind - 1; if(prevInd  < 0) prevInd = 0;
				nextInd  = ind + 1; if(nextInd  >= thisData->numNodes) nextInd  = thisData->numNodes-1;
				next2Ind = ind + 2; if(next2Ind >= thisData->numNodes) next2Ind = thisData->numNodes-1;

			} while(ind < thisData->numNodes);
		}
		break;

	case PARFXM_CREATE:
		{
			fxLightningY_init *initDat = (fxLightningY_init *)dumbParam;

			fxLightningY *newData = (fxLightningY *)GFX_MALLOC(sizeof(fxLightningY));

			if(!newData)
				return RETCODE_FAILURE;

			newData->lightningTxt = initDat->lightningTxt; TextureAddRef(newData->lightningTxt);

			newData->clr[eR] = initDat->r/255.0f;
			newData->clr[eG] = initDat->g/255.0f;
			newData->clr[eB] = initDat->b/255.0f;
			newData->clr[eA] = 0;

			newData->scale = initDat->scale;

			memcpy(newData->min, initDat->min, sizeof(newData->min));
			memcpy(newData->max, initDat->max, sizeof(newData->max));

			newData->maxParticle = 20;//initDat->maxParticle;

			TimeInit(&newData->onTimer, initDat->delayOn);
			TimeReset(&newData->onTimer);

			newData->delayMove = initDat->delayMove;

			newData->numNodes = initDat->maxPoint;

			newData->Nodes = (lightningNode *)GFX_MALLOC(sizeof(lightningNode)*newData->numNodes);

			if(!newData->Nodes)
				return RETCODE_FAILURE;

			////////////////////////////////////////////////////////////////
			//initialize the nodes
			float trans[eMaxPt] = {0};

			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
			{
				PARFXGetOBJLoc(thisPARFX, obj, trans);
			}

			for(int i = 0; i < newData->numNodes; i++)
			{
				newData->Nodes[i].sLoc[eX] = _GFXMathRand(newData->min[eX], newData->max[eX]) + trans[eX];
				newData->Nodes[i].sLoc[eY] = newData->min[eY] + ((float)i/(float)newData->numNodes)*(newData->max[eY] - newData->min[eY]) + trans[eY];
				newData->Nodes[i].sLoc[eZ] = _GFXMathRand(newData->min[eZ], newData->max[eZ]) + trans[eZ];

				memcpy(newData->Nodes[i].curLoc, newData->Nodes[i].sLoc, sizeof(newData->Nodes[i].curLoc));

				newData->Nodes[i].eLoc[eX] = _GFXMathRand(newData->min[eX], newData->max[eX]) + trans[eX];
				newData->Nodes[i].eLoc[eY] = newData->min[eY] + ((float)i/(float)newData->numNodes)*(newData->max[eY] - newData->min[eY]) + trans[eY];
				newData->Nodes[i].eLoc[eZ] = _GFXMathRand(newData->min[eZ], newData->max[eZ]) + trans[eZ];

				TimeInit(&newData->Nodes[i].timeMove, newData->delayMove);
			}

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		{
			if(thisData)
			{
				if(thisData->Nodes)
					GFX_FREE(thisData->Nodes);

				if(thisData->lightningTxt)
					TextureDestroy(&thisData->lightningTxt);
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}