#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

#define MAX_EXPAND_ALPHA		255

//this is only used by this
typedef struct _expandVtx {
    float x, y, z; //model space vertex position
    DWORD color;   //the color!
	float s, t;	   //texture coord
} expandVtx;

#define TXT_EXPANDVTX_USAGE	D3DUSAGE_WRITEONLY
#define TXT_EXPANDVTX_POOL	D3DPOOL_MANAGED

#define MAXEXPANDVTX 4

#define GFXEXPANDVTXFLAG (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1) //xyz and color and texture

PRIVATE DWORD _expandGetMajorAxis(const D3DXVECTOR3 & normal, D3DXVECTOR3 & mAxis)
{
	float xAbs = abs(normal.x);
	float yAbs = abs(normal.y);
	float zAbs = abs(normal.z);

	DWORD major = xAbs > yAbs ? xAbs > zAbs ? eX : eZ : yAbs > zAbs ? eY : eZ;

	switch(major)
	{
	case eX:
		mAxis.x = 1; mAxis.y = mAxis.z = 0;
		break;
	case eY:
		mAxis.y = 1; mAxis.x = mAxis.z = 0;
		break;
	case eZ:
		mAxis.z = 1; mAxis.x = mAxis.y = 0;
		break;
	}

	return major;
}

PROTECTED RETCODE fxExpandFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fxExpand *thisData = (fxExpand *)thisPARFX->data;

	expandVtx *pVtx;

	switch(message)
	{
	case PARFXM_UPDATE:
		{
			hOBJ obj = PARFXGetOBJ(thisPARFX);

			if(obj)
				PARFXGetOBJLoc(thisPARFX, obj, thisData->center);

			double t = TimeGetTime(&thisData->delayTimer)/TimeGetDelay(&thisData->delayTimer);
			
			float  alpha;

			alpha = MAX_EXPAND_ALPHA - (t*MAX_EXPAND_ALPHA);

			thisData->scale = thisData->scaleStart + t*(thisData->scaleEnd-thisData->scaleStart);

			if(alpha > 0 && thisData->scale > 0)
			{
				BYTE alphaB = alpha > 255 ? 255 : alpha;
				thisData->clr = D3DCOLOR_RGBA(thisData->r,thisData->g,thisData->b,alphaB);

				if(SUCCEEDED(thisData->vtxBuff->Lock(0,0,(void**)&pVtx,0)))
				{
					pVtx[0].color = thisData->clr;
					pVtx[1].color = thisData->clr;
					pVtx[2].color = thisData->clr;
					pVtx[3].color = thisData->clr;

					thisData->vtxBuff->Unlock();
				}
			}
			else if(!thisData->bRepeat 
				|| dumbParam == PARFX_UPDATE_DEAD)
				return RETCODE_BREAK;
			else
				TimeReset(&thisData->delayTimer);
		}
		break;

	case PARFXM_DISPLAY:
		{
			D3DXMATRIX trans;

			//do transformations
			D3DXMatrixIdentity(&trans);

			trans._11 = thisData->scale;
			trans._22 = thisData->scale;
			trans._33 = thisData->scale;

			trans._41 = thisData->center[eX];
			trans._42 = thisData->center[eY];
			trans._43 = thisData->center[eZ];

			g_p3DDevice->SetTransform(D3DTS_WORLD, &trans);

			//set vtx shader...
			g_p3DDevice->SetFVF(GFXEXPANDVTXFLAG);

			g_p3DDevice->SetStreamSource(0, thisData->vtxBuff, 0, sizeof(expandVtx));

			//set texture
			TextureSet(thisData->expandTxt, 0);

			//draw
			if(_GFXCheckError(g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2),
				true, "Error in TextureBlt3D"))
				return RETCODE_FAILURE;

			g_p3DDevice->SetStreamSource(0,0,0,0);
			g_p3DDevice->SetTexture(0,0);
		}
		break;

	case PARFXM_CREATE:
		{
			fxExpand_init *initDat = (fxExpand_init *)dumbParam;

			fxExpand *newData = (fxExpand *)GFX_MALLOC(sizeof(fxExpand));

			if(!newData)
				return RETCODE_FAILURE;

			//fill-in stuff
			newData->expandTxt = initDat->expandTxt; TextureAddRef(newData->expandTxt);

			memcpy(newData->center, initDat->center, sizeof(initDat->center));

			newData->r           = initDat->r;
			newData->g           = initDat->g;
			newData->b           = initDat->b;

			newData->scaleStart	 = initDat->scaleStart;
			newData->scaleEnd    = initDat->scaleEnd;

			newData->bRepeat	 = initDat->bRepeat;

			TimeInit(&newData->delayTimer, initDat->delay);

			//create the vertex buffer
			if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(expandVtx)*MAXEXPANDVTX,
				TXT_EXPANDVTX_USAGE, GFXEXPANDVTXFLAG, TXT_EXPANDVTX_POOL, &newData->vtxBuff, 0),
				true, "fxExpandFunc"))
				return RETCODE_FAILURE;

			//create the up and right vector from normal
			D3DXVECTOR3 right, up, mAxis, normal(initDat->normal[eX],initDat->normal[eY], initDat->normal[eZ]);

			//get major axis
			DWORD major = _expandGetMajorAxis(normal, mAxis);

			//now get the right vector
			if(normal == mAxis)
			{
				switch(major)
				{
				case eX:
				case eZ:
					right.y = 1; right.x = right.z = 0;
					break;
				case eY:
					right.x = 1; right.y = right.z = 0;
					break;
				}
			}
			else
				D3DXVec3Cross(&right, &mAxis, &normal);

			//get the up vector
			D3DXVec3Cross(&up, &normal, &right);

			D3DXVec3Normalize(&right, &right);
			D3DXVec3Normalize(&up, &up);

			//fill in the vertices
			if(SUCCEEDED(newData->vtxBuff->Lock(0,0,(void**)&pVtx,0)))
			{
				pVtx[0].x =  -(right.x+up.x);
				pVtx[0].y =  -(right.y+up.y);
				pVtx[0].z =  -(right.z+up.z);

				pVtx[1].x =  (right.x-up.x);
				pVtx[1].y =  (right.y-up.y);
				pVtx[1].z =  (right.z-up.z);

				pVtx[2].x =  (right.x+up.x);
				pVtx[2].y =  (right.y+up.y);
				pVtx[2].z =  (right.z+up.z);

				pVtx[3].x =  -(right.x-up.x);
				pVtx[3].y =  -(right.y-up.y);
				pVtx[3].z =  -(right.z-up.z);

				//Top Left
				pVtx[0].s = 0;
				pVtx[0].t = 1;
				//Bottom Left
				pVtx[1].s = 1;
				pVtx[1].t = 1;
				//Bottom Right
				pVtx[2].s = 1;
				pVtx[2].t = 0;
				//Top Right
				pVtx[3].s = 0;
				pVtx[3].t = 0;

				newData->vtxBuff->Unlock();
			}

			thisPARFX->data = newData;
		}
		break;

	case PARFXM_DESTROY:
		if(thisData)
		{
			TextureDestroy(&thisData->expandTxt);

			if(thisData->vtxBuff)
				thisData->vtxBuff->Release();
		}
		break;
	}

	return RETCODE_SUCCESS;
}