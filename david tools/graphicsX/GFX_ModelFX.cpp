#include "gdix.h"
#include "gdix_i.h"

D3DXMATRIX	g_idenMtx(1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0,
					  0, 0, 0, 1);

/////////////////////////////////////
// Name:	MeshFXCreate
// Purpose:	create the model fx
// Output:	stuff
// Return:	the new model fx
/////////////////////////////////////
PROTECTED gfxMeshFX * MeshFXCreate()
{
	gfxMeshFX *newFX = (gfxMeshFX *)GFX_MALLOC(sizeof(gfxMeshFX));

	if(!newFX) { ASSERT_MSG(0, "Unable to allocate model FX", "ModelFXCreate"); return 0; }

	newFX->renderStates  = new FXRenderStateArray; assert(newFX->renderStates);
	newFX->textureStates = new FXRenderStateDArray; assert(newFX->textureStates);

	newFX->textureStates->resize(NUMVTXTEXTURE);

	//initialize default values for texture animation...just set scale to '1'
	for(int i = 0; i < NUMVTXTEXTURE; i++)
	{
		newFX->txtAnim[i].scale.s = newFX->txtAnim[i].scale.t = 1;

		_GFXMathMtxLoadIden(&newFX->txtAnim[i].txt_mtx);
	}

	return newFX;
}

/////////////////////////////////////
// Name:	MeshFXDestroy
// Purpose:	destroys model FX
// Output:	model FX destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXDestroy(gfxMeshFX **meshFX)
{
	if(*meshFX)
	{
		if((*meshFX)->renderStates)
			delete (*meshFX)->renderStates;

		if((*meshFX)->textureStates)
			delete (*meshFX)->textureStates;

		for(int i = 0; i < NUMVTXTEXTURE; i++)
			TextureDestroy(&(*meshFX)->txt[i]);

		GFX_FREE(*meshFX);

		meshFX = 0;
	}
}

/////////////////////////////////////
// Name:	MeshFXAddRenderState
// Purpose:	add a render state
// Output:	render state added to meshFX
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXAddRenderState(gfxMeshFX *meshFX, DWORD state, DWORD val)
{
	FXRenderState newState = {state, val, 0};

	//first check to see if this render state already exists
	for(int i = 0; i < meshFX->renderStates->size(); i++)
	{
		if((*meshFX->renderStates)[i].state == state)
		{ (*meshFX->renderStates)[i].value = val; return; }
	}

	meshFX->renderStates->push_back(newState);
}

