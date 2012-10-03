#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

GFXMATERIAL g_qMdlMat = {
	{0.1f,0.1f,0.1f,1},
	{0.1f,0.1f,0.1f,1},
	{0,0,0,0},
	0};

DWORD g_diffMatState;

PRIVATE void _QBSPRenderPrepare()
{
	g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//set material to be per vertex
	g_p3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &g_diffMatState);
	g_p3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	//g_p3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	//g_p3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
}

PRIVATE void _QBSPRenderUnPrepare()
{
	//enable light again, if flag is set
	if(!TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE))
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	//revert states
	g_p3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, g_diffMatState);
}

PRIVATE void _QBSPRenderFace(hQBSP qbsp, int i, bool bLightMap)
{
	HRESULT hr;

	if(!VIS_IS_ON(qbsp->faceVis, i))
	{
#ifndef SHOW_INVISIBLE
		if(!TESTFLAGS(qbsp->textures[qbsp->faces[i].textureID].flags, QBSP_TXT_INVISIBLE))
		{
#endif
			//set texture
			if(qbsp->faces[i].textureID >= 0)
				TextureSet(qbsp->textures[qbsp->faces[i].textureID].texture, 0);

			if(qbsp->lightmaps 
				&& qbsp->faces[i].lightmapID >= 0 
				&& !TESTFLAGS(qbsp->status, QBSP_FLAG_VTXLIGHTING)
				&& bLightMap)
				TextureSet(qbsp->lightmaps[qbsp->faces[i].lightmapID], 1);

			switch(qbsp->faces[i].type)
			{
			/*case eFacePolygon:
				hr=g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, qbsp->faces[i].startVertIndex, qbsp->faces[i].numOfVerts-2);
				//hr=g_p3DDevice->DrawPrimitive(D3DPT_LINELIST, qbsp->faces[i].startVertIndex, qbsp->faces[i].numOfVerts/2);
				break;*/

			case eFacePatch:
				break;

			case eFacePolygon:
			case eFaceMesh:
				//set indices
				g_p3DDevice->SetIndices(qbsp->meshIndBuff);

				hr=g_p3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, qbsp->faces[i].startVertIndex,
					0, qbsp->faces[i].numMeshVerts, qbsp->faces[i].meshVertIndex, qbsp->faces[i].numMeshVerts/3);

				g_p3DDevice->SetIndices(0);
				break;

			case eFaceBillboard:
				{
					if(qbsp->textures[qbsp->faces[i].textureID].texture)
					{
						float center[eMaxPt]={0};

						gfxVtxQBSP *pVtx;//=qbsp->vtx;

						if(SUCCEEDED(qbsp->vtx->Lock(0,0, (void**)&pVtx, D3DLOCK_DISCARD)))
						{
							center[eX] = pVtx[qbsp->faces[i].startVertIndex].pt.x;
							center[eY] = pVtx[qbsp->faces[i].startVertIndex].pt.y;
							center[eZ] = pVtx[qbsp->faces[i].startVertIndex].pt.z;

							qbsp->vtx->Unlock();

							TextureBltBillboard(qbsp->textures[qbsp->faces[i].textureID].texture, center,
											   1, 0);
						}
					}
				}
				break;
			}

			g_p3DDevice->SetTexture(0,0);
			g_p3DDevice->SetTexture(1,0);

#ifndef SHOW_INVISIBLE
		}
#endif

		VIS_SET(qbsp->faceVis, i);
	}
}

