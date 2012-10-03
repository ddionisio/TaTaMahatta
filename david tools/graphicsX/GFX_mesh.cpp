#include "gdix.h"
#include "gdix_i.h"
#include "gdiX_mesh_i.h"

#include "gdiX_modelFX_i.h"

#define VTXBUFF_USAGE_FLAG		D3DUSAGE_WRITEONLY//D3DUSAGE_WRITEONLY//D3DUSAGE_SOFTWAREPROCESSING//D3DUSAGE_DYNAMIC//
#define VTXBUFF_POOL_FLAG		D3DPOOL_MANAGED//D3DPOOL_DEFAULT//

#define INDBUFF_USAGE_FLAG		D3DUSAGE_WRITEONLY//D3DUSAGE_DYNAMIC
#define INDBUFF_POOL_FLAG		D3DPOOL_MANAGED//D3DPOOL_DEFAULT

/////////////////////////////////////
// Name:	MeshDestroyGroup
// Purpose:	destroy group data
// Output:	group deinitialized
// Return:	none
/////////////////////////////////////
PROTECTED void MeshDestroyGroup(gfxMesh *pMesh, DWORD grpInd)
{
	if(grpInd < pMesh->numGrp)
	{
		//destroy index buffer
		if(pMesh->grp[grpInd].indBuff)
			pMesh->grp[grpInd].indBuff->Release();

		pMesh->maxFaceCount -= pMesh->grp[grpInd].faceCount;
		pMesh->maxIndCount -= pMesh->grp[grpInd].numInd;

		//destroy FX if it exists
		MeshFXDestroy(&pMesh->grp[grpInd].mdlFX);

		memset(&pMesh->grp[grpInd], 0, sizeof(gfxMeshGrp));
	}
}