/////////////////////////////////////
// Name:	MeshFXAddTextureState
// Purpose:	add a texture state
// Output:	texture state added to meshFX
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXAddTextureState(gfxMeshFX *meshFX, 
									 DWORD txtStage, DWORD state, DWORD val)
{
	assert(txtStage < NUMVTXTEXTURE);

	FXRenderState newState = {state, val, 0};

	//first check to see if this texture state already exists
	for(int i = 0; i < (*meshFX->textureStates)[txtStage].size(); i++)
	{
		if((*meshFX->textureStates)[txtStage][i].state == state)
		{ (*meshFX->textureStates)[txtStage][i].value = val; return; }
	}

	(*meshFX->textureStates)[txtStage].push_back(newState);
}

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
									float max_s, float max_t)
{
	assert(txtStage < NUMVTXTEXTURE);

	meshFX->txtAnim[txtStage].trans.s = meshFX->txtAnim[txtStage].trans.t = 0;
	
	meshFX->txtAnim[txtStage].trans.spd_s = spd_s;
	meshFX->txtAnim[txtStage].trans.spd_t = spd_t;

	meshFX->txtAnim[txtStage].trans.acc_s = acc_s;
	meshFX->txtAnim[txtStage].trans.acc_t = acc_t;

	meshFX->txtAnim[txtStage].trans.min_s = min_s;
	meshFX->txtAnim[txtStage].trans.min_t = min_t;

	meshFX->txtAnim[txtStage].trans.max_s = max_s;
	meshFX->txtAnim[txtStage].trans.max_t = max_t;
}

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
									float max_s, float max_t)
{
	assert(txtStage < NUMVTXTEXTURE);

	meshFX->txtAnim[txtStage].rotate.s = meshFX->txtAnim[txtStage].rotate.t = 0;
	
	meshFX->txtAnim[txtStage].rotate.spd_s = spd_s;
	meshFX->txtAnim[txtStage].rotate.spd_t = spd_t;

	meshFX->txtAnim[txtStage].rotate.acc_s = acc_s;
	meshFX->txtAnim[txtStage].rotate.acc_t = acc_t;

	meshFX->txtAnim[txtStage].rotate.min_s = min_s;
	meshFX->txtAnim[txtStage].rotate.min_t = min_t;

	meshFX->txtAnim[txtStage].rotate.max_s = max_s;
	meshFX->txtAnim[txtStage].rotate.max_t = max_t;
}

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
									float max_s, float max_t)
{
	assert(txtStage < NUMVTXTEXTURE);

	meshFX->txtAnim[txtStage].scale.s = meshFX->txtAnim[txtStage].scale.t = 0;
	
	meshFX->txtAnim[txtStage].scale.spd_s = spd_s;
	meshFX->txtAnim[txtStage].scale.spd_t = spd_t;

	meshFX->txtAnim[txtStage].scale.acc_s = acc_s;
	meshFX->txtAnim[txtStage].scale.acc_t = acc_t;

	meshFX->txtAnim[txtStage].scale.min_s = min_s;
	meshFX->txtAnim[txtStage].scale.min_t = min_t;

	meshFX->txtAnim[txtStage].scale.max_s = max_s;
	meshFX->txtAnim[txtStage].scale.max_t = max_t;
}

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
									float *max_s, float *max_t)
{
	assert(txtStage < NUMVTXTEXTURE);
	
	*spd_s = meshFX->txtAnim[txtStage].trans.spd_s;
	*spd_t = meshFX->txtAnim[txtStage].trans.spd_t;

	*acc_s = meshFX->txtAnim[txtStage].trans.acc_s;
	*acc_t = meshFX->txtAnim[txtStage].trans.acc_t;

	*min_s = meshFX->txtAnim[txtStage].trans.min_s;
	*min_t = meshFX->txtAnim[txtStage].trans.min_t;

	*max_s = meshFX->txtAnim[txtStage].trans.max_s;
	*max_t = meshFX->txtAnim[txtStage].trans.max_t;
}

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
									float *max_s, float *max_t)
{
	assert(txtStage < NUMVTXTEXTURE);
	
	*spd_s = meshFX->txtAnim[txtStage].rotate.spd_s;
	*spd_t = meshFX->txtAnim[txtStage].rotate.spd_t;

	*acc_s = meshFX->txtAnim[txtStage].rotate.acc_s;
	*acc_t = meshFX->txtAnim[txtStage].rotate.acc_t;

	*min_s = meshFX->txtAnim[txtStage].rotate.min_s;
	*min_t = meshFX->txtAnim[txtStage].rotate.min_t;

	*max_s = meshFX->txtAnim[txtStage].rotate.max_s;
	*max_t = meshFX->txtAnim[txtStage].rotate.max_t;
}

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
									float *max_s, float *max_t)
{
	assert(txtStage < NUMVTXTEXTURE);
	
	*spd_s = meshFX->txtAnim[txtStage].scale.spd_s;
	*spd_t = meshFX->txtAnim[txtStage].scale.spd_t;

	*acc_s = meshFX->txtAnim[txtStage].scale.acc_s;
	*acc_t = meshFX->txtAnim[txtStage].scale.acc_t;

	*min_s = meshFX->txtAnim[txtStage].scale.min_s;
	*min_t = meshFX->txtAnim[txtStage].scale.min_t;

	*max_s = meshFX->txtAnim[txtStage].scale.max_s;
	*max_t = meshFX->txtAnim[txtStage].scale.max_t;
}