PRIVATE inline void QBSPRenderLeaf(hQBSP qbsp, gfxBSPLeaf *pLeaf, bool bCheckFrustum)
{
	//check if the object is too far away
	float R = kDEPTHFAR*kDEPTHFAR;
	D3DXVECTOR3 d;
	d =  pLeaf->loc - g_eyePt;
	float lSqr = D3DXVec3LengthSq(&d);

	// If the current leaf is not in the camera's frustum, go to the next leaf
	if(bCheckFrustum
		&& (!FrustrumCheckBox(pLeaf->min[eX], pLeaf->min[eY], pLeaf->min[eZ], pLeaf->max[eX], pLeaf->max[eY], pLeaf->max[eZ])
		|| lSqr > R))
		return;

	// If we get here, the leaf we are testing must be visible in our camera's view.
	// Get the number of faces that this leaf is in charge of.
	int faceCount = pLeaf->numOfLeafFaces;

	// Loop through and render all of the faces in this leaf
	while(faceCount--)
	{
		// Grab the current face index from our leaf faces array
		int faceIndex = qbsp->leafFaces[pLeaf->leafface + faceCount];

		_QBSPRenderFace(qbsp, faceIndex, true);
	}
}

PRIVATE void QBSPRecursiveRender(hQBSP qbsp, int node, int cluster, float pos[eMaxPt],
								 bool bCheckFrustum)
{
	int ind=0;
	int side = 1;
	float distance = 0.0f;

	gfxBSPLeaf *pLeaf;

	if(node < 0)
	{
		ind = ~node;
		
		pLeaf = &qbsp->leaves[ind];

		if(!QBSPIsClusterVisible(qbsp, cluster, pLeaf->cluster))
			return;

#ifdef _DEBUG
		g_numFaceProcessed++;
#endif

		//render stuff
		QBSPRenderLeaf(qbsp, pLeaf, bCheckFrustum);

		return;
	}
	else
		ind = node;

	if(bCheckFrustum)
	{
		switch(FrustrumCheckBoxEX(qbsp->nodes[ind].min[eX], qbsp->nodes[ind].min[eY], qbsp->nodes[ind].min[eZ],
			qbsp->nodes[ind].max[eX], qbsp->nodes[ind].max[eY], qbsp->nodes[ind].max[eZ]))
		{
		case BBOX_IN:
			bCheckFrustum = false;
			break;
		case BBOX_OUT:
			return;
		}
	}

	//find where we are, front or back
	int pI = qbsp->nodes[ind].plane;

	distance = qbsp->planes[pI].a*pos[eX] + qbsp->planes[pI].b*pos[eY]
			      +qbsp->planes[pI].c*pos[eZ] - qbsp->planes[pI].d;

	if(distance >= 0)
		side = 0;

	//recurse down from front
	QBSPRecursiveRender(qbsp, qbsp->nodes[ind].child[side], cluster, pos, bCheckFrustum);

	//recurse down from back
	QBSPRecursiveRender(qbsp, qbsp->nodes[ind].child[!side], cluster, pos, bCheckFrustum);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE QBSPRender(hQBSP qbsp, hCAMERA cam)
{
#ifdef _DEBUG
	g_numFaceProcessed = 0;
#endif

	//assume that the frustum is calculated!
	int i;

	float camPos[eMaxPt];

	_QBSPRenderPrepare();

	g_p3DDevice->SetMaterial(&g_qMdlMat);

	g_p3DDevice->SetFVF(GFXVERTEXQBSPFLAG); //nothing cool for now...

	//get camera position
	CameraGetLoc(cam, camPos);

	//clear face vis
	QBSPVisBitClearAll(qbsp->faceVis, qbsp->numFaceVis);

	//set stream source
	g_p3DDevice->SetStreamSource(0, qbsp->vtx, 0, sizeof(gfxVtxQBSP));

	////////////////////////////////////////////////////////////////////
	//render models
	float bbMin[eMaxPt], bbMax[eMaxPt];
	for(i = 1; i < qbsp->numModels; i++)
	{
		int f;

		QBSPRayModelGetBBoxMin(qbsp, i, bbMin);
		QBSPRayModelGetBBoxMax(qbsp, i, bbMax);

		//check if the object is too far away
		float R = kDEPTHFAR*kDEPTHFAR;
		D3DXVECTOR3 d;
		QBSPModelGetTranslate(qbsp, i, (float*)d);
		d -=  g_eyePt;
		float lSqr = D3DXVec3LengthSq(&d);

		// model has to be enabled
		// and within furstrum
		if(TESTFLAGS(qbsp->models[i].status, QBSP_MODEL_FLAG_DISABLE)
			|| !FrustrumCheckBox(bbMin[eX], bbMin[eY], bbMin[eZ],
		  	 				       bbMax[eX], bbMax[eY], bbMax[eZ])
			|| lSqr > R)
		{
			for(f = 0; f < qbsp->models[i].numOfFaces; f++)
				VIS_SET(qbsp->faceVis, f+qbsp->models[i].faceIndex);

#ifdef _DEBUG
			g_numCulled++;
#endif

			continue;
		}

		//set the lights up.
		LightEnableAllNearest(qbsp->models[i].wrldTrans);

		g_p3DDevice->SetTransform(D3DTS_WORLD, &qbsp->models[i].wrldMtx);

		for(f = 0; f < qbsp->models[i].numOfFaces; f++)
		{
			_QBSPRenderFace(qbsp, f+qbsp->models[i].faceIndex, false);
		}
	}
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// render the static polygons

	//disable light
	if(!TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE))
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	g_p3DDevice->SetTransform(D3DTS_WORLD, &qbsp->wrldMtx);

#if 1
	// Grab the leaf index that our camera is in
	int leafIndex = QBSPFindLeaf(qbsp, camPos);

	assert(leafIndex >= 0);

	// Grab the cluster that is assigned to the leaf
	int cluster = qbsp->leaves[leafIndex].cluster;

	QBSPRecursiveRender(qbsp, 0, cluster, camPos, true);
#else
	int leafIndex,cluster;

	gfxBSPLeaf *pLeaf;

	// Grab the leaf index that our camera is in
	leafIndex = QBSPFindLeaf(qbsp, camPos);

	assert(leafIndex >= 0);

	// Grab the cluster that is assigned to the leaf
	cluster = qbsp->leaves[leafIndex].cluster;

	i = qbsp->numLeaves;
	while(i--)
	{
		pLeaf = &qbsp->leaves[i];

		// If the current leaf can't be seen from our cluster, go to the next leaf
		if(!QBSPIsClusterVisible(qbsp, cluster, pLeaf->cluster))
		{
#ifdef _DEBUG
			g_numCulled++;
#endif
			continue;
		}

		g_numFaceProcessed++;
		
		// If the current leaf is not in the camera's frustum, go to the next leaf
		if(!FrustrumCheckBox(pLeaf->min[eX], pLeaf->min[eY], pLeaf->min[eZ],
		  	 					   pLeaf->max[eX], pLeaf->max[eY], pLeaf->max[eZ]))
		{
#ifdef _DEBUG
			g_numCulled++;
#endif
			continue;
		}

		// If we get here, the leaf we are testing must be visible in our camera's view.
		// Get the number of faces that this leaf is in charge of.
		int faceCount = pLeaf->numOfLeafFaces;

		// Loop through and render all of the faces in this leaf
		while(faceCount--)
		{
			// Grab the current face index from our leaf faces array
			int faceIndex = qbsp->leafFaces[pLeaf->leafface + faceCount];

			_QBSPRenderFace(qbsp, faceIndex, true);
		}
	}
#endif
	////////////////////////////////////////////////////////////////////

	g_p3DDevice->SetStreamSource(0, 0, 0, 0);

	_QBSPRenderUnPrepare();

	//GFXBltModeEnable(0);
	//TextureSet(qbsp->lightmaps[qbsp->faces[i].lightmapID], 1);
	//TextureBlt(qbsp->lightmaps[0], 0,0, 0,0,0);
	//GFXBltModeDisable();

	return RETCODE_SUCCESS;
}