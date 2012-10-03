#include "gdix.h"
#include "gdix_i.h"

typedef enum
{
	eRIGHT,
	eLEFT,
	eBOTTOM,
	eTOP,
	eBACK,
	eFRONT,
	eNUMSIDE
} eFrustumSide;

//the six side frustrum plane
D3DXPLANE g_Frustum[eNUMSIDE];


/////////////////////////////////////
// Name:	FrustrumCalculate
// Purpose:	calculate the viewing
//			frustrum
// Output:	frustrum plane calculated
// Return:	none
/////////////////////////////////////
PUBLIC void FrustrumCalculate()
{
	D3DXMATRIX mtxClip, mtxView, mtxProj;

	g_p3DDevice->GetTransform(D3DTS_VIEW, (D3DMATRIX*)&mtxView);
	g_p3DDevice->GetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&mtxProj);

	D3DXMatrixMultiply(&mtxClip, &mtxView, &mtxProj);

	//set the right side
	g_Frustum[eRIGHT].a = mtxClip._14 - mtxClip._11;
	g_Frustum[eRIGHT].b = mtxClip._24 - mtxClip._21;
	g_Frustum[eRIGHT].c = mtxClip._34 - mtxClip._31;
	g_Frustum[eRIGHT].d = mtxClip._44 - mtxClip._41;

	//D3DXPlaneNormalize(&g_Frustum[eRIGHT], &g_Frustum[eRIGHT]);

	//set the left side
	g_Frustum[eLEFT].a = mtxClip._14 + mtxClip._11;
	g_Frustum[eLEFT].b = mtxClip._24 + mtxClip._21;
	g_Frustum[eLEFT].c = mtxClip._34 + mtxClip._31;
	g_Frustum[eLEFT].d = mtxClip._44 + mtxClip._41;

	//D3DXPlaneNormalize(&g_Frustum[eLEFT], &g_Frustum[eLEFT]);

	//set the bottom side
	g_Frustum[eBOTTOM].a = mtxClip._14 + mtxClip._12;
	g_Frustum[eBOTTOM].b = mtxClip._24 + mtxClip._22;
	g_Frustum[eBOTTOM].c = mtxClip._34 + mtxClip._32;
	g_Frustum[eBOTTOM].d = mtxClip._44 + mtxClip._42;

	//D3DXPlaneNormalize(&g_Frustum[eBOTTOM], &g_Frustum[eBOTTOM]);

	//set the top side
	g_Frustum[eTOP].a = mtxClip._14 - mtxClip._12;
	g_Frustum[eTOP].b = mtxClip._24 - mtxClip._22;
	g_Frustum[eTOP].c = mtxClip._34 - mtxClip._32;
	g_Frustum[eTOP].d = mtxClip._44 - mtxClip._42;

	//D3DXPlaneNormalize(&g_Frustum[eTOP], &g_Frustum[eTOP]);

	//set the back side
	g_Frustum[eBACK].a = mtxClip._14 - mtxClip._13;
	g_Frustum[eBACK].b = mtxClip._24 - mtxClip._23;
	g_Frustum[eBACK].c = mtxClip._34 - mtxClip._33;
	g_Frustum[eBACK].d = mtxClip._44 - mtxClip._43;

	//D3DXPlaneNormalize(&g_Frustum[eBACK], &g_Frustum[eBACK]);

	//set the front side
	g_Frustum[eFRONT].a = mtxClip._14 - mtxClip._13;
	g_Frustum[eFRONT].b = mtxClip._24 - mtxClip._23;
	g_Frustum[eFRONT].c = mtxClip._34 - mtxClip._33;
	g_Frustum[eFRONT].d = mtxClip._44 - mtxClip._43;

	//D3DXPlaneNormalize(&g_Frustum[eFRONT], &g_Frustum[eFRONT]);
}

/////////////////////////////////////
// Name:	FrustrumCheckPoint
// Purpose:	check point in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckPoint(float x, float y, float z)
{
	//just go through and plug-in the point to check if > 0
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(g_Frustum[i].a * x + g_Frustum[i].b * y + g_Frustum[i].c * z + g_Frustum[i].d <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	return true;
}

/////////////////////////////////////
// Name:	FrustrumCheckSphere
// Purpose:	check sphere in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckSphere(float x, float y, float z, float r)
{
	//just go through and plug-in the point to check within radius
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(g_Frustum[i].a * x + g_Frustum[i].b * y + g_Frustum[i].c * z + g_Frustum[i].d <= -r)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	return true;
}

/////////////////////////////////////
// Name:	FrustrumCheckCube
// Purpose:	check cube in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckCube(float x, float y, float z, float s)
{
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z - s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y - s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x - s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;
		if(g_Frustum[i].a * (x + s) + g_Frustum[i].b * (y + s) + g_Frustum[i].c * (z + s) + g_Frustum[i].d > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

/////////////////////////////////////
// Name:	FrustrumCheckBox
// Purpose:	check box in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckBox(float x1, float y1, float z1,
							 float x2, float y2, float z2)
{
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y1 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y1 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y2 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y2 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y1 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y1 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y2 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  continue;
		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y2 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

#define V_BOX_IN			1
#define V_BOX_OUT			2
#define V_BOX_INTERSECT		3

/////////////////////////////////////
// Name:	FrustrumCheckBoxEX
// Purpose:	check box in frustrum
// Output:	none
// Return:	BBOX_IN
//			BBOX_OUT
//			BBOX_INTERSECT
/////////////////////////////////////
PUBLIC int FrustrumCheckBoxEX(float x1, float y1, float z1,
							 float x2, float y2, float z2)
{
	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < eNUMSIDE; i++ )
	{
		mode &= V_BOX_OUT;				// clear the IN bit to 0 
		
		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y1 + g_Frustum[i].c * z1 + g_Frustum[i].d >= 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y1 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y2 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y2 + g_Frustum[i].c * z1 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y1 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y1 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x1 + g_Frustum[i].b * y2 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if(g_Frustum[i].a * x2 + g_Frustum[i].b * y2 + g_Frustum[i].c * z2 + g_Frustum[i].d > 0)  mode |= V_BOX_IN;
		else mode |= V_BOX_OUT;

		if(mode == V_BOX_INTERSECT) continue;

		if (mode == V_BOX_IN) continue;

		// If we get here, it isn't in the frustum
		return BBOX_OUT;
	}

	if (mode == V_BOX_INTERSECT) return BBOX_INTERSECT;

	return BBOX_IN;
}