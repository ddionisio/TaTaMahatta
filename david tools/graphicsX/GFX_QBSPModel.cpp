#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

/////////////////////////////////////
// Name:	QBSPRayModelGetBBoxMin
// Purpose:	get the bbox
// Output:	pOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPRayModelGetBBoxMin(hQBSP qbsp, int modelInd, float pOut[eMaxPt])
{
	pOut[eX] = qbsp->models[modelInd].min[eX]+qbsp->models[modelInd].wrldTrans[eX];
	pOut[eY] = qbsp->models[modelInd].min[eY]+qbsp->models[modelInd].wrldTrans[eY];
	pOut[eZ] = qbsp->models[modelInd].min[eZ]+qbsp->models[modelInd].wrldTrans[eZ];
}

/////////////////////////////////////
// Name:	QBSPRayModelGetBBoxMax
// Purpose:	get the bbox
// Output:	pOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPRayModelGetBBoxMax(hQBSP qbsp, int modelInd, float pOut[eMaxPt])
{
	pOut[eX] = qbsp->models[modelInd].max[eX]+qbsp->models[modelInd].wrldTrans[eX];
	pOut[eY] = qbsp->models[modelInd].max[eY]+qbsp->models[modelInd].wrldTrans[eY];
	pOut[eZ] = qbsp->models[modelInd].max[eZ]+qbsp->models[modelInd].wrldTrans[eZ];
}

/////////////////////////////////////
// Name:	QBSPRayModelGetRadius
// Purpose:	get radius of the model
// Output:	none
// Return:	the radius
/////////////////////////////////////
PUBLIC float QBSPRayModelGetRadius(hQBSP qbsp, int modelInd)
{
	return qbsp->models[modelInd].radius;
}

PRIVATE void _CollBrush(ConvexPolyhedron *pBrush, bool bIsPt, 
							  float mins[eMaxPt], float maxs[eMaxPt], 
							  float p[eMaxPt], float dir[eMaxPt], 
							  gfxTrace *pTrace)
{
	int txtInd = -1;

	//go through each half-space and determine the interval t[min,max]
	float tMin=0, tMax=1;

	float vDotn, pDotn, tInt;

	float *pPlane, ofs[eMaxPt], dist;

	int indMin=0, indMax=0, j;

	//go through the sides
	for(int i = 0; i < pBrush->numPlane; i++)
	{
		if(tMin > tMax) //no intersect!
			break;

		pPlane = (float*)pBrush->tPlanes[i];

		if(!bIsPt)
		{	// general box case
			// push the plane out apropriately for mins/maxs

			for (j=0 ; j<eMaxPt ; j++)
			{
				if (pPlane[j] < 0)
					ofs[j] = maxs[j];
				else
					ofs[j] = mins[j];
			}
			dist = pPlane[eX]*ofs[eX] + pPlane[eY]*ofs[eY] + pPlane[eZ]*ofs[eZ];
			dist += pPlane[ePD];//-((-pPlane[ePD]) - dist);
		}
		else
		{	// special point case
			dist = pPlane[ePD];
		}

		//normal dot ray vector
		vDotn = pPlane[eX]*dir[eX] + pPlane[eY]*dir[eY] + pPlane[eZ]*dir[eZ];

		pDotn = pPlane[eX]*p[eX] + pPlane[eY]*p[eY] + pPlane[eZ]*p[eZ];

		if(vDotn < 0)
		{
			tInt = -(pDotn + dist - DIST_EPSILON)/vDotn;

			if (tInt > tMin) {
			  tMin = tInt;
			  indMin = i;
			}
			
		}
		else if(vDotn > 0)
		{
			tInt = -(pDotn + dist + DIST_EPSILON)/vDotn;

			if(tInt < tMax)
			{
				tMax = tInt;
				indMax = i;
			}

		}
		else
		{
			if(pDotn + dist > 0)
				tMin = tMax+1;
		}
	}

	//now let's see if we intersect something...
	if(tMin < tMax && tMin < pTrace->t)
	{
		pTrace->t = tMin;

		pTrace->norm[eX] = pBrush->tPlanes[indMin].a;
		pTrace->norm[eY] = pBrush->tPlanes[indMin].b;
		pTrace->norm[eZ] = pBrush->tPlanes[indMin].c;
		pTrace->d		 = -pBrush->tPlanes[indMin].d;

		pTrace->txtInd   = pBrush->txtID[indMin];
		
		pTrace->bStartSolid = tMin < 0 ? true : false;
		pTrace->bAllSolid = (tMax < 0 && pTrace->bStartSolid) ? true : false;

		pTrace->brushIndex = -1;
	}
	else
	{
		if(tMin < 0)
		{
			pTrace->bStartSolid = true;

			if(tMax < 0)
				pTrace->bAllSolid = true;

			pTrace->brushIndex = -1;
		}
	}
}

