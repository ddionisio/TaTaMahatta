#include "gdix.h"
#include "gdix_i.h"

typedef struct _gfxLight {
	gfxID		ID;
	GFXLIGHT	dat;

	int			lightInd;	//-1 if not set, otherwise, which index.
	float		dist;		//distance used for LightEnableNearest
} gfxLight;

gfxLight *g_lights=0;
int g_numLight=0;

int g_maxLight = MAXLIGHT;

/////////////////////////////////////
// Name:	GFXSetMaxLight
// Purpose:	sets the max light, must be
//			at max 8
// Output:	max light set
// Return:	
/////////////////////////////////////
PUBLIC void GFXSetMaxLight(int numLight)
{
	if(numLight <= MAXLIGHT)
		g_maxLight = numLight;
}

/////////////////////////////////////
// Name:	GFXEnableLight
// Purpose:	enable/disable light
//			also set ambient color
// Output:	light is enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnableLight(bool do_you_want_light)
{
	//set light
	if(do_you_want_light)
	{
		g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
		CLEARFLAG(g_FLAGS, GFX_LIGHTDISABLE);
	}
	else
	{
		g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		SETFLAG(g_FLAGS, GFX_LIGHTDISABLE);
	}
}

/////////////////////////////////////
// Name:	GFXIsLightEnabled
// Purpose:	is light enabled?
// Output:	none
// Return:	true if so...
/////////////////////////////////////
PUBLIC bool GFXIsLightEnabled()
{
	return !TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE);
}

/////////////////////////////////////
// Name:	Light(Set/Get)Ambient
// Purpose:	set/get the ambient
// Output:	ambient if 'Get'
// Return:	none
/////////////////////////////////////
PUBLIC void LightSetAmbient(DWORD ambient)
{
	g_p3DDevice->SetRenderState(D3DRS_AMBIENT,ambient);
}

PUBLIC void LightGetAmbient(DWORD * ambient)
{
	g_p3DDevice->GetRenderState(D3DRS_AMBIENT, ambient);
}

/////////////////////////////////////
// Name:	LightIsEnable
// Purpose:	checks to see if given
//			light is enabled
// Output:	none
// Return:	TRUE if enabled
/////////////////////////////////////
PUBLIC BOOL LightIsEnable(const gfxID * ID)
{
	gfxLight *pLight = (gfxLight *)GFXPageQuery(ID);
	if(pLight && pLight->lightInd != -1)
	{
		BOOL bEnable; g_p3DDevice->GetLightEnable(pLight->lightInd, &bEnable);
		return bEnable;
	}

	return FALSE;
}

/////////////////////////////////////
// Name:	LightEnable
// Purpose:	enables/disables light
// Output:	enable/disable light
// Return:	none
/////////////////////////////////////
PUBLIC void LightEnable(const gfxID * ID, DWORD lightInd, BOOL enable)
{
	gfxLight *pLight = (gfxLight *)GFXPageQuery(ID);

	if(pLight)
	{
		if(pLight->lightInd != -1)
			g_p3DDevice->LightEnable(pLight->lightInd, FALSE);

		pLight->lightInd = lightInd;

		if(enable)
			g_p3DDevice->SetLight(lightInd, &pLight->dat);

		g_p3DDevice->LightEnable(lightInd, enable);
	}
}

/////////////////////////////////////
// Name:	LightSet
// Purpose:	set light
// Output:	light is set
// Return:	none
/////////////////////////////////////
PUBLIC void LightSet(const gfxID * ID, GFXLIGHT *lightStruct)
{
	gfxLight *pLight = (gfxLight *)GFXPageQuery(ID);

	if(pLight)
		memcpy(&pLight->dat, lightStruct, sizeof(GFXLIGHT));
}

PUBLIC void LightSet(DWORD lightInd, GFXLIGHT *lightStruct, BOOL enable)
{
	g_p3DDevice->SetLight(lightInd, lightStruct);
	g_p3DDevice->LightEnable(lightInd, enable);
}

/////////////////////////////////////
// Name:	LightCreate
// Purpose:	adds new light to GFX
// Output:	increment number of light
// Return:	ID of new light
/////////////////////////////////////
PUBLIC gfxID LightCreate(GFXLIGHT *lightStruct)
{
	int newInd = g_numLight;

	gfxLight *pNewLightList = (gfxLight *)GFX_MALLOC(sizeof(gfxLight)*(g_numLight+1));

	if(pNewLightList)
	{
		if(g_numLight > 0)
			memcpy(pNewLightList, g_lights, sizeof(gfxLight)*g_numLight);

		GFXPageAdd(&pNewLightList[newInd], &pNewLightList[newInd].ID);
		memcpy(&pNewLightList[newInd].dat, lightStruct, sizeof(GFXLIGHT));

		GFX_FREE(g_lights);

		g_lights = pNewLightList;

		g_numLight++;

		return pNewLightList[newInd].ID;
	}

	gfxID badID; badID.ID = 0; badID.counter = -1;
	return badID;
}

