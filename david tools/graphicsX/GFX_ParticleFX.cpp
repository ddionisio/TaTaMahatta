#include "gdix_particlefx.h"

#include "gdix_i.h"

#include "gdix_particlefx_i.h"

list<hPARFX> g_parFXList;

/**********************************************************
***********************************************************

  The methods for all FX

***********************************************************
**********************************************************/

//
// A bunch of FX functions
//
PRIVATE PAREFFECT ParEffectTable[ePARFX_MAX] = {fxPuffFunc, fxExpandFunc, fxExplodeFunc, fxGlowFunc, fxGasFunc, fx3DExplodeFunc, fxSmokeFunc, fxGatherFunc, fxLightningYFunc};

PRIVATE void _PARFXDestroy(hPARFX *parFX)
{
	if(*parFX)
	{
		g_parFXList.remove(*parFX);

		if((*parFX)->Effect)
			(*parFX)->Effect(*parFX, PARFXM_DESTROY, 0, 0);

		if((*parFX)->data)
			GFX_FREE((*parFX)->data);

		GFX_FREE(*parFX);
	}

	parFX=0;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void PARFXDestroy(hPARFX *parFX)
{
	if(*parFX)
	{
		SETFLAG((*parFX)->status, PARFX_FLAG_DONE);
	}

	parFX=0;
}

PUBLIC void PARFXDestroyAll()
{
	hPARFX parFX;

	list<hPARFX>::iterator i;

	while(g_parFXList.size() > 0)
	{
		parFX = g_parFXList.back();
		g_parFXList.remove(parFX);
		_PARFXDestroy(&parFX);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hPARFX PARFXCreate(ePARFXType type, void *initStruct, double durDelay, 
						  hOBJ objAttach, int objJointInd, COLLISIONFUNC collision)
{
	hPARFX newParFX = (hPARFX)GFX_MALLOC(sizeof(gfxParticleFX));

	if(!newParFX) { ASSERT_MSG(0, "Unable to allocate new particle", "PARFXCreate"); return 0; }

	newParFX->type = type;

	newParFX->collision = collision;

	if(objAttach)
	{
		newParFX->objKey	   = OBJGetKey(objAttach);
		
		SETFLAG(newParFX->status, PARFX_FLAG_OBJATTACHED);
	}

	newParFX->objJointInd = objJointInd;

	newParFX->Effect = ParEffectTable[type];

	if(durDelay != -1)
		TimeInit(&newParFX->ticker, durDelay);
	else
		SETFLAG(newParFX->status, PARFX_FLAG_INFINITE);

	if(newParFX->Effect(newParFX, PARFXM_CREATE, (LPARAM)initStruct, 0) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to initialize new particle", "PARFXCreate"); PARFXDestroy(&newParFX); return 0; }

	g_parFXList.push_back(newParFX);

	return newParFX;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdate(hPARFX parFX)
{
	RETCODE ret=RETCODE_SUCCESS;

	//check to see if the timer is done and that it is not infinite
	//or if the host object has been destroyed
	if((TimeElapse(&parFX->ticker) && !TESTFLAGS(parFX->status, PARFX_FLAG_INFINITE))
		|| (TESTFLAGS(parFX->status, PARFX_FLAG_OBJATTACHED) && !PARFXGetOBJ(parFX)))
		SETFLAG(parFX->status, PARFX_FLAG_DONE);

	if(TESTFLAGS(parFX->status, PARFX_FLAG_DONE)
		|| TESTFLAGS(parFX->status, PARFX_FLAG_DISABLE))
		ret = parFX->Effect(parFX, PARFXM_UPDATE, PARFX_UPDATE_DEAD, 0);
	else
		ret = parFX->Effect(parFX, PARFXM_UPDATE, 0, 0);

	return ret;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplay(hPARFX parFX)
{
	return parFX->Effect(parFX, PARFXM_DISPLAY, 0, 0);
}

/////////////////////////////////////
// Name:	PARFXUpdateAll
// Purpose:	updates all particles
//			created.
//			NOTE: This will destroy
//				  any particle that has
//				  expired
// Output:	stuff updated
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdateAll()
{
	hPARFX parFX;

	list<hPARFX>::iterator i_next;

	for(list<hPARFX>::iterator i = g_parFXList.begin(); i != g_parFXList.end(); i=i_next)
	{
		i_next=i; i_next++;

		parFX = *i;

		if(!TESTFLAGS(parFX->status, PARFX_FLAG_DISABLE_DONE))
		{
			if(PARFXUpdate(parFX)==RETCODE_BREAK)
			{
				if(!TESTFLAGS(parFX->status, PARFX_FLAG_DISABLE))
					_PARFXDestroy(&parFX);
				else
					SETFLAG(parFX->status, PARFX_FLAG_DISABLE_DONE);
			}
		}
	}

	return RETCODE_SUCCESS;
}

DWORD g_srcBlend, g_destBlend, g_zWrite;

PROTECTED void PARFXPrepare()
{
	//assume all particles to use this render state
	//if(GFXIsLightEnabled())
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

    //disable culling
    g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	g_p3DDevice->GetRenderState( D3DRS_SRCBLEND, &g_srcBlend );
	g_p3DDevice->GetRenderState( D3DRS_DESTBLEND, &g_destBlend );
	g_p3DDevice->GetRenderState( D3DRS_ZWRITEENABLE, &g_zWrite);

	g_p3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
}

PROTECTED void PARFXUnPrepare()
{
	//enable light again, if flag is set
    //if(GFXIsLightEnabled())
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

    //enable culling
	g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	g_p3DDevice->SetRenderState( D3DRS_SRCBLEND, g_srcBlend );
	g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, g_destBlend );
	g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, g_zWrite);
}

/////////////////////////////////////
// Name:	PARFXDisplayAll
// Purpose:	displays all particles
//			created.
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplayAll()
{
	PARFXPrepare();

	hPARFX parFX;

	for(list<hPARFX>::iterator i = g_parFXList.begin(); i != g_parFXList.end(); ++i)
	{
		parFX = *i;
		
		if(!TESTFLAGS(parFX->status, PARFX_FLAG_DISABLE_DONE))
		{
			PARFXDisplay(parFX);
		}
	}

	PARFXUnPrepare();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	PARFXActivate
// Purpose:	activate/deactivate given
//			particle
// Output:	particle deactivated
// Return:	none
/////////////////////////////////////
PUBLIC void PARFXActivate(hPARFX parFX, bool bActive)
{
	if(bActive)
	{
		CLEARFLAG(parFX->status, PARFX_FLAG_DISABLE_DONE);
		CLEARFLAG(parFX->status, PARFX_FLAG_DISABLE);
	}
	else
	{
		SETFLAG(parFX->status, PARFX_FLAG_DISABLE);
	}
}

/////////////////////////////////////
// Name:	PARFXIsActive
// Purpose:	check to see if the given
//			particle FX is active
// Output:	none
// Return:	true if active
/////////////////////////////////////
PUBLIC bool PARFXIsActive(hPARFX parFX)
{
	if(TESTFLAGS(parFX->status, PARFX_FLAG_DISABLE_DONE | PARFX_FLAG_DISABLE))
		return false;

	return true;
}

//
// Particle FX internal functions
//

PROTECTED hOBJ PARFXGetOBJ(hPARFX parFX)
{
	if(TESTFLAGS(parFX->status, PARFX_FLAG_OBJATTACHED))
		return OBJQuery(&parFX->objKey);

	return 0;
}

PROTECTED void PARFXGetOBJLoc(hPARFX parFX, hOBJ obj, float ptOut[eMaxPt])
{
	/*OBJGetLoc(obj, ptOut);

	if(parFX->objJointInd != -1)
	{
		float jLoc[eMaxPt];

		OBJJointGetTranslate(obj, parFX->objJointInd, jLoc);

		ptOut[eX] += jLoc[eX];
		ptOut[eY] += jLoc[eY];
		ptOut[eZ] += jLoc[eZ];
	}*/
	
	if(parFX->objJointInd != -1)
	{
		OBJJointGetWorldLoc(obj, parFX->objJointInd, ptOut);
	}
	else
		OBJGetWorldLoc(obj, ptOut);
		
}

PROTECTED bool PARFXCheckStatus(hPARFX parFX, DWORD status)
{
	return TESTFLAGS(parFX->status, status);
}

//return true when hit
PROTECTED bool PARFXCollision(hPARFX parFX, float pt1[eMaxPt], float pt2[eMaxPt], float *tOut, float nOut[eMaxPt])
{
	if(parFX->collision)
		return parFX->collision(pt1, pt2, tOut, nOut);

	return false;
}
