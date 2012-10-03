#ifndef _gdix_modelfx_i_h
#define _gdix_modelfx_i_h

//THIS IS FOR INTERNAL GFX USE ONLY!

/////////////////////////////////////
// Name:	MeshFXCreate
// Purpose:	create the model fx
// Output:	stuff
// Return:	the new model fx
/////////////////////////////////////
PROTECTED gfxMeshFX * MeshFXCreate();

/////////////////////////////////////
// Name:	MeshFXDestroy
// Purpose:	destroys model FX
// Output:	model FX destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXDestroy(gfxMeshFX **meshFX);

/////////////////////////////////////
// Name:	MeshFXAddRenderState
// Purpose:	add a render state
// Output:	render state added to meshFX
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXAddRenderState(gfxMeshFX *meshFX, DWORD state, DWORD val);

/////////////////////////////////////
// Name:	MeshFXAddTextureState
// Purpose:	add a texture state
// Output:	texture state added to meshFX
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXAddTextureState(gfxMeshFX *meshFX, 
									 DWORD txtStage, DWORD state, DWORD val);

/////////////////////////////////////
// Name:	MeshFXSetTextureAnimTrans
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXSetTextureAnimTrans(gfxMeshFX *meshFX, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t);

/////////////////////////////////////
// Name:	MeshFXSetTextureAnimRot
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXSetTextureAnimRot(gfxMeshFX *meshFX, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t);

/////////////////////////////////////
// Name:	MeshFXSetTextureAnimScale
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXSetTextureAnimScale(gfxMeshFX *meshFX, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t);

/////////////////////////////////////
// Name:	MeshFXGetTextureAnimTrans
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXGetTextureAnimTrans(gfxMeshFX *meshFX, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t);

/////////////////////////////////////
// Name:	MeshFXGetTextureAnimRot
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXGetTextureAnimRot(gfxMeshFX *meshFX, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t);

/////////////////////////////////////
// Name:	MeshFXGetTextureAnimScale
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXGetTextureAnimScale(gfxMeshFX *meshFX, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t);

/////////////////////////////////////
// Name:	MeshFXEnableTextureAnim
// Purpose:	enable/disable texture anim
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXEnableTextureAnim(gfxMeshFX *meshFX, DWORD txtStage, bool bEnable);

/////////////////////////////////////
// Name:	MeshFXIsTextureAnim
// Purpose:	is texture anim enable?
// Output:	none
// Return:	true if so
/////////////////////////////////////
PROTECTED bool MeshFXIsTextureAnim(gfxMeshFX *meshFX, DWORD txtStage);

/////////////////////////////////////
// Name:	MeshFXSetTexture
// Purpose:	set the texture to apply
//			give txt = 0 to unset the
//			texture
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXSetTexture(gfxMeshFX *meshFX, DWORD txtStage, hTXT txt);

/////////////////////////////////////
// Name:	MeshFXPrepareStates
// Purpose:	sets up the render states
//			to the graphics pipeline
//			Will also set the texture
//			transformation
//			And also set the texture skin
//			if there is one
// Output:	states set
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXPrepareStates(gfxMeshFX *meshFX);

/////////////////////////////////////
// Name:	MeshFXUnPrepareStates
// Purpose:	revert back to old states before
//			MeshFXPrepareStates was called
//			Will also set the texture
//			transformation to iden.
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXUnPrepareStates(gfxMeshFX *meshFX);

/////////////////////////////////////
// Name:	MeshFXUpdateTextureAnim
// Purpose:	update texture anim
//			call this first before
//			preparing the mesh FX
// Output:	anim update
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXUpdateTextureAnim(gfxMeshFX *meshFX);

/////////////////////////////////////
// Name:	MeshFXGetRenderState
// Purpose:	get the render state value
//			if it exists
// Output:	valOut filled
// Return:	true if it exists
/////////////////////////////////////
PROTECTED bool MeshFXGetRenderState(gfxMeshFX *meshFX, DWORD state, DWORD *valOut);

/////////////////////////////////////
// Name:	MeshFXGetTextureState
// Purpose:	get the texture state value
//			if it exists
// Output:	valOut filled
// Return:	true if it exists
/////////////////////////////////////
PROTECTED bool MeshFXGetTextureState(gfxMeshFX *meshFX, 
									 DWORD txtStage, DWORD state, DWORD *valOut);

#endif