/////////////////////////////////////
// Name:	QBSPModelCollision
// Purpose:	find collision with the given
//			model inside the map.
//			NOTE: testT is the starting
//				  't' value (starting minimum)
//				  usu. set as 't = 1'
// Output:	pTrace filled
// Return:	true if collided
/////////////////////////////////////
PUBLIC bool QBSPModelCollision(hQBSP qbsp, int modelInd, 
								  float mins[eMaxPt], float maxs[eMaxPt],
								  float pt1[eMaxPt], float pt2[eMaxPt], 
								  gfxTrace *pTrace, float testT)
{
	memset(pTrace, 0, sizeof(gfxTrace));
	pTrace->t = testT;

	if(TESTFLAGS(qbsp->models[modelInd].status, QBSP_MODEL_FLAG_DISABLE))
		return false;

	float t, vec[eMaxPt] = {pt2[eX]-pt1[eX], pt2[eY]-pt1[eY], pt2[eZ]-pt1[eZ]};

	//determine if we are colliding with bbox or pt.
	bool bIsPt = (mins && maxs) ? false : true;

	//clear face vis (I don't think there will be more brushes than faces)
	QBSPVisBitClearAll(qbsp->faceVis, qbsp->numFaceVis);
		
	// If the current leaf is not hit by ray
	float bbMin[eMaxPt];
	QBSPRayModelGetBBoxMin(qbsp, modelInd, bbMin);

	float bbMax[eMaxPt];
	QBSPRayModelGetBBoxMax(qbsp, modelInd, bbMax);

	if(!bIsPt)
	{
		bbMin[eX] += mins[eX];
		bbMin[eY] += mins[eY];
		bbMin[eZ] += mins[eZ];

		bbMax[eX] += maxs[eX];
		bbMax[eY] += maxs[eY];
		bbMax[eZ] += maxs[eZ];
	}

	if(GFXIntersectBox(pt1, vec, bbMin, bbMax, &t) && t < 1)
	{
		// Loop through and check all of the brushes in this model
		for(int i = 0; i < qbsp->models[modelInd].numOfBrushes; i++)
		{
			_CollBrush(&qbsp->models[modelInd].brushes[i], bIsPt, mins, maxs, 
				pt1, vec, pTrace);
		}
	}

	if(pTrace->t < testT)
		return true;

	return false;
}

PRIVATE void _BrushTransormPlanes(ConvexPolyhedron *pBrush, D3DXMATRIX *pMtx)
{
	//go through the sides
	for(int i = 0; i < pBrush->numPlane; i++)
	{
		D3DXPlaneTransform(&pBrush->tPlanes[i], &pBrush->planes[i], pMtx);
		D3DXPlaneNormalize(&pBrush->tPlanes[i], &pBrush->tPlanes[i]);
	}
}

PRIVATE void _ModelTransformPlanes(hQBSP qbsp, gfxBSPModel *mdl)
{
	D3DXMATRIX  InvTMtx;

	D3DXMatrixInverse(&InvTMtx, NULL, &mdl->wrldMtx);
	D3DXMatrixTranspose(&InvTMtx, &InvTMtx);

	// Loop through and tranform the brush side planes
	for(int i = 0; i < mdl->numOfBrushes; i++)
		_BrushTransormPlanes(&mdl->brushes[i], &InvTMtx);
}

/////////////////////////////////////
// Name:	QBSPModelTranslate
// Purpose:	translate the model
// Output:	model is translated
//			as well as it's planes
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelTranslate(hQBSP qbsp, int modelInd, float trans[eMaxPt], bool bAdd)
{
	gfxBSPModel *mdl = &qbsp->models[modelInd];

	if(bAdd)
	{
		mdl->wrldTrans[eX] += trans[eX];
		mdl->wrldTrans[eY] += trans[eY];
		mdl->wrldTrans[eZ] += trans[eZ];
	}
	else
	{
		mdl->wrldTrans[eX] = trans[eX];
		mdl->wrldTrans[eY] = trans[eY];
		mdl->wrldTrans[eZ] = trans[eZ];
	}

	mdl->wrldMtx._41 = mdl->wrldTrans[eX];
	mdl->wrldMtx._42 = mdl->wrldTrans[eY];
	mdl->wrldMtx._43 = mdl->wrldTrans[eZ];

	//transform the planes
	for(int i = 0; i < mdl->numOfBrushes; i++)
	{
		ConvexPolyhedron *pBrush = &mdl->brushes[i];

		for(int j = 0; j < pBrush->numPlane; j++)
		{
			pBrush->tPlanes[j].d = pBrush->planes[j].d - (pBrush->planes[j].a*mdl->wrldTrans[eX] + pBrush->planes[j].b*mdl->wrldTrans[eY] + pBrush->planes[j].c*mdl->wrldTrans[eZ]);
		}
	}
	
	//_ModelTransformPlanes(qbsp, mdl);
}