/////////////////////////////////////
// Name:	MeshFXEnableTextureAnim
// Purpose:	enable/disable texture anim
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXEnableTextureAnim(gfxMeshFX *meshFX, DWORD txtStage, bool bEnable)
{
	assert(txtStage < NUMVTXTEXTURE);

	meshFX->txtAnim[txtStage].bEnable = bEnable;
}

/////////////////////////////////////
// Name:	MeshFXIsTextureAnim
// Purpose:	is texture anim enable?
// Output:	none
// Return:	true if so
/////////////////////////////////////
PROTECTED bool MeshFXIsTextureAnim(gfxMeshFX *meshFX, DWORD txtStage)
{
	assert(txtStage < NUMVTXTEXTURE);

	return meshFX->txtAnim[txtStage].bEnable;
}

/////////////////////////////////////
// Name:	MeshFXSetTexture
// Purpose:	set the texture to apply
//			give txt = 0 to unset the
//			texture
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXSetTexture(gfxMeshFX *meshFX, DWORD txtStage, hTXT txt)
{
	assert(txtStage < NUMVTXTEXTURE);

	if(txt)
	{
		TextureDestroy(&meshFX->txt[txtStage]);

		meshFX->txt[txtStage] = txt;

		TextureAddRef(meshFX->txt[txtStage]);
	}
	else
	{
		TextureDestroy(&meshFX->txt[txtStage]);
		meshFX->txt[txtStage] = 0;
	}
}

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
PROTECTED void MeshFXPrepareStates(gfxMeshFX *meshFX)
{
	int i;

	//set the render states
	for(i = 0; i < meshFX->renderStates->size(); i++)
	{
		D3DRENDERSTATETYPE state = (D3DRENDERSTATETYPE)(*meshFX->renderStates)[i].state;
		DWORD			   val	 = (*meshFX->renderStates)[i].value;

		g_p3DDevice->GetRenderState(state, &(*meshFX->renderStates)[i].oldValue);

		if(val != (*meshFX->renderStates)[i].oldValue)
			g_p3DDevice->SetRenderState(state, val);
	}

	//set the texture states
	for(i = 0; i < NUMVTXTEXTURE; i++)
	{
		for(int j = 0; j < (*meshFX->textureStates)[i].size(); j++)
		{
			D3DTEXTURESTAGESTATETYPE state = (D3DTEXTURESTAGESTATETYPE)(*meshFX->textureStates)[i][j].state;
			DWORD					 val   = (*meshFX->textureStates)[i][j].value;

			DWORD oldVal;

			g_p3DDevice->GetTextureStageState(i, state, &oldVal);

			(*meshFX->textureStates)[i][j].oldValue = oldVal;

			if(val != oldVal)
				g_p3DDevice->SetTextureStageState(i, state, val);
		}

		//set texture mtx. D3DTS_TEXTURE0
		g_p3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+i), 
			&meshFX->txtAnim[i].txt_mtx.mtxWrld);

		if(meshFX->txt[i])
			TextureSet(meshFX->txt[i], i);
	}
}

/////////////////////////////////////
// Name:	MeshFXUnPrepareStates
// Purpose:	revert back to old states before
//			MeshFXPrepareStates was called
//			Will also set the texture
//			transformation to iden.
// Output:	stuff
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXUnPrepareStates(gfxMeshFX *meshFX)
{
	int i;

	//set the render states
	for(i = 0; i < meshFX->renderStates->size(); i++)
	{
		D3DRENDERSTATETYPE state = (D3DRENDERSTATETYPE)(*meshFX->renderStates)[i].state;
		DWORD			   val	 = (*meshFX->renderStates)[i].oldValue;

		g_p3DDevice->SetRenderState(state, val);
	}

	//set the texture states
	for(i = 0; i < NUMVTXTEXTURE; i++)
	{
		for(int j = 0; j < (*meshFX->textureStates)[i].size(); j++)
		{
			D3DTEXTURESTAGESTATETYPE state = (D3DTEXTURESTAGESTATETYPE)(*meshFX->textureStates)[i][j].state;
			DWORD					 val   = (*meshFX->textureStates)[i][j].oldValue;

			g_p3DDevice->SetTextureStageState(i, state, val);
		}

		//set texture mtx. D3DTS_TEXTURE0
		g_p3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+i), 
			&g_idenMtx);

		TextureSet(0, i);
	}
}