/////////////////////////////////////
// Name:	MeshInitGroup
// Purpose:	initialize a group
// Output:	group intialized
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE MeshInitGroup(gfxMesh *pMesh, DWORD grpInd, const char name[NAMEMAX2],
								DWORD matInd, DWORD numFace)
{
	if(grpInd >= pMesh->numGrp) return RETCODE_FAILURE;

	//deinitialize the group
	MeshDestroyGroup(pMesh, grpInd);

	//set stuff up
	strcpy(pMesh->grp[grpInd].name, name);
	
	pMesh->grp[grpInd].faceCount = numFace;
	pMesh->maxFaceCount += numFace;
	
	pMesh->grp[grpInd].numInd = numFace*NUMPTFACE;
	pMesh->maxIndCount += pMesh->grp[grpInd].numInd;

	pMesh->grp[grpInd].materialInd = matInd;

	pMesh->grp[grpInd].flag = 0;

	//create index buffer
	if(_GFXCheckError(g_p3DDevice->CreateIndexBuffer(sizeof(unsigned short)*pMesh->grp[grpInd].numInd,
		INDBUFF_USAGE_FLAG, D3DFMT_INDEX16, INDBUFF_POOL_FLAG, &pMesh->grp[grpInd].indBuff, 0),
		true, "Error in MeshInitGroup"))
	{ MeshDestroyGroup(pMesh, grpInd); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MeshCreate
// Purpose:	create mesh
// Output:	pMesh is set
// Return:	success
/////////////////////////////////////
PROTECTED RETCODE MeshCreate(gfxMesh *pMesh, DWORD numGrp, DWORD numVtx)
{
	assert(numGrp > 0);	//there should at least be one group
	assert(numVtx > 0);	//there should at least be one vtx

	MeshDestroy(pMesh);
	
	pMesh->numGrp = numGrp;
	pMesh->numVtx = numVtx;
	pMesh->vtxBuffFlag = VTXBUFF_USAGE_FLAG;

	//create vertices
	if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(gfxVtx)*pMesh->numVtx,
		VTXBUFF_USAGE_FLAG, GFXVERTEXFLAG, VTXBUFF_POOL_FLAG, &pMesh->vtxBuff, 0),
		true, "Error in MeshCreate"))
	{ MeshDestroy(pMesh); return RETCODE_FAILURE; }

	//create the system memory vertex buffer
	pMesh->vtxBuffSys = (gfxVtx*)GFX_MALLOC(sizeof(gfxVtx)*pMesh->numVtx);

	if(!pMesh->vtxBuffSys)
	{ MeshDestroy(pMesh); return RETCODE_FAILURE; }

	//create bone IDs
	pMesh->boneVtxIDs = (char*)GFX_MALLOC(sizeof(char)*pMesh->numVtx);
	
	if(!pMesh->boneVtxIDs)
	{ MeshDestroy(pMesh); return RETCODE_FAILURE; }

	for(int i = 0; i < pMesh->numVtx; i++)
		pMesh->boneVtxIDs[i] = -1;

	//create groups (just create them...)
	pMesh->grp = (gfxMeshGrp*)GFX_MALLOC(sizeof(gfxMeshGrp)*pMesh->numGrp);

	if(!pMesh->grp)
	{ MeshDestroy(pMesh); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MeshDestroy
// Purpose:	destroy mesh
// Output:	mesh cleaned
// Return:	none
/////////////////////////////////////
PROTECTED void MeshDestroy(gfxMesh *mesh)
{
	if(mesh)
	{
		if(mesh->vtxBuff)
		{
			mesh->vtxBuff->Release();
		}

		if(mesh->vtxBuffSys)
			GFX_FREE(mesh->vtxBuffSys);

		if(mesh->boneVtxIDs)
			GFX_FREE(mesh->boneVtxIDs);

		if(mesh->grp)
		{
			for(int i = 0; i < mesh->numGrp; i++)
				MeshDestroyGroup(mesh, i);

			GFX_FREE(mesh->grp);
		}

		memset(mesh, 0, sizeof(gfxMesh));
	}
}

/////////////////////////////////////
// Name:	MeshGetNumVtx
// Purpose:	get the number of vtx.
// Output:	none
// Return:	num vtx.
/////////////////////////////////////
PROTECTED DWORD MeshGetNumVtx(gfxMesh *mesh)
{
	return mesh->numVtx;
}

/////////////////////////////////////
// Name:	MeshGetNumFace
// Purpose:	get the number of faces
//          (total faces)
// Output:	none
// Return:	num faces
/////////////////////////////////////
PROTECTED DWORD MeshGetNumFace(gfxMesh *mesh)
{
	return mesh->maxFaceCount;
}

/////////////////////////////////////
// Name:	MeshGetNumInd
// Purpose:	get the number of indices
//			(total indices)
// Output:	none
// Return:	num indices
/////////////////////////////////////
PROTECTED DWORD MeshGetNumInd(gfxMesh *mesh)
{
	return mesh->maxIndCount;
}

/////////////////////////////////////
// Name:	MeshLockVtx
// Purpose:	lock mesh vtx to get access
//			to vertices
// Output:	lock vtx, set pVtx
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE MeshLockVtx(gfxMesh *mesh, DWORD flag, gfxVtx **pVtx)
{
	if(_GFXCheckError(
		mesh->vtxBuff->Lock(0,0,(void**)pVtx, flag),
					   true,
			"Error in MeshLockVtx"))
	{ return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MeshUnlockVtx
// Purpose:	unlock vtx, use after
//			MeshLockVtx
// Output:	unlock vtx
// Return:	none
/////////////////////////////////////
PROTECTED void MeshUnlockVtx(gfxMesh *mesh)
{
	mesh->vtxBuff->Unlock();
}

/////////////////////////////////////
// Name:	MeshGetVtxSys
// Purpose:	get the system vtx.
//			buffer
// Output:	vtx sys.
// Return:	none
/////////////////////////////////////
PROTECTED void MeshGetVtxSys(gfxMesh *mesh, gfxVtx **pVtx)
{
	*pVtx = mesh->vtxBuffSys;
}

/////////////////////////////////////
// Name:	MeshLockInd
// Purpose:	lock mesh indices to get access
//			to index stuff
// Output:	lock indices, set pInd
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE MeshLockInd(gfxMesh *mesh, DWORD grpInd, DWORD flag, unsigned short **pInd)
{
	if(grpInd >= mesh->numGrp) return RETCODE_FAILURE;

	if(_GFXCheckError(
			mesh->grp[grpInd].indBuff->Lock(0, sizeof(unsigned short)*mesh->grp[grpInd].numInd,
			(void**)pInd, flag),
			true,
			"Error in MeshLockInd"))
	{ return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MeshUnlockInd
// Purpose:	unlock ind, use after
//			MeshLockInd
// Output:	unlock ind
// Return:	none
/////////////////////////////////////
PROTECTED void MeshUnlockInd(gfxMesh *mesh, DWORD grpInd)
{
	if(grpInd >= mesh->numGrp) return;

	mesh->grp[grpInd].indBuff->Unlock();
}

/////////////////////////////////////
// Name:	MeshGetBoneIDs
// Purpose:	get mesh bone IDs corresponding
//			to the vertices
// Output:	none
// Return:	the bone IDs
/////////////////////////////////////
PROTECTED char * MeshGetBoneIDs(gfxMesh *mesh)
{
	return mesh->boneVtxIDs;
}

/////////////////////////////////////
// Name:	MeshComputeNormals
// Purpose:	compute vtx normals
// Output:	normals are calculated
// Return:	none
/////////////////////////////////////
PROTECTED void MeshComputeNormals(gfxMesh *mesh)
{
	//This outta make lighting easy
	//D3DXComputeNormals(mesh->theMesh);
}

/////////////////////////////////////
// Name:	MeshComputeBound
// Purpose:	compute the bounds
// Output:	pBoundOut is computed
// Return:	none
/////////////////////////////////////
PROTECTED void MeshComputeBound(gfxMesh *mesh, gfxBound *pBoundOut)
{
	gfxVtx *pVtx;

	if(MeshLockVtx(mesh, 0, &pVtx) != RETCODE_SUCCESS)
		return;

	//compute bound

	//initiate bound
	pBoundOut->max[eX] = pBoundOut->min[eX] = pVtx[0].x;
	pBoundOut->max[eY] = pBoundOut->min[eY] = pVtx[0].y;
	pBoundOut->max[eZ] = pBoundOut->min[eZ] = pVtx[0].z;
	memset(pBoundOut->center, 0, sizeof(float)*eMaxPt);

	for(int i = 0; i < mesh->numVtx; i++)
	{
		pBoundOut->min[eX] = MIN(pBoundOut->min[eX], pVtx[i].x);
		pBoundOut->min[eY] = MIN(pBoundOut->min[eY], pVtx[i].y);
		pBoundOut->min[eZ] = MIN(pBoundOut->min[eZ], pVtx[i].z);
		
		pBoundOut->max[eX] = MAX(pBoundOut->max[eX], pVtx[i].x);
		pBoundOut->max[eY] = MAX(pBoundOut->max[eY], pVtx[i].y);
		pBoundOut->max[eZ] = MAX(pBoundOut->max[eZ], pVtx[i].z);

		pBoundOut->center[eX] += pVtx[i].x;
		pBoundOut->center[eY] += pVtx[i].y;
		pBoundOut->center[eZ] += pVtx[i].z;
	}

	pBoundOut->center[eX] /= mesh->numVtx;
	pBoundOut->center[eY] /= mesh->numVtx;
	pBoundOut->center[eZ] /= mesh->numVtx;

	//calculate radius
	float xL=abs(pBoundOut->center[eX]-pBoundOut->max[eX]), 
		yL=abs(pBoundOut->center[eY]-pBoundOut->max[eY]), zL=abs(pBoundOut->center[eZ]-pBoundOut->max[eZ]);

	pBoundOut->radius = MAX(xL, yL);
	pBoundOut->radius = MAX(pBoundOut->radius, zL);
	
	MeshUnlockVtx(mesh);
}

/////////////////////////////////////
// Name:	MeshNegativeTranslate
// Purpose:	subtract with given trans vector
// Output:	mesh translated
// Return:	none
/////////////////////////////////////
PROTECTED void MeshNegativeTranslate(gfxMesh *mesh, float trans[eMaxPt])
{
	gfxVtx *pVtx;

	if(MeshLockVtx(mesh, 0, &pVtx) == RETCODE_SUCCESS)
	{
		int max = MeshGetNumVtx(mesh);
		for(int i = 0; i < max; i++)
		{
			pVtx[i].x -= trans[eX];
			pVtx[i].y -= trans[eY];
			pVtx[i].z -= trans[eZ];
		}

		MeshUnlockVtx(mesh);
	}
}

/////////////////////////////////////
// Name:	MeshDisplay
// Purpose:	display mesh
// Output:	mesh displayed
// Return:	SUCCESS if success
/////////////////////////////////////
PROTECTED RETCODE MeshDisplay(gfxMesh *mesh, GFXMATERIAL *materials,
							  hTXT *textures)
{
	g_p3DDevice->SetStreamSource(0, mesh->vtxBuff, 0, sizeof(gfxVtx));

	for(int i = 0; i < mesh->numGrp; i++)
	{
		if(mesh->grp[i].mdlFX)
			MeshFXPrepareStates(mesh->grp[i].mdlFX);

		if(_GFXCheckError(
		g_p3DDevice->SetIndices(mesh->grp[i].indBuff),
		true,
			"Error in MeshDisplay"))
		{ return RETCODE_FAILURE; }

		if(mesh->grp[i].materialInd >= 0)
		{
			if(materials)
				g_p3DDevice->SetMaterial(&materials[mesh->grp[i].materialInd]);

			if(textures)
				if(textures[mesh->grp[i].materialInd])
					TextureSet(textures[mesh->grp[i].materialInd], 0);
		}

		if(_GFXCheckError(
		g_p3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh->numVtx,
			0, mesh->grp[i].faceCount),
			true,
			"Error in MeshDisplay"))
		{ return RETCODE_FAILURE; }

		if(mesh->grp[i].mdlFX)
			MeshFXUnPrepareStates(mesh->grp[i].mdlFX);

		g_p3DDevice->SetIndices(0);
		TextureSet(0,0);
	}

	g_p3DDevice->SetStreamSource(0, 0, 0, 0);
	

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MeshInterpolate
// Purpose:	interpolate vertices between
//			mesh1 and mesh2 and put in
//			meshOut
// Output:	meshOut set
// Return:	success
/////////////////////////////////////
PROTECTED RETCODE MeshInterpolate(gfxMesh *mesh1, gfxMesh *mesh2, float t,
								  gfxMesh *meshOut)
{
	gfxVtx *ptrVtxOut, *ptrVtx1, *ptrVtx2;

	if(MeshLockVtx(meshOut, 0, &ptrVtxOut) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	MeshGetVtxSys(mesh1, &ptrVtx1);

	MeshGetVtxSys(mesh2, &ptrVtx2);

	int max = MeshGetNumVtx(meshOut);
	for(int i = 0; i < max; i++)
	{
		ptrVtxOut[i].x = ptrVtx1[i].x + ((ptrVtx2[i].x-ptrVtx1[i].x)*t);
		ptrVtxOut[i].y = ptrVtx1[i].y + ((ptrVtx2[i].y-ptrVtx1[i].y)*t);
		ptrVtxOut[i].z = ptrVtx1[i].z + ((ptrVtx2[i].z-ptrVtx1[i].z)*t);
	}

	MeshUnlockVtx(meshOut);
	
	return RETCODE_SUCCESS;
}

//here for now...
PRIVATE void __TranslateGfxVtx(const gfxVtx *pVtx, const D3DXMATRIX *mtx,
							   gfxVtx *pOut)
{
	//do point
	pOut->x = pVtx->x*mtx->_11+pVtx->y*mtx->_21+pVtx->z*mtx->_31+mtx->_41;
	pOut->y = pVtx->x*mtx->_12+pVtx->y*mtx->_22+pVtx->z*mtx->_32+mtx->_42;
	pOut->z = pVtx->x*mtx->_13+pVtx->y*mtx->_23+pVtx->z*mtx->_33+mtx->_43;

	//do normal
	pOut->nX = pVtx->nX*mtx->_11+pVtx->nY*mtx->_21+pVtx->nZ*mtx->_31;
	pOut->nY = pVtx->nX*mtx->_12+pVtx->nY*mtx->_22+pVtx->nZ*mtx->_32;
	pOut->nZ = pVtx->nX*mtx->_13+pVtx->nY*mtx->_23+pVtx->nZ*mtx->_33;

	//normalize the normals...sigh...
	float l = sqrtf(pOut->nX*pOut->nX+pOut->nY*pOut->nY+pOut->nZ*pOut->nZ);

	pOut->nX /= l;
	pOut->nY /= l;
	pOut->nZ /= l;
}

/////////////////////////////////////
// Name:	MeshCopyVtxToVtxSys
// Purpose:	copy mesh vtx to vtxjoint
// Output:	vtx buffer joint is filled
// Return:	success if success
/////////////////////////////////////
PROTECTED RETCODE MeshCopyVtxToVtxSys(gfxMesh *mesh)
{
	gfxVtx *pVtx, *pVtxSys;

	if(MeshLockVtx(mesh, 0, &pVtx) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	MeshGetVtxSys(mesh, &pVtxSys);

	int max = MeshGetNumVtx(mesh);
	for(int i = 0; i < max; i++)
		memcpy(&pVtxSys[i], &pVtx[i], sizeof(gfxVtx));

	MeshUnlockVtx(mesh);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MeshTranslateVtxJoint
// Purpose:	do the final translation
//			if joint to vertices
// Output:	vtx buffer joint is filled
// Return:	success if success
/////////////////////////////////////
PROTECTED RETCODE MeshTranslateVtxJoint(gfxMesh *mesh, gfxObjJoint *joints)
{
	gfxVtx *pVtx, *pVtxSys;

	if(MeshLockVtx(mesh, 0, &pVtx) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	MeshGetVtxSys(mesh, &pVtxSys);

	int max = MeshGetNumVtx(mesh);
	for(int i = 0; i < max; i++)
	{
		memcpy(&pVtx[i], &pVtxSys[i], sizeof(gfxVtx));

		if(mesh->boneVtxIDs[i] != -1)
			__TranslateGfxVtx(&pVtxSys[i], &joints[mesh->boneVtxIDs[i]].final,
							   &pVtx[i]);
	}

	MeshUnlockVtx(mesh);

	return RETCODE_SUCCESS;
}

/*************************************************************************
 *************************************************************************
	Model FX stuff
 *************************************************************************
 ************************************************************************/

/////////////////////////////////////
// Name:	MeshUpdate
// Purpose:	update necessary stuff in
//			mesh. Stuff like FX
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshUpdate(gfxMesh *mesh)
{
	/*for(int i = 0; i < mesh->numGrp; i++)
	{
		if(mesh->grp[i].mdlFX)
			MeshFXUpdateTextureAnim(mesh->grp[i].mdlFX);
	}*/
}

//Editor Purpose

/////////////////////////////////////
// Name:	MDLGetNumGroup
// Purpose:	get number of group
// Output:	none
// Return:	number of group
/////////////////////////////////////
PUBLIC int MDLGetNumGroup(hMDL mdl)
{
	return mdl->mesh.numGrp;
}

/////////////////////////////////////
// Name:	MDLGetGroupName
// Purpose:	get name of group
// Output:	sOut filled, NULL if 
//			non-existence
// Return:	none
/////////////////////////////////////
PUBLIC void MDLGetGroupName(hMDL mdl, int index, char *sOut)
{
	if(index < mdl->mesh.numGrp)
		strcpy(sOut, mdl->mesh.grp[index].name);
	else
		*sOut = 0;
}

/////////////////////////////////////
// Name:	MDLGetGroupInd
// Purpose:	get group index
// Output:	none
// Return:	group index (-1 not found)
/////////////////////////////////////
PUBLIC int MDLGetGroupInd(hMDL mdl, const char *groupName)
{
	for(int i = 0; i < mdl->mesh.numGrp; i++)
	{
		if(strcmp(mdl->mesh.grp[i].name, groupName) == 0)
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	MDLFXDestroy
// Purpose:	destroys FX from a group
// Output:	FX destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXDestroy(hMDL mdl, int groupInd)
{
	assert(groupInd < mdl->mesh.numGrp);

	MeshFXDestroy(&mdl->mesh.grp[groupInd].mdlFX);
}

/////////////////////////////////////
// Name:	MDLFXInit
// Purpose:	initializes FX for group 
// Output:	FX initialize
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE MDLFXInit(hMDL mdl, int groupInd)
{
	assert(groupInd < mdl->mesh.numGrp);

	MDLFXDestroy(mdl, groupInd);

	mdl->mesh.grp[groupInd].mdlFX = MeshFXCreate();

	if(!mdl->mesh.grp[groupInd].mdlFX) return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MDLFXAddRenderState
// Purpose:	add a render state to
//			FX
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXAddRenderState(hMDL mdl, int groupInd, DWORD state, DWORD val)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXAddRenderState(mdl->mesh.grp[groupInd].mdlFX, state, val);
}

/////////////////////////////////////
// Name:	MDLFXAddTextureState
// Purpose:	add a texture state to FX
// Output:	texture state added
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXAddTextureState(hMDL mdl, int groupInd, 
									 DWORD txtStage, DWORD state, DWORD val)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXAddTextureState(mdl->mesh.grp[groupInd].mdlFX, txtStage, state, val);
}

/////////////////////////////////////
// Name:	MDLFXSetTextureAnimTrans
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTextureAnimTrans(hMDL mdl, int groupInd, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXSetTextureAnimTrans(mdl->mesh.grp[groupInd].mdlFX, txtStage,
									spd_s, spd_t,
									acc_s, acc_t,
									min_s, min_t,
									max_s, max_t);
}

/////////////////////////////////////
// Name:	MDLFXSetTextureAnimRot
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTextureAnimRot(hMDL mdl, int groupInd, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXSetTextureAnimRot(mdl->mesh.grp[groupInd].mdlFX, txtStage,
									spd_s, spd_t,
									acc_s, acc_t,
									min_s, min_t,
									max_s, max_t);
}

/////////////////////////////////////
// Name:	MDLFXSetTextureAnimScale
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTextureAnimScale(hMDL mdl, int groupInd, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXSetTextureAnimScale(mdl->mesh.grp[groupInd].mdlFX, txtStage,
									spd_s, spd_t,
									acc_s, acc_t,
									min_s, min_t,
									max_s, max_t);
}

/////////////////////////////////////
// Name:	MDLFXGetTextureAnimTrans
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MDLFXGetTextureAnimTrans(hMDL mdl, int groupInd, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXGetTextureAnimTrans(mdl->mesh.grp[groupInd].mdlFX, txtStage,
									spd_s, spd_t,
									acc_s, acc_t,
									min_s, min_t,
									max_s, max_t);
}

/////////////////////////////////////
// Name:	MDLFXGetTextureAnimRot
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MDLFXGetTextureAnimRot(hMDL mdl, int groupInd, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXGetTextureAnimRot(mdl->mesh.grp[groupInd].mdlFX, txtStage,
									spd_s, spd_t,
									acc_s, acc_t,
									min_s, min_t,
									max_s, max_t);
}

/////////////////////////////////////
// Name:	MDLFXGetTextureAnimScale
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MDLFXGetTextureAnimScale(hMDL mdl, int groupInd, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXGetTextureAnimScale(mdl->mesh.grp[groupInd].mdlFX, txtStage,
									spd_s, spd_t,
									acc_s, acc_t,
									min_s, min_t,
									max_s, max_t);
}

/////////////////////////////////////
// Name:	MDLFXEnableTextureAnim
// Purpose:	enable/disable texture anim
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXEnableTextureAnim(hMDL mdl, int groupInd, DWORD txtStage, bool bEnable)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXEnableTextureAnim(mdl->mesh.grp[groupInd].mdlFX, txtStage, bEnable);
}

/////////////////////////////////////
// Name:	MDLFXIsTextureAnim
// Purpose:	is texture anim enable?
// Output:	none
// Return:	true if so
/////////////////////////////////////
PROTECTED bool MDLFXIsTextureAnim(hMDL mdl, int groupInd, DWORD txtStage)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(!mdl->mesh.grp[groupInd].mdlFX)
		return false;

	return MeshFXIsTextureAnim(mdl->mesh.grp[groupInd].mdlFX, txtStage);
}

/////////////////////////////////////
// Name:	MDLFXSetTexture
// Purpose:	set the texture to apply
//			give txt = 0 to unset the
//			texture
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTexture(hMDL mdl, int groupInd, DWORD txtStage, hTXT txt)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(mdl->mesh.grp[groupInd].mdlFX)
		MeshFXSetTexture(mdl->mesh.grp[groupInd].mdlFX, txtStage, txt);
}

/////////////////////////////////////
// Name:	MDLFXGetRenderState
// Purpose:	get the render state value
//			if it exists
// Output:	valOut is filled
// Return:	true if state exists
/////////////////////////////////////
PUBLIC bool MDLFXGetRenderState(hMDL mdl, int groupInd, DWORD state, DWORD *valOut)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(!mdl->mesh.grp[groupInd].mdlFX)
		return false;

	return MeshFXGetRenderState(mdl->mesh.grp[groupInd].mdlFX, state, valOut);
}

/////////////////////////////////////
// Name:	MDLFXGetTextureState
// Purpose:	get the texture state value
//			if it exists
// Output:	valOut is filled
// Return:	true if state exists
/////////////////////////////////////
PUBLIC bool MDLFXGetTextureState(hMDL mdl, int groupInd, DWORD txtStage, 
									DWORD state, DWORD *valOut)
{
	assert(groupInd < mdl->mesh.numGrp);

	if(!mdl->mesh.grp[groupInd].mdlFX)
		return false;

	return MeshFXGetTextureState(mdl->mesh.grp[groupInd].mdlFX, txtStage, state, valOut);
}

PRIVATE void _MDLFXGetItem(hMDL mdl, const char *filename,
							 int grpInd, char *itmBuff,
							 vector< vector<string> > *txtPaths)
{
	char buff[MAXCHARBUFF];

	char *oneItemWalker = itmBuff;

	char retChar = ParserReadWordBuff(&oneItemWalker, buff, MAXCHARBUFF, '_');

	DWORD stage;
	DWORD state;
	DWORD val;

	if(retChar == '_')
	{
		//determine what the item is
		if(stricmp(buff, "render") == 0)
		{
			sscanf(oneItemWalker, "%d=%d", &state, &val);

			MDLFXAddRenderState(mdl, grpInd, state, val);
		}
		else if(stricmp(buff, "texture") == 0)
		{
			sscanf(oneItemWalker, "%d_%d=%d", &stage, &state, &val);

			MDLFXAddTextureState(mdl, grpInd, stage, state, val);
		}
		else if(stricmp(buff, "textureimg") == 0)
		{
			char *buffWalker = oneItemWalker;

			sscanf(oneItemWalker, "%d=", &stage);

			char imgPath[MAXCHARBUFF];

			if(ParserReadStringBuff(&buffWalker, imgPath, MAXCHARBUFF, '"', '"') == RETCODE_ENDREACHED)
			{
				char fullPath[MAXCHARBUFF];

				ConcatinateFilePath(filename, imgPath, fullPath);

				hTXT texture = TextureCreate(0, fullPath, false, 0);

				if(texture)
				{
					if(txtPaths)
						(*txtPaths)[grpInd].push_back(string(fullPath));

					MDLFXSetTexture(mdl, grpInd, stage, texture);

					TextureDestroy(&texture); //this will only remove one reference
				}
			}
		}
		else if(stricmp(buff, "textureanim") == 0)
		{
			char animType;

			float spdS,spdT,accS,accT,minS,minT,maxS,maxT;

			sscanf(oneItemWalker, "%d_%c=(%f,%f,%f,%f,%f,%f,%f,%f)",
				&stage, &animType, &spdS,&spdT,&accS,&accT,&minS,&minT,&maxS,&maxT);

			switch(animType)
			{
			case 's': //scale
				MDLFXSetTextureAnimScale(mdl, grpInd, stage, 
					spdS,spdT,accS,accT,minS,minT,maxS,maxT);
				break;
			case 'r': //rotate
				MDLFXSetTextureAnimRot(mdl, grpInd, stage, 
					spdS,spdT,accS,accT,minS,minT,maxS,maxT);
				break;
			case 't': //translate
				MDLFXSetTextureAnimTrans(mdl, grpInd, stage, 
					spdS,spdT,accS,accT,minS,minT,maxS,maxT);
				break;
			}

			MDLFXEnableTextureAnim(mdl, grpInd, stage, true);
		}
	}
}

PRIVATE void _MDLFXLoadItems(hMDL mdl, const char *filename,
							 int grpInd, char *buff,
							 vector< vector<string> > *txtPaths)
{
	char *itemsWalker = buff;

	char oneItemBuff[MAXCHARBUFF];

	RETCODE ret;

	while(1)
	{
		ret = ParserReadStringBuff(&itemsWalker, oneItemBuff, MAXCHARBUFF, 0, ';');

		_MDLFXGetItem(mdl, filename, grpInd, oneItemBuff, txtPaths);

		if(ret == RETCODE_FAILURE || *itemsWalker == 0) //finished getting all items
			break;
	}
}

/////////////////////////////////////
// Name:	MDLFXLoad
// Purpose:	load FX for the model
//			NOTE: texture paths use
//			relative path to filename
//			NOTE: all groups inside
//			the FX must correspond to
//			those found inside the model.
//			NOTE: this function is automatically
//			called when creating the model,
//			assuming that the path and name of
//			the FX file is the same, /w ext: .mfx
//			NOTE: the vector passed in is only used
//			by Model FX Editor.
// Output:	FX intialized
//			txtPaths is filled with the FULL PATH of
//			textures stage per group (it's a little crazy...)
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE MDLFXLoad(hMDL mdl, const char *filename, 
						 vector< vector<string> > *txtPaths)
{
	FILE *fp = fopen(filename, "rt");

	if(!fp) { return RETCODE_FAILURE; }

	//resize the vector with number of groups
	if(txtPaths)
	{
		txtPaths->clear();
		txtPaths->resize(MDLGetNumGroup(mdl));
	}

	RETCODE ret;
	char buff[MAXCHARBUFF];

	char *itemsBuff;
	int itemsBuffSize;

	int  grpInd;

	while(1)
	{
		//look for a word
		ret = ParserReadStringFile(fp, buff, MAXCHARBUFF, '[', ']');

		//that means we have reached the end of file
		//or that we reach the end char
		if(ret == RETCODE_FAILURE || buff[0] == 0)
			break;		 //we are done parsing

		//get the group index
		grpInd = MDLGetGroupInd(mdl, buff);

		if(grpInd != -1)
		{
			//re-initialize the group
			MDLFXInit(mdl, grpInd);

			//get the items for the group
			ParserCountStringFile(fp, &itemsBuffSize, '{', '}');

			if(itemsBuffSize > 0)
			{
				itemsBuffSize++;

				itemsBuff = (char *)GFX_MALLOC(sizeof(char)*itemsBuffSize);

				ParserReadStringFile(fp, itemsBuff, itemsBuffSize, '{', '}');

				_MDLFXLoadItems(mdl, filename, grpInd, itemsBuff, txtPaths);

				GFX_FREE(itemsBuff);
			}
		}
	}

	fclose(fp);

	return RETCODE_SUCCESS;
}