/////////////////////////////////////
// Name:	LightReset
// Purpose:	disables all light and
//			resets number of lights
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void LightReset()
{
	if(g_p3DDevice)
		for(int i = 0; i < MAXLIGHT; i++)
			g_p3DDevice->LightEnable(i, FALSE);
}

/////////////////////////////////////
// Name:	LightDestroy
// Purpose:	removes light
// Output:	decrease number of light
//			disables given light ind
// Return:	none
/////////////////////////////////////
PUBLIC void LightDestroy(const gfxID * ID)
{
	gfxLight *pLight = (gfxLight *)GFXPageQuery(ID);

	if(pLight)
	{
		GFXPageRemove(&pLight->ID);

		if(pLight->lightInd != -1)
			g_p3DDevice->LightEnable(pLight->lightInd, FALSE);

		gfxLight *pNewLightList = (gfxLight *)GFX_MALLOC(sizeof(gfxLight)*(g_numLight-1));

		if(pNewLightList)
		{
			for(int i = 0, j = 0; i < g_numLight; i++)
			{
				if(!GFXID_IS_EQUAL(g_lights[i].ID, pLight->ID))
				{
					memcpy(&pNewLightList[j], &g_lights[i], sizeof(gfxLight));
					j++;
				}
			}

			GFX_FREE(g_lights);

			g_lights = pNewLightList;

			g_numLight--;
		}
	}
}

/////////////////////////////////////
// Name:	LightDestroyAll
// Purpose:	destroys all light, making all
//			lights invalid
// Output:	all lights dead
// Return:	none
/////////////////////////////////////
PUBLIC void LightDestroyAll()
{
	if(g_lights)
	{
		for(int i = 0; i < g_numLight; i++)
			GFXPageRemove(&g_lights[i].ID);

		GFX_FREE(g_lights);

		g_lights = 0;
	}

	g_numLight = 0;

	LightReset();
}

/////////////////////////////////////
// Name:	LightGetData
// Purpose:	grabs given light's data
// Output:	lightStruct is filled
// Return:	none
/////////////////////////////////////
PUBLIC void LightGetData(const gfxID * ID, GFXLIGHT *lightStruct)
{
	gfxLight *pLight = (gfxLight *)GFXPageQuery(ID);

	if(pLight)
		memcpy(lightStruct, &pLight->dat, sizeof(GFXLIGHT));
}

int _LightCompare( const void *arg1, const void *arg2 )
{
	gfxLight *l1 = (gfxLight *)arg1;
	gfxLight *l2 = (gfxLight *)arg2;

	if(l1->dist < l2->dist)
		return -1;
	else if(l1->dist == -1
		|| l1->dist > l2->dist)
		return 1;

	return 0;
}

/////////////////////////////////////
// Name:	LightEnableAllNearest
// Purpose:	enable lights that are near
//			location.
// Output:	lights enabled
// Return:	none
/////////////////////////////////////
PUBLIC void LightEnableAllNearest(const float loc[eMaxPt])
{
	if(g_lights && g_numLight > 0)
	{
		int curMax = 0, i;

		float v[eMaxPt];

		for(i = 0; i < g_numLight; i++)
		{
			if(g_lights[i].dat.Type == D3DLIGHT_POINT
				|| g_lights[i].dat.Type  == D3DLIGHT_SPOT)
			{
				v[eX] = loc[eX] - g_lights[i].dat.Position.x;
				v[eY] = loc[eY] - g_lights[i].dat.Position.y;
				v[eZ] = loc[eZ] - g_lights[i].dat.Position.z;

				g_lights[i].dist = v[eX]*v[eX] + v[eY]*v[eY] + v[eZ]*v[eZ];
			}
			else
				g_lights[i].dist = -1;
		}

		//sort
		qsort(g_lights, g_numLight, sizeof(gfxLight), _LightCompare);
		
		int max = g_maxLight < g_numLight ? g_maxLight : g_numLight;
		for(i = 0; i < max; i++)
			LightSet(i, &g_lights[i].dat, TRUE);
	}

	//LightReset();

	//gfxLight *pLight;
	/*for(int i = 0; i < g_numLight; i++)
	{
		//don't add anymore light...
		if(curMax == g_maxLight)
			break;

		pLight = &g_lights[i];

		if(pLight 
			&& (pLight->dat.Type == D3DLIGHT_POINT || pLight->dat.Type == D3DLIGHT_SPOT))
		{
			v[eX] = loc[eX] - pLight->dat.Position.x;
			v[eY] = loc[eY] - pLight->dat.Position.y;
			v[eZ] = loc[eZ] - pLight->dat.Position.z;

			light_len_sq = v[eX]*v[eX] + v[eY]*v[eY] + v[eZ]*v[eZ];

			if(light_len_sq <= pLight->dat.Range*pLight->dat.Range)
			{
				LightSet(curMax, &pLight->dat, TRUE);
				curMax++;
			}
		}
	}*/
}