/////////////////////////////////////
// Name:	QBSPModelRotate
// Purpose:	rotate the model
// Output:	model is rotated
//			as well as it's planes
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelRotate(hQBSP qbsp, int modelInd, float rot[eMaxPt], bool bAdd)
{
	if(bAdd)
	{
		qbsp->models[modelInd].wrldRot[eX] += rot[eX];
		qbsp->models[modelInd].wrldRot[eY] += rot[eY];
		qbsp->models[modelInd].wrldRot[eZ] += rot[eZ];
	}
	else
	{
		qbsp->models[modelInd].wrldRot[eX] = rot[eX];
		qbsp->models[modelInd].wrldRot[eY] = rot[eY];
		qbsp->models[modelInd].wrldRot[eZ] = rot[eZ];
	}

	D3DXMatrixRotationYawPitchRoll(&qbsp->models[modelInd].wrldMtx,
		qbsp->models[modelInd].wrldRot[eY],
		qbsp->models[modelInd].wrldRot[eX],
		qbsp->models[modelInd].wrldRot[eZ]); 

	qbsp->models[modelInd].wrldMtx._41 = qbsp->models[modelInd].wrldTrans[eX];
	qbsp->models[modelInd].wrldMtx._42 = qbsp->models[modelInd].wrldTrans[eY];
	qbsp->models[modelInd].wrldMtx._43 = qbsp->models[modelInd].wrldTrans[eZ];

	//transform the planes
	_ModelTransformPlanes(qbsp, &qbsp->models[modelInd]);
}

/////////////////////////////////////
// Name:	QBSPModelEnable
// Purpose:	enable/disable model
//			If disabled, model will
//			not perform collision
//			and will not be displayed
// Output:	model status set
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelEnable(hQBSP qbsp, int modelInd, bool bEnable)
{
	if(bEnable)
		CLEARFLAG(qbsp->models[modelInd].status, QBSP_MODEL_FLAG_DISABLE);
	else
		SETFLAG(qbsp->models[modelInd].status, QBSP_MODEL_FLAG_DISABLE);
}

//transform the plane model space
PRIVATE void _BrushRawTransormPlanes(ConvexPolyhedron *pBrush, D3DXMATRIX *pMtx)
{
	//go through the sides
	for(int i = 0; i < pBrush->numPlane; i++)
	{
		pBrush->planes[i].d *= -1;
		D3DXPlaneTransform(&pBrush->planes[i], &pBrush->planes[i], pMtx);
		D3DXPlaneNormalize(&pBrush->planes[i], &pBrush->planes[i]);

		pBrush->tPlanes[i] = pBrush->planes[i];
	}
}

//translate the actual raw vertex data and plane
PROTECTED void _QBSPModelTranslate(hQBSP qbsp, gfxVtxQBSP *pVtx, gfxBSPModel *mdl, float trans[eMaxPt])
{
	if(!pVtx)
		return;

	int i;

	///////////////////////////////////
	// Translate the Vertices
	int maxFace = mdl->faceIndex + mdl->numOfFaces;
	for(i = mdl->faceIndex; i < maxFace; i++)
	{
		int j, maxVert;

		//assume face is a polygon...
		switch(qbsp->faces[i].type)
		{
		case eFacePolygon:
			maxVert = qbsp->faces[i].startVertIndex+qbsp->faces[i].numOfVerts;
			for(j = qbsp->faces[i].startVertIndex; j < maxVert; j++)
			{
				pVtx[j].pt.x += trans[eX];
				pVtx[j].pt.y += trans[eY];
				pVtx[j].pt.z += trans[eZ];
			}
			break;

		case eFacePatch:
			break;

		case eFaceMesh:
			break;

		case eFaceBillboard:
			pVtx[qbsp->faces[i].startVertIndex].pt.x += trans[eX];
			pVtx[qbsp->faces[i].startVertIndex].pt.y += trans[eY];
			pVtx[qbsp->faces[i].startVertIndex].pt.z += trans[eZ];
			break;
		}
	}
	///////////////////////////////////

	///////////////////////////////////
	// Translate the Planes
	D3DXMATRIX  InvTMtx;

	D3DXMatrixScaling(&InvTMtx, 0.995f, 0.995f, 0.995f);//0.99999994f, 0.99999994f, 0.99999994f);
	InvTMtx._14 = -trans[eX];
	InvTMtx._24 = -trans[eY];
	InvTMtx._34 = -trans[eZ];

	//D3DXMatrixTranslation(&InvTMtx, trans[eX], trans[eY], trans[eZ]);
	//D3DXMatrixInverse(&InvTMtx, NULL, &InvTMtx);
	//D3DXMatrixTranspose(&InvTMtx, &InvTMtx);

	// Loop through and tranform the brush side planes
	for(i = 0; i < mdl->numOfBrushes; i++)
		_BrushRawTransormPlanes(&mdl->brushes[i], &InvTMtx);
	///////////////////////////////////
}

/////////////////////////////////////
// Name:	QBSPModelGetTranslate
// Purpose:	get the translate of 
//			the model
// Output:	ptOut
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelGetTranslate(hQBSP qbsp, int modelInd, float ptOut[eMaxPt])
{
	memcpy(ptOut, qbsp->models[modelInd].wrldTrans, sizeof(float)*eMaxPt);
}