PRIVATE inline void _TextureAnimUpdate(gfxFXTxtAnimDat & dat)
{
	dat.s += dat.spd_s;
	dat.t += dat.spd_t;

	dat.spd_s += dat.acc_s;
	dat.spd_t += dat.acc_s;

	if(dat.s < dat.min_s || dat.s > dat.max_s)
	{
		if(dat.acc_s != 0)
		{
			dat.spd_s = 0;
			dat.acc_s *= -1;
		}
		else
			dat.spd_s *= -1;

		dat.s = dat.s < dat.min_s ? dat.min_s : dat.max_s;
	}

	if(dat.t < dat.min_t || dat.t > dat.max_t)
	{
		if(dat.acc_t != 0)
		{
			dat.spd_t = 0;
			dat.acc_t *= -1;
		}
		else
			dat.spd_t *= -1;

		dat.t = dat.t < dat.min_t ? dat.min_t : dat.max_t;
	}
}

/////////////////////////////////////
// Name:	MeshFXUpdateTextureAnim
// Purpose:	update texture anim
//			call this first before
//			preparing the mesh FX
// Output:	anim update
// Return:	none
/////////////////////////////////////
PROTECTED void MeshFXUpdateTextureAnim(gfxMeshFX *meshFX)
{
	for(int i = 0; i < NUMVTXTEXTURE; i++)
	{
		_TextureAnimUpdate(meshFX->txtAnim[i].trans);
		_TextureAnimUpdate(meshFX->txtAnim[i].rotate);
		_TextureAnimUpdate(meshFX->txtAnim[i].scale);

		if(meshFX->txtAnim[i].scale.s != 0 
			&& meshFX->txtAnim[i].scale.t != 0)
			D3DXMatrixScaling(&meshFX->txtAnim[i].txt_mtx.mtxScale,
				meshFX->txtAnim[i].scale.s, meshFX->txtAnim[i].scale.t, 0);

		D3DXMatrixRotationYawPitchRoll(&meshFX->txtAnim[i].txt_mtx.mtxRot,
			meshFX->txtAnim[i].rotate.t, meshFX->txtAnim[i].rotate.s, 0);

		D3DXMatrixTranslation(&meshFX->txtAnim[i].txt_mtx.mtxTrans,
			meshFX->txtAnim[i].trans.s, meshFX->txtAnim[i].trans.t, 0);

		_GFXCreateWrld(&meshFX->txtAnim[i].txt_mtx);
	}
}

/////////////////////////////////////
// Name:	MeshFXGetRenderState
// Purpose:	get the render state value
//			if it exists
// Output:	valOut filled
// Return:	true if it exists
/////////////////////////////////////
PROTECTED bool MeshFXGetRenderState(gfxMeshFX *meshFX, DWORD state, DWORD *valOut)
{
	for(int i = 0; i < meshFX->renderStates->size(); i++)
	{
		if((*meshFX->renderStates)[i].state == state)
		{
			*valOut = (*meshFX->renderStates)[i].value;
			return true;
		}
	}

	return false;
}

/////////////////////////////////////
// Name:	MeshFXGetTextureState
// Purpose:	get the texture state value
//			if it exists
// Output:	valOut filled
// Return:	true if it exists
/////////////////////////////////////
PROTECTED bool MeshFXGetTextureState(gfxMeshFX *meshFX, 
									 DWORD txtStage, DWORD state, DWORD *valOut)
{
	assert(txtStage < NUMVTXTEXTURE);

	for(int i = 0; i < (*meshFX->textureStates)[txtStage].size(); i++)
	{
		if((*meshFX->textureStates)[txtStage][i].state == state)
		{ 
			*valOut = (*meshFX->textureStates)[txtStage][i].value; 
			return true; 
		}
	}

	return false;
}