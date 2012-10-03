#include "gdix.h"
#include "gdix_i.h"
#include "gdix_mesh_i.h"
#include "gdiX_joints_i.h"

//All object creation/destruction/manipulation are done here

PROTECTED void _destroyObj(hOBJ *obj) //this will destroy the object, will not search list
{
	if(*obj)
	{
		GFXPageRemove(&(*obj)->key);

		//go through object's children and clear those out
		if((*obj)->objNode)
		{
			hOBJ thisObj;
			for(LISTPTR::iterator i = (*obj)->objNode->begin(); i != (*obj)->objNode->end(); ++i)
			{
				thisObj = (hOBJ)(*i);
				_destroyObj(&thisObj);
			}

			delete (*obj)->objNode;
		}

		if((*obj)->theMdl)
		{
			if((*obj)->materials)
				GFX_FREE((*obj)->materials);

			if((*obj)->textures)
			{
				for(int i = 0; i < (*obj)->theMdl->numMaterial; i++)
					TextureDestroy(&(*obj)->textures[i]);

				GFX_FREE((*obj)->textures);
			}

			MDLDestroy(&(*obj)->theMdl);
		}

		if((*obj)->states)
			GFX_FREE((*obj)->states);

		if((*obj)->joints)
			GFX_FREE((*obj)->joints);

		GFX_FREE(*obj);
		obj=0;
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJCreate(unsigned int newID, hMDL model, float x, float y, float z, 
					  eOBJTYPE objType, const char *animationfile)
{
	assert(model);

	hOBJ newObj = (hOBJ)GFX_MALLOC(sizeof(gfxObject));

	if(!newObj)
	{ ASSERT_MSG(0, "Unable to allocate new object", "Error in OBJCreate"); return 0; }

	newObj->ID = newID;

	assert(model);
	
	newObj->theMdl = model;
	MDLAddRef(newObj->theMdl);
	newObj->curMesh = &model->mesh;

	gfxBound mBound = MDLGetBound(model);
	OBJSetDrawBound(newObj, mBound.min, mBound.max);

	//load the animation
	char ext[MAXCHARBUFF]; //file extension

	if(animationfile && animationfile[0] != 0)
	{
		strcpy(ext, GetExtension(animationfile));

		if(stricmp(ext, ".anm") == 0) //key frame animation states
		{
			if(_ObjLoadAnimationKey(newObj, animationfile) == RETCODE_FAILURE)
			{ _destroyObj(&newObj); return 0; }
		}
	}

	//set up joints, if there are any
	if(TESTFLAGS(model->status, MDL_JOINT))
	{
		//allocate object joint
		newObj->joints = (gfxObjJoint*)GFX_MALLOC(sizeof(gfxObjJoint)*model->numJoints);
		if(!newObj->joints)
		{ ASSERT_MSG(0, "Unable to allocate object joints", "OBJCreate"); _destroyObj(&newObj); return 0; }

		//initialize joints
		for(int i = 0; i < model->numJoints; i++)
			JointSetObj(&newObj->joints[i], &model->joints[i]);

		//initialize the vertex joints
		MeshTranslateVtxJoint(newObj->curMesh, newObj->joints);
	}

	//set up textures and materials
	//reason for this: since several objects might share the same model.
	if(model->numMaterial > 0)
	{
		//allocate textures and materials and copy from model
		newObj->materials = (GFXMATERIAL*)GFX_MALLOC(sizeof(GFXMATERIAL)*model->numMaterial);
		if(!newObj->materials)
		{ ASSERT_MSG(0, "Unable to allocate object materials", "OBJCreate"); _destroyObj(&newObj); return 0; }

		newObj->textures = (hTXT*)GFX_MALLOC(sizeof(hTXT)*model->numMaterial);
		if(!newObj->textures)
		{ ASSERT_MSG(0, "Unable to allocate object textures", "OBJCreate"); _destroyObj(&newObj); return 0; }

		memcpy(newObj->materials, model->materials, sizeof(GFXMATERIAL)*model->numMaterial);

		for(int i = 0; i < model->numMaterial; i++)
		{
			newObj->textures[i] = model->textures[i];
			TextureAddRef(newObj->textures[i]);
		}
	}

	//initialize matrices
	_GFXMathMtxLoadIden(&newObj->wrldMtx);

	//set object position
	float v[eMaxPt] = {x, y, z};

	OBJTranslate(newObj, v, false);

	//set scale to default
	v[eX] = v[eY] = v[eZ] = 1;
	OBJScale(newObj, v, false);

	//create the world matrix
	_GFXCreateWrld(&newObj->wrldMtx);

	//set animation scale to 1
	newObj->animeScale = 1;

	//initialize list
	newObj->objNode = new LISTPTR;

	//append object to list
	g_OBJLIST.insert(g_OBJLIST.end(), (unsigned int)newObj);
	newObj->listRef = &g_OBJLIST;

	GFXPageAdd(newObj, &newObj->key);

	return newObj;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJAddRef(hOBJ obj)
{
	obj->ref++;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJSearch(unsigned int ID)
{
	hOBJ thisObj;

	for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
	{
		thisObj = (hOBJ)(*i);

		if(thisObj->ID == ID)
			return thisObj;
	}

	return 0;

	//return (hOBJ)GFXPageGet(ID, 0);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _OBJDestroyAll()
{
	hOBJ thisObj;

	for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		_destroyObj(&thisObj);
	}

	for(LISTPTR::iterator j = g_OBJDEACTIVATELIST.begin(); j != g_OBJDEACTIVATELIST.end(); ++j)
	{
		thisObj = (hOBJ)(*j);
		_destroyObj(&thisObj);
	}
	

	g_OBJLIST.clear();
	g_OBJDEACTIVATELIST.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJDestroy(hOBJ *obj)
{
	if(*obj)
	{
		if((*obj)->ref <= 0)
		{
			//just remove the pointer of object from list
			(*obj)->listRef->remove((unsigned int)(*obj));

			_destroyObj(obj);
		}
		else
			(*obj)->ref--;
	}

	obj=0;
}

/////////////////////////////////////
// Name:	OBJTranslate
// Purpose:	translate object, bAdd is
//			used for moving
// Output:	object translated
// Return:	none
/////////////////////////////////////
PUBLIC void OBJTranslate(hOBJ obj, float trans[eMaxPt], bool bAdd)
{
	if(bAdd)
	{ obj->wrldTrans[eX] += trans[eX]; obj->wrldTrans[eY] += trans[eY]; obj->wrldTrans[eZ] += trans[eZ]; }
	else
	{ obj->wrldTrans[eX] = trans[eX]; obj->wrldTrans[eY] = trans[eY]; obj->wrldTrans[eZ] = trans[eZ]; }

	//set the translation matrix
	D3DXMatrixTranslation(&obj->wrldMtx.mtxTrans, 
		obj->wrldTrans[eX], obj->wrldTrans[eY], obj->wrldTrans[eZ]);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJRotX(hOBJ obj, float radian)
{
	obj->wrldRot[eX] += radian;

	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationX(&mtxR, radian);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&mtxR,&obj->wrldMtx.mtxRot);
}

PUBLIC void OBJRotY(hOBJ obj, float radian)
{
	obj->wrldRot[eY] += radian;

	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationY(&mtxR, radian);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&mtxR,&obj->wrldMtx.mtxRot);
}

PUBLIC void OBJRotZ(hOBJ obj, float radian)
{
	obj->wrldRot[eZ] += radian;

	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationZ(&mtxR, radian);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&mtxR,&obj->wrldMtx.mtxRot);
}

PUBLIC void OBJRot(hOBJ obj, float rot[eMaxPt], bool bAdd)
{
	if(bAdd)
	{
		obj->wrldRot[eX] += rot[eX];
		obj->wrldRot[eY] += rot[eY];
		obj->wrldRot[eZ] += rot[eZ];
	}
	else
	{
		obj->wrldRot[eX] = rot[eX];
		obj->wrldRot[eY] = rot[eY];
		obj->wrldRot[eZ] = rot[eZ];
	}

	D3DXMATRIX mtxR;

	D3DXMatrixRotationYawPitchRoll(&obj->wrldMtx.mtxRot,
				obj->wrldRot[eY],
				obj->wrldRot[eX],
				obj->wrldRot[eZ]);
}

/////////////////////////////////////
// Name:	OBJScale
// Purpose:	scale the object
// Output:	obj scale
// Return:	none
/////////////////////////////////////
PUBLIC void OBJScale(hOBJ obj, float s[eMaxPt], bool bAdd)
{
	if(bAdd)
	{ obj->wrldScl[eX] += s[eX]; obj->wrldScl[eY] += s[eY]; obj->wrldScl[eZ] += s[eZ]; }
	else
	{ obj->wrldScl[eX] = s[eX]; obj->wrldScl[eY] = s[eY]; obj->wrldScl[eZ] = s[eZ]; }

	//set the translation matrix
	D3DXMatrixScaling(&obj->wrldMtx.mtxScale,
		obj->wrldScl[eX], obj->wrldScl[eY], obj->wrldScl[eZ]);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJDisplay(hOBJ obj, bool bApplyLight)
{
	assert(obj);
	assert(obj->theMdl);

	float trans[eMaxPt]; OBJGetLoc(obj, trans);

	if(!FrustrumCheckBox(
		obj->drawBoundMin[eX]+trans[eX], 
		obj->drawBoundMin[eY]+trans[eY], 
		obj->drawBoundMin[eZ]+trans[eZ],
		obj->drawBoundMax[eX]+trans[eX], 
		obj->drawBoundMax[eY]+trans[eY], 
		obj->drawBoundMax[eZ]+trans[eZ]))
	{
#ifdef _DEBUG
		g_numCulled++;
#endif
		return RETCODE_SUCCESS;
	}

	//check if the object is too far away
	float R = kDEPTHFAR*kDEPTHFAR;
	D3DXVECTOR3 d(trans[eX],trans[eY],trans[eZ]);
	d -=  g_eyePt;
	float lSqr = D3DXVec3LengthSq(&d);

	if(lSqr > R)
	{
#ifdef _DEBUG
		g_numCulled++;
#endif
		return RETCODE_SUCCESS;
	}

	if(bApplyLight)
		LightEnableAllNearest(trans);

	//sigh...translate mesh vertices to joints
	//update the vertices
	if(TESTFLAGS(obj->theMdl->status, MDL_JOINT))
		MeshTranslateVtxJoint(obj->curMesh, obj->joints);

	//set up culling
	if(TESTFLAGS(obj->theMdl->status, MDL_BACKFACECCW))
		g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	else
		g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//Vertex shaders are a complex topic, but you can do some amazing things with them
    //For this example we're not creating one, so we tell Direct3D that we're just
    //using a plain vertex format.
    g_p3DDevice->SetFVF(GFXVERTEXFLAG);

	//push world stack and set this object's world mtx
	g_pWrldStack->Push();
	g_pWrldStack->MultMatrixLocal(&obj->wrldMtx.mtxWrld);

	//do transformations
	D3DXMATRIX *pMtx = g_pWrldStack->GetTop();
    g_p3DDevice->SetTransform(D3DTS_WORLD, g_pWrldStack->GetTop());

	//draw the darn thing
	MeshDisplay(obj->curMesh, obj->materials, obj->textures);

	//display this object's children within it's world mtx
	for(LISTPTR::iterator j = obj->objNode->begin(); j != obj->objNode->end(); ++j)
		OBJDisplay((hOBJ)(*j));

	//take this junk out!
	g_pWrldStack->Pop();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJUpdateFrame(hOBJ obj)
{
	//create the world matrix
	_GFXCreateWrld(&obj->wrldMtx);

	if(!TESTFLAGS(obj->status, OBJ_PAUSE))
	{
		if(obj->states)
		{
			//there can either be keyframes or joints,
			//we cannot have both!

			//if there are key frames...
			if(TESTFLAGS(obj->theMdl->status, MDL_FRAME))
			{
				double time = TimeGetTime(&obj->ticker)*obj->animeScale;

				if(obj->states[obj->curState].delay > 0)
					time /= obj->states[obj->curState].delay;
				else
					time = 0;

				//time *= obj->animeScale;

				//advance to next frame if we reach 1
				while(time >= 1)
				{
					TimeReset(&obj->ticker);

					obj->prevFrame = obj->curFrame;
					obj->curFrame++;

					if(obj->curFrame > obj->states[obj->curState].endFrame)
					{
						if(TESTFLAGS(obj->status, OBJ_PAUSE_ON_END))
						{ OBJPause(obj, true); obj->curFrame = obj->states[obj->curState].endFrame; }
						else
							obj->curFrame = obj->states[obj->curState].startFrame;
					}

					time -= 1;
				}

				MeshInterpolate(&obj->theMdl->frames[obj->prevFrame].frameMesh, 
					&obj->theMdl->frames[obj->curFrame].frameMesh, 
					time, obj->curMesh);
			}
			//otherwise, if there are joints...
			else if(TESTFLAGS(obj->theMdl->status, MDL_JOINT))
			{
				double time = TimeGetTime(&obj->ticker)*obj->animeScale;

				if(obj->states[obj->curState].delay > 0)
					time /= obj->states[obj->curState].delay;
				else
					time = 0;

				//time *= obj->animeScale;

				//advance to next frame if we reach 1
				while(time >= 1)
				{
					TimeReset(&obj->ticker);

					obj->prevFrame = obj->curFrame;
					obj->curFrame++;

					if(obj->curFrame > obj->states[obj->curState].endFrame)
					{
						if(TESTFLAGS(obj->status, OBJ_PAUSE_ON_END))
						{ 
							SETFLAG(obj->status, OBJ_PAUSE); 
							obj->curFrame = obj->states[obj->curState].endFrame;
							time=0;
							break;
						}
						else
						{
							obj->curFrame = obj->states[obj->curState].startFrame;
						}
					}

					time -= 1;
				}

				//interpolate joint
				if(TESTFLAGS(obj->status, OBJ_CHILD))
				{
					hOBJ parent = OBJQuery(&obj->parentKey);

					JointInterpolate(obj->joints,
									obj->theMdl->joints,
									obj->theMdl->numJoints, time,
									obj->theMdl->numFrames, obj->prevFrame, obj->curFrame,
									parent ? parent->joints : 0, obj->parentJointInd);
				}
				else
				{
					JointInterpolate(obj->joints,
									obj->theMdl->joints,
									obj->theMdl->numJoints, time,
									obj->theMdl->numFrames, obj->prevFrame, obj->curFrame);
				}

				//MeshTranslateVtxJoint(obj->curMesh, obj->joints);
			}
		}

		//go through the children and update them...
		for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
			OBJUpdateFrame((hOBJ)(*i));
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int OBJGetCurState(hOBJ obj)
{
	return obj->curState;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJSetState(hOBJ obj, int state)
{
	if((state >= obj->numStates) || (state < 0))
		return RETCODE_OUTOFBOUND;

	if(state == obj->curState)
		return RETCODE_SUCCESS;

	obj->curState = state;

	obj->curFrame = obj->states[obj->curState].startFrame;

	//set up timer
	TimeInit(&obj->ticker, obj->states[obj->curState].delay);

	hOBJ thisObj;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);

		if(TESTFLAGS(thisObj->status, OBJ_SYNCSTATE))
			OBJSetState(thisObj, state);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	OBJIsStateEnd
// Purpose:	checks to see if the state
//			of an object reached the end
//			frame of current state
// Output:	none
// Return:	true if curframe reached end
/////////////////////////////////////
PUBLIC bool OBJIsStateEnd(hOBJ obj)
{
	if(obj->curFrame == obj->states[obj->curState].endFrame)
		return true;
	return false;
}

/////////////////////////////////////
// Name:	OBJPause
// Purpose:	pause / resume  object
//			(true)  (false)
// Output:	Obj flag is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJPause(hOBJ obj, bool bPause)
{
	if(bPause)
		SETFLAG(obj->status, OBJ_PAUSE);
	else
		CLEARFLAG(obj->status, OBJ_PAUSE);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJPauseOnStateEnd(hOBJ obj, bool bPause)
{
	if(bPause)
		SETFLAG(obj->status, OBJ_PAUSE_ON_END);
	else
	{
		CLEARFLAG(obj->status, OBJ_PAUSE_ON_END);
		CLEARFLAG(obj->status, OBJ_PAUSE);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJAddChild(hOBJ obj, hOBJ child, int parentJointInd, bool syncState)
{
	//remove child from it's refered list
	if(TESTFLAGS(child->status, OBJ_CHILD))
	{
		OBJDestroy(&child); //this will just subtract the child ref
		//OBJRemoveFromParent(child);
	}

	child->listRef->remove((unsigned int)child);

	obj->objNode->push_back((unsigned int)child);
	child->listRef = obj->objNode;

	OBJAddRef(child);

	//set flags
	SETFLAG(child->status, OBJ_CHILD);

	//set parent ID key
	child->parentKey = obj->key;

	//set parent joint
	child->parentJointInd = parentJointInd;

	if(syncState) //will the child synchronize state with parent?
	{
		SETFLAG(child->status, OBJ_SYNCSTATE);
		child->curState = obj->curState;
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	OBJRemoveFromParent
// Purpose:	remove object from it's
//			parent, if it has one
// Output:	parent free object
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE OBJRemoveFromParent(hOBJ obj)
{
	if(obj && TESTFLAGS(obj->status, OBJ_CHILD)) //does it still exist?  It damn better!
	{
		OBJDestroy(&obj); //this will just subtract the child ref

		//remove child from it's refered list
		//in this case, it would be obj's children list
		obj->listRef->remove((unsigned int)obj);

		//put the child back to the global object list
		g_OBJLIST.push_back((unsigned int)obj);
		obj->listRef = &g_OBJLIST;

		CLEARFLAG(obj->status, OBJ_CHILD);
		CLEARFLAG(obj->status, OBJ_SYNCSTATE);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJRemoveChild(hOBJ obj, hOBJ child)
{
	//we will assume that the child was added to the given obj
	//before this function.

	//OBJDestroy(&child); //this will just subtract the child ref

	OBJRemoveFromParent(child);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	OBJGetModel
// Purpose:	grabs model of obj.
// Output:	none
// Return:	the model of obj.
/////////////////////////////////////
PUBLIC hMDL OBJGetModel(hOBJ obj)
{
	return obj->theMdl;
}

/////////////////////////////////////
// Name:	OBJGetLoc
// Purpose:	grabs the location of 
//			the obj.  This is only
//			the translation(local/world)
// Output:	loc is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetLoc(hOBJ obj, float loc[eMaxPt])
{
	memcpy(loc, obj->wrldTrans, sizeof(float)*eMaxPt);

	if(TESTFLAGS(obj->status, OBJ_CHILD))
	{
		hOBJ parentOBJ = OBJQuery(&obj->parentKey);

		if(parentOBJ)
		{
			float parentLoc[eMaxPt];

			OBJGetLoc(parentOBJ, parentLoc);

			loc[eX] += parentLoc[eX];
			loc[eY] += parentLoc[eY];
			loc[eZ] += parentLoc[eZ];

			if(obj->parentJointInd != -1)
			{
				OBJJointGetTranslate(parentOBJ, obj->parentJointInd, parentLoc);

				loc[eX] += parentLoc[eX];
				loc[eY] += parentLoc[eY];
				loc[eZ] += parentLoc[eZ];
			}
		}
	}
}

PRIVATE void _OBJGetWrldMtx(hOBJ obj, D3DXMATRIX *pMtx)
{
	if(!D3DXMatrixIsIdentity(pMtx))
		D3DXMatrixMultiply(pMtx, pMtx, &obj->wrldMtx.mtxWrld);
	else
		*pMtx = obj->wrldMtx.mtxWrld;

	if(TESTFLAGS(obj->status, OBJ_CHILD))
	{
		hOBJ parentOBJ = OBJQuery(&obj->parentKey);

		if(parentOBJ)
		{
			_OBJGetWrldMtx(parentOBJ, pMtx);

			if(obj->parentJointInd != -1)
			{
				float loc[eMaxPt];
				OBJJointGetTranslate(parentOBJ, obj->parentJointInd, loc);

				pMtx->_41 += loc[eX];
				pMtx->_42 += loc[eY];
				pMtx->_43 += loc[eZ];

				/*D3DXMATRIX nMtx;

				D3DXMatrixTranslation(&nMtx, loc[eX], loc[eY], loc[eZ]);

				D3DXMatrixMultiply(pMtx, pMtx, &nMtx);*/
			}
		}
	}
}

/////////////////////////////////////
// Name:	OBJGetWorldLoc
// Purpose:	grabs the actual world 
//			location
// Output:	loc
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetWorldLoc(hOBJ obj, float loc[eMaxPt])
{
	D3DXMATRIX mtx; D3DXMatrixIdentity(&mtx);

	_OBJGetWrldMtx(obj, &mtx);

	loc[eX] = mtx._41;
	loc[eY] = mtx._42;
	loc[eZ] = mtx._43;
}

/////////////////////////////////////
// Name:	OBJGetScale
// Purpose:	grabs the scale value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetScale(hOBJ obj, float scl[eMaxPt])
{
	memcpy(scl, obj->wrldScl, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	OBJGetRotate
// Purpose:	grabs the rotate value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetRotate(hOBJ obj, float rot[eMaxPt])
{
	memcpy(rot, obj->wrldRot, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumFace(hOBJ obj)
{
	return MeshGetNumFace(obj->curMesh);
}

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumVtx(hOBJ obj)
{
	return MeshGetNumVtx(obj->curMesh);
}

/////////////////////////////////////
// Name:	OBJGetNumModel	
// Purpose:	This will give you the num
//			of models associated with
//			the obj.  This will go through
//			the children as well.
// Output:	none
// Return:	num model
/////////////////////////////////////
PUBLIC int OBJGetNumModel(hOBJ obj)
{
	int numMdl = 1;

	hOBJ thisObj;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		numMdl += OBJGetNumModel(thisObj);
	}

	return numMdl;
}

/////////////////////////////////////
// Name:	OBJGetBounds
// Purpose:	fills up the bounds array
//			with bound info of obj and
//			all it's children.
//			NOTE:  Assumes that bounds
//			is allocated to the number of
//			models associated w/ object
// Output:	bounds is filled
// Return:	count of bounds filled
/////////////////////////////////////
PUBLIC int OBJGetBounds(hOBJ obj, gfxBound *bounds, bool bGet1stOnly)
{
	//get first bound
	*bounds = MDLGetBound(obj->theMdl);

	bounds->radius *= obj->wrldScl[eX];	//assume equal scaling on all axis

	//add offset to object center if object is a child
	if(TESTFLAGS(obj->status, OBJ_CHILD))
	{
		float local[eMaxPt]; OBJGetWorldLoc(obj, local);

		(*bounds).center[eX] += local[eX];
		(*bounds).center[eY] += local[eY];
		(*bounds).center[eZ] += local[eZ];
		
		(*bounds).min[eX] += local[eX];
		(*bounds).min[eY] += local[eY];
		(*bounds).min[eZ] += local[eZ];
		
		(*bounds).max[eX] += local[eX];
		(*bounds).max[eY] += local[eY];
		(*bounds).max[eZ] += local[eZ];
	}

	if(bGet1stOnly)
		return 1;

	hOBJ thisObj;
	int count = 1;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		count += OBJGetBounds(thisObj, bounds + count);
	}

	return count;
}

/////////////////////////////////////
// Name:	OBJIntersectLine
// Purpose:	checks to see if the given
//			object is intersecting a line
//			give the bounding box (min/max)
//			if you want box to box collision
// Output:	time 't' between two points
// Return:	true if collided
/////////////////////////////////////
PUBLIC bool OBJIntersectLine(hOBJ obj, float mins[eMaxPt], float maxs[eMaxPt],
								  float pt1[eMaxPt], float pt2[eMaxPt], float *tOut,
								  float nOut[eMaxPt])
{
	gfxBound bound = MDLGetBound(obj->theMdl);

	float trans[eMaxPt]; OBJGetWorldLoc(obj, trans);

	bound.min[eX] += trans[eX];
	bound.min[eY] += trans[eY];
	bound.min[eZ] += trans[eZ];
	
	bound.max[eX] += trans[eX];
	bound.max[eY] += trans[eY];
	bound.max[eZ] += trans[eZ];

	float vec[eMaxPt] = {pt2[eX]-pt1[eX], pt2[eY]-pt1[eY], pt2[eZ]-pt1[eZ]};

	if(mins)
	{bound.min[eX] += mins[eX]; bound.min[eY] += mins[eY]; bound.min[eZ] += mins[eZ];}

	if(maxs)
	{bound.max[eX] += maxs[eX]; bound.max[eY] += maxs[eY]; bound.max[eZ] += maxs[eZ];}

	return GFXIntersectBox(pt1, vec, bound.min, bound.max, tOut, nOut);
}

/////////////////////////////////////
// Name:	OBJFixOrientation
// Purpose:	fixes obj's orientation
// Output:	object rotated to -90
// Return:	none
/////////////////////////////////////
PUBLIC void OBJFixOrientation(hOBJ obj)
{
	if(TESTFLAGS(obj->theMdl->status, MDL_YDEPTH))
		OBJRotX(obj, D3DXToRadian(-90));
}

/////////////////////////////////////
// Name:	OBJIsDeactive
// Purpose:	check to see if given
//			obj is deactive
// Output:	none
// Return:	true if deactive
/////////////////////////////////////
PUBLIC bool OBJIsDeactivate(hOBJ obj)
{
	return TESTFLAGS(obj->status, OBJ_DEACTIVATE);
}

/////////////////////////////////////
// Name:	OBJDeactivate
// Purpose:	activates/deactivates obj
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void OBJActivate(hOBJ obj, bool bActivate)
{
	//remove child from it's refered list
	if(TESTFLAGS(obj->status, OBJ_CHILD))
	{
		OBJDestroy(&obj); //this will just subtract the child ref
		CLEARFLAG(obj->status, OBJ_CHILD);
		CLEARFLAG(obj->status, OBJ_SYNCSTATE);
		//OBJRemoveFromParent(obj);
	}

	if(bActivate)
	{
		if(OBJIsDeactivate(obj))
		{
			//remove object from previous list
			obj->listRef->remove((unsigned int)obj);

			//add object to main object list
			g_OBJLIST.push_back((unsigned int)obj);
			obj->listRef = &g_OBJLIST;

			CLEARFLAG(obj->status, OBJ_DEACTIVATE);
		}
	}
	else
	{
		//remove object from previous list
		obj->listRef->remove((unsigned int)obj);

		//add object to deactivated object list
		g_OBJDEACTIVATELIST.push_back((unsigned int)obj);
		obj->listRef = &g_OBJDEACTIVATELIST;

		SETFLAG(obj->status, OBJ_DEACTIVATE);
	}
}

/////////////////////////////////////
// Name:	OBJDuplicate
// Purpose:	duplicates a given object
// Output:	stuff happens
// Return:	new clone object
/////////////////////////////////////
PUBLIC hOBJ OBJDuplicate(hOBJ obj, unsigned int newID, float x, float y, float z)
{
	hOBJ newObj;

	newObj = OBJCreate(newID, obj->theMdl, x, y, z, eOBJ_STILL, 0);

	if(newObj)
	{
		newObj->curFrame = obj->curFrame;
		newObj->curState = obj->curState;
		
		SETFLAG(newObj->status, obj->status);

		//set orientation
		OBJSetOrientation(newObj, (float*)obj->orient);

		//copy the animation
		newObj->numStates = obj->numStates;

		if(obj->numStates > 0)
		{
			newObj->states = (gfxObjMdlState*)GFX_MALLOC(sizeof(gfxObjMdlState)*obj->numStates);
			if(!newObj->states)
			{ ASSERT_MSG(0, "Unable to allocate new object", "Error in OBJDuplicate"); _destroyObj(&newObj); return 0; }

			memcpy(newObj->states, obj->states, sizeof(gfxObjMdlState)*obj->numStates);

			TimeInit(&newObj->ticker, TimeGetDelay(&newObj->ticker));
		}

		//copy children
		hOBJ thisChildObj, newChildObj;
		
		for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
		{
			thisChildObj = (hOBJ)(*i);
			newChildObj = OBJDuplicate(thisChildObj, thisChildObj->ID, thisChildObj->wrldTrans[eX], thisChildObj->wrldTrans[eY], thisChildObj->wrldTrans[eZ]);

			if(newChildObj)
				OBJAddChild(newObj, newChildObj, thisChildObj->parentJointInd, TESTFLAGS(thisChildObj->status, OBJ_SYNCSTATE));
		}
	}

	return newObj;
}

/////////////////////////////////////
// Name:	OBJSetOrientation
// Purpose:	set the orient of obj
// Output:	orient is done
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetOrientation(hOBJ obj, float vec[eMaxPt])
{
	if(vec[eX] == 0 && vec[eZ] == 0)
		return;

#if 1
	float radians = obj->wrldRot[eY];

	obj->orient.x = vec[eX];
	obj->orient.y = vec[eY];
	obj->orient.z = vec[eZ];

	obj->wrldRot[eY] = vec[eZ] != 0 ? atan ( vec[eX]/vec[eZ] ) : vec[eX] <= 0 ? (DAVIDPI*0.5f) : (DAVIDPI*1.5f);

	if ( vec[eZ] > 0 ) obj->wrldRot[eY] += 3.14f;
	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationY( &mtxR, obj->wrldRot[eY] - radians );
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxRot,&mtxR);

	//set the world matrix
	//D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxTrans);
#else
	obj->orient.x = vec[eX];
	obj->orient.y = vec[eY];
	obj->orient.z = vec[eZ];

	float rot[eMaxPt]={0};

	//if(vec[eX] == 0 && vec[eZ] == 0)
	//	rot[eY]=0;
	//else
	{
		if( obj->orient.x < 0.0f )
			rot[eY] = -atanf(obj->orient.z/obj->orient.x)+(DAVIDPI*0.5f);
		else
		{
			rot[eY] = -atanf(obj->orient.z/obj->orient.x)-(DAVIDPI*0.5f);
		}
	}

	/*if(vec[eY] == 0 && vec[eZ] == 0)
		rot[eX]=0;
	else
	{
		if( obj->orient.y > 0.0f )
			rot[eX] = -atanf(obj->orient.z/obj->orient.y)+DAVIDPI/2;
		else
		{
			rot[eX] = -atanf(obj->orient.z/obj->orient.y)-DAVIDPI/2;
		}
	}*/

	OBJRot(obj, rot, false);
#endif
}

/////////////////////////////////////
// Name:	OBJGetOrientation
// Purpose:	get the orient of obj
// Output:	the vec is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetOrientation(hOBJ obj, float vec[eMaxPt])
{
	memcpy(vec, (float*)obj->orient, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int OBJGetID(hOBJ obj)
{
	return obj->key.ID;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC gfxID OBJGetKey(hOBJ obj)
{
	return obj->key;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJQuery(const gfxID *key)
{
	return (hOBJ)GFXPageQuery(key);
}

/////////////////////////////////////
// Name:	OBJSetTexture
// Purpose:	change obj's
//			texture within material
//			index
// Output:	obj's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE OBJSetTexture(hOBJ obj, int materialInd, hTXT texture)
{
	//allocate textures if there is none
	//set it automatically to one
	if(obj->theMdl->numMaterial > 0)
	{
		if(materialInd >= obj->theMdl->numMaterial)
		{ ASSERT_MSG(0, "Bad material index", "Error in OBJSetTexture"); return RETCODE_FAILURE; }

		TextureDestroy(&obj->textures[materialInd]);

		obj->textures[materialInd] = texture;
		TextureAddRef(obj->textures[materialInd]);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	OBJGetNumMaterial
// Purpose:	get the number of materials
// Output:	none
// Return:	number of materials
/////////////////////////////////////
PUBLIC int OBJGetNumMaterial(hOBJ obj)
{
	return obj->theMdl->numMaterial;
}

/////////////////////////////////////
// Name:	OBJSetMaterial
// Purpose:	change obj's
//			material within material
//			index
// Output:	obj's material change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE OBJSetMaterial(hOBJ obj, int materialInd, const GFXMATERIAL * material)
{
	//allocate materials if there is none
	//set it automatically to one
	if(obj->theMdl->numMaterial > 0)
	{
		if(materialInd >= obj->theMdl->numMaterial)
		{ ASSERT_MSG(0, "Bad material index", "Error in OBJSetMaterial"); return RETCODE_FAILURE; }

		memcpy(&obj->materials[materialInd], material, sizeof(GFXMATERIAL));
	}

	return RETCODE_SUCCESS;
}

PUBLIC void OBJGetMaterial(hOBJ obj, int materialInd, GFXMATERIAL * materialOut)
{
	if(obj->theMdl->numMaterial > 0)
	{
		memcpy(materialOut, &obj->materials[materialInd], sizeof(GFXMATERIAL));
	}
}

/////////////////////////////////////
// Name:	OBJSetAlpha
// Purpose:	set the object alpha channel
//			value ranges from 0~1
// Output:	materials are set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetAlpha(hOBJ obj, float alphaVal)
{
	for(int i = 0; i < obj->theMdl->numMaterial; i++)
	{
		obj->materials[i].Diffuse.a = alphaVal;
	}
}

//Joint related stuff

/////////////////////////////////////
// Name:	OBJJointGetIndex
// Purpose:	get index of given joint
//			name
// Output:	none
// Return:	joint index, -1 if not found
/////////////////////////////////////
PUBLIC int OBJJointGetIndex(hOBJ obj, const char *jointName)
{
	return JointGetIndex(obj->theMdl->joints, obj->theMdl->numJoints, jointName);
}

/////////////////////////////////////
// Name:	JointGetTranslate
// Purpose:	get the joint location
//			(model space)
// Output:	ptOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJJointGetTranslate(hOBJ obj, int ind, float ptOut[eMaxPt])
{
	assert(ind > -1 && ind < obj->theMdl->numJoints);

	JointGetTranslate(obj->joints, ind, ptOut);
}

/////////////////////////////////////
// Name:	OBJJointGetWorldLoc
// Purpose:	get the world location
//			of a joint
// Output:	ptOut
// Return:	none
/////////////////////////////////////
PUBLIC void OBJJointGetWorldLoc(hOBJ obj, int ind, float ptOut[eMaxPt])
{
	assert(ind > -1 && ind < obj->theMdl->numJoints);

	D3DXMATRIX mtx; D3DXMatrixIdentity(&mtx);

	_OBJGetWrldMtx(obj, &mtx);

	D3DXVECTOR3 vtx;

	OBJJointGetTranslate(obj, ind, (float*)vtx);

	D3DXVec3TransformCoord(&vtx, &vtx, &mtx);

	ptOut[eX] = vtx.x; ptOut[eY] = vtx.y; ptOut[eZ] = vtx.z;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJJointTranslate(hOBJ obj, int ind, float trans[eMaxPt], bool bAdd)
{
	assert(ind > -1 && ind < obj->theMdl->numJoints);

	return JointUserTranslate(obj->joints, ind, trans, bAdd);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJJointRotate(hOBJ obj, int ind, float rot[eMaxPt], bool bAdd)
{
	assert(ind > -1 && ind < obj->theMdl->numJoints);

	return JointUserRotate(obj->joints, ind, rot, bAdd);
}

/////////////////////////////////////
// Name:	OBJSetDrawBound
// Purpose:	set the draw bound of the
//			given object
// Output:	object draw bound set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetDrawBound(hOBJ obj, float mins[eMaxPt], float maxs[eMaxPt])
{
	memcpy(obj->drawBoundMin, mins, sizeof(mins));
	memcpy(obj->drawBoundMax, maxs, sizeof(maxs));

	obj->drawBoundRadius = MAX(maxs[eX], maxs[eY]);
	obj->drawBoundRadius = MAX(obj->drawBoundRadius, maxs[eZ]);
}

/////////////////////////////////////
// Name:	OBJSetToLast
// Purpose:	set the given object to
//			be the last item in the
//			object render list
// Output:	object list updated
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetToLast(hOBJ obj)
{
	if(obj->listRef)
	{
		obj->listRef->remove((unsigned int)obj);

		//add object to main object list
		obj->listRef->push_back((unsigned int)obj);
	}
}

/////////////////////////////////////
// Name:	OBJSetAnimScale
// Purpose:	set the animation time
//			scale. (usu.: 1)
// Output:	sets the speed of animatin
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetAnimScale(hOBJ obj, double s)
{
	obj->animeScale = s;
}