#ifndef _gdix_mesh_i_h
#define _gdix_mesh_i_h

//THIS IS FOR INTERNAL GFX USE ONLY!

/////////////////////////////////////
// Name:	MeshCreate
// Purpose:	create mesh
// Output:	pMesh is set
// Return:	success
/////////////////////////////////////
PROTECTED RETCODE MeshCreate(gfxMesh *pMesh, DWORD numGrp, DWORD numVtx);

/////////////////////////////////////
// Name:	MeshDestroy
// Purpose:	destroy mesh
// Output:	mesh cleaned
// Return:	none
/////////////////////////////////////
PROTECTED void MeshDestroy(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshInitGroup
// Purpose:	initialize a group
// Output:	group intialized
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE MeshInitGroup(gfxMesh *pMesh, DWORD grpInd, const char name[NAMEMAX2],
								DWORD matInd, DWORD numFace);

/////////////////////////////////////
// Name:	MeshDestroyGroup
// Purpose:	destroy group data
// Output:	group deinitialized
// Return:	none
/////////////////////////////////////
PROTECTED void MeshDestroyGroup(gfxMesh *pMesh, DWORD grpInd);

/////////////////////////////////////
// Name:	MeshGetNumVtx
// Purpose:	get the number of vtx.
// Output:	none
// Return:	num vtx.
/////////////////////////////////////
PROTECTED DWORD MeshGetNumVtx(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshGetNumFace
// Purpose:	get the number of faces
//          (total faces)
// Output:	none
// Return:	num faces
/////////////////////////////////////
PROTECTED DWORD MeshGetNumFace(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshGetNumInd
// Purpose:	get the number of indices
//			(total indices)
// Output:	none
// Return:	num indices
/////////////////////////////////////
PROTECTED DWORD MeshGetNumInd(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshLockVtx
// Purpose:	lock mesh vtx to get access
//			to vertices
// Output:	lock vtx, set pVtx
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE MeshLockVtx(gfxMesh *mesh, DWORD flag, gfxVtx **pVtx);

/////////////////////////////////////
// Name:	MeshUnlockVtx
// Purpose:	unlock vtx, use after
//			MeshLockVtx
// Output:	unlock vtx
// Return:	none
/////////////////////////////////////
PROTECTED void MeshUnlockVtx(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshGetVtxSys
// Purpose:	get the system vtx.
//			buffer
// Output:	vtx sys.
// Return:	none
/////////////////////////////////////
PROTECTED void MeshGetVtxSys(gfxMesh *mesh, gfxVtx **pVtx);

/////////////////////////////////////
// Name:	MeshLockInd
// Purpose:	lock mesh indices to get access
//			to index stuff
// Output:	lock indices, set pInd
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE MeshLockInd(gfxMesh *mesh, DWORD grpInd, DWORD flag, unsigned short **pInd);

/////////////////////////////////////
// Name:	MeshUnlockInd
// Purpose:	unlock ind, use after
//			MeshLockInd
// Output:	unlock ind
// Return:	none
/////////////////////////////////////
PROTECTED void MeshUnlockInd(gfxMesh *mesh, DWORD grpInd);

/////////////////////////////////////
// Name:	MeshGetBoneIDs
// Purpose:	get mesh bone IDs corresponding
//			to the vertices
// Output:	none
// Return:	the bone IDs
/////////////////////////////////////
PROTECTED char * MeshGetBoneIDs(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshComputeNormals
// Purpose:	compute vtx normals
// Output:	normals are calculated
// Return:	none
/////////////////////////////////////
PROTECTED void MeshComputeNormals(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshComputeBound
// Purpose:	compute the bounds
// Output:	pBoundOut is computed
// Return:	none
/////////////////////////////////////
PROTECTED void MeshComputeBound(gfxMesh *mesh, gfxBound *pBoundOut);

/////////////////////////////////////
// Name:	MeshNegativeTranslate
// Purpose:	subtract with given trans vector
// Output:	mesh translated
// Return:	none
/////////////////////////////////////
PROTECTED void MeshNegativeTranslate(gfxMesh *mesh, float trans[eMaxPt]);

/////////////////////////////////////
// Name:	MeshDisplay
// Purpose:	display mesh
// Output:	mesh displayed
// Return:	SUCCESS if success
/////////////////////////////////////
PROTECTED RETCODE MeshDisplay(gfxMesh *mesh, GFXMATERIAL *materials,
							  hTXT *textures);

/////////////////////////////////////
// Name:	MeshInterpolate
// Purpose:	interpolate vertices between
//			mesh1 and mesh2 and put in
//			meshOut
// Output:	meshOut set
// Return:	success
/////////////////////////////////////
PROTECTED RETCODE MeshInterpolate(gfxMesh *mesh1, gfxMesh *mesh2, float t,
								  gfxMesh *meshOut);

/////////////////////////////////////
// Name:	MeshCopyVtxToVtxSys
// Purpose:	copy mesh vtx to vtxjoint
// Output:	vtx buffer joint is filled
// Return:	success if success
/////////////////////////////////////
PROTECTED RETCODE MeshCopyVtxToVtxSys(gfxMesh *mesh);

/////////////////////////////////////
// Name:	MeshTranslateVtxJoint
// Purpose:	do the final translation
//			if joint to vertices
// Output:	vtx buffer joint is filled
// Return:	success if success
/////////////////////////////////////
PROTECTED RETCODE MeshTranslateVtxJoint(gfxMesh *mesh, gfxObjJoint *joints);

/////////////////////////////////////
// Name:	MeshUpdate
// Purpose:	update necessary stuff in
//			mesh. Stuff like FX
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshUpdate(gfxMesh *mesh);

#endif