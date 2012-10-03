#include "gdix.h"
#include "gdix_i.h"
#include "gdiX_joints_i.h"

//quaternion rotation
PROTECTED void JointQuaternionRotation(D3DXQUATERNION *pQuat, 
									  const float rot[eMaxPt])
{
	float angle;
	double sr, sp, sy, cr, cp, cy;

	angle = rot[eZ]*0.5f;
	sy = sin( angle );
	cy = cos( angle );
	angle = rot[eY]*0.5f;
	sp = sin( angle );
	cp = cos( angle );
	angle = rot[eX]*0.5f;
	sr = sin( angle );
	cr = cos( angle );

	double crcp = cr*cp;
	double srsp = sr*sp;

	pQuat->x = ( float )( sr*cp*cy-cr*sp*sy );
	pQuat->y = ( float )( cr*sp*cy+sr*cp*sy );
	pQuat->z = ( float )( crcp*sy-srsp*cy );
	pQuat->w = ( float )( crcp*cy+srsp*sy );
}

//matrix transformation
PRIVATE void _JointMtxTransformation(D3DXMATRIX *m, 
									 const float rot[eMaxPt], 
									 const float trans[eMaxPt])
{
	double cr = cos( rot[eX] );
	double sr = sin( rot[eX] );
	double cp = cos( rot[eY] );
	double sp = sin( rot[eY] );
	double cy = cos( rot[eZ] );
	double sy = sin( rot[eZ] );

	m->_11 = ( float )( cp*cy );
	m->_12 = ( float )( cp*sy );
	m->_13 = ( float )( -sp );
	m->_14 = 0;

	double srsp = sr*sp;
	double crsp = cr*sp;

	m->_21 = ( float )( srsp*cy-cr*sy );
	m->_22 = ( float )( srsp*sy+cr*cy );
	m->_23 = ( float )( sr*cp );
	m->_24 = 0;

	m->_31 = ( float )( crsp*cy+sr*sy );
	m->_32 = ( float )( crsp*sy-sr*cy );
	m->_33 = ( float )( cr*cp );
	m->_34 = 0;

	m->_41 = trans[eX];
	m->_42 = trans[eY];
	m->_43 = trans[eZ];
	m->_44 = 1.0f;
}

//matrix transformation with quaternion
PRIVATE void _JointMtxTransformationQuat(D3DXMATRIX *m, 
									 const D3DXQUATERNION *quat, 
									 const float trans[eMaxPt])
{
	m->_11 = ( float )( 1.0 - 2.0*(quat->y*quat->y + quat->z*quat->z) );
	m->_12 = ( float )( 2.0*(quat->x*quat->y + quat->w*quat->z) );
	m->_13 = ( float )( 2.0*(quat->x*quat->z - quat->w*quat->y) );
	m->_14 = 0;

	m->_21 = ( float )( 2.0*(quat->x*quat->y - quat->w*quat->z) );
	m->_22 = ( float )( 1.0 - 2.0*(quat->x*quat->x + quat->z*quat->z) );
	m->_23 = ( float )( 2.0*(quat->y*quat->z + quat->w*quat->x) );
	m->_24 = 0;

	m->_31 = ( float )( 2.0*(quat->x*quat->z + quat->w*quat->y) );
	m->_32 = ( float )( 2.0*(quat->y*quat->z - quat->w*quat->x) );
	m->_33 = ( float )( 1.0 - 2.0*(quat->x*quat->x + quat->y*quat->y) );
	m->_34 = 0;

	m->_41 = trans[eX];
	m->_42 = trans[eY];
	m->_43 = trans[eZ];
	m->_44 = 1.0f;
}

/////////////////////////////////////
// Name:	JointDestroy
// Purpose:	destroy joint data
// Output:	joint is clean
// Return:	none
/////////////////////////////////////
PROTECTED void JointDestroy(gfxModelJoint *pJoint)
{
	if(pJoint)
	{
		if(pJoint->jointFrames)
			GFX_FREE(pJoint->jointFrames);

		memset(pJoint, 0, sizeof(gfxModelJoint));
	}
}

/////////////////////////////////////
// Name:	JointInit
// Purpose:	initialize given joint
// Output:	joint initialized
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE JointInit(gfxModelJoint *pJoint, const char name[NAMEMAX2],
						 int parent, float rotate[eMaxPt], float trans[eMaxPt],
						 int numFrames)
{
	strcpy(pJoint->name, name);

	pJoint->parent = parent;

	memcpy(pJoint->localRotate, rotate, sizeof(float)*eMaxPt);
	memcpy(pJoint->localTrans, trans, sizeof(float)*eMaxPt);

	if(numFrames > 0)
	{
		pJoint->jointFrames = (jointFrame*)GFX_MALLOC(sizeof(jointFrame)*numFrames);
		
		if(!pJoint->jointFrames)
		{ ASSERT_MSG(0, "Unable to allocate joint frames", "JointInit"); return RETCODE_FAILURE; }
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	JointSetMatrices
// Purpose:	set the matrices of the
//			joint array
//			NOTE: call JointCreate for
//			all joints first!
// Output:	joint matrices are set
// Return:	none
/////////////////////////////////////
PROTECTED void JointSetMatrices(gfxModelJoint *pJoints, int numJoints)
{
	//NOTE: assume that parents are in correct order within the array
	//      ie. in heirarchy order
	for(int i = 0; i < numJoints; i++)
	{
		//set relative
		_JointMtxTransformation(&pJoints[i].relative, pJoints[i].localRotate, 
			pJoints[i].localTrans);
   
		//set absolute
		if(pJoints[i].parent != -1)
			pJoints[i].absolute = pJoints[i].relative*pJoints[pJoints[i].parent].absolute;
		else
			pJoints[i].absolute = pJoints[i].relative;
	}
}

/////////////////////////////////////
// Name:	JointTransVertices
// Purpose:	translate vertices correctly
//			to the joints
//			NOTE: this should only be called
//				  once during initialization
// Output:	vertices are set
// Return:	none
/////////////////////////////////////
PROTECTED void JointTransVertices(const gfxModelJoint *pJoints, const char *boneIDs,
								  gfxVtx *pVtx, int numVtx)
{
	D3DXMATRIX invAbs;
	float vec[eMaxPt];

	//translate these vertices to inverse absolute
	for(int i = 0; i < numVtx; i++)
	{
		if(boneIDs[i] != -1)
		{
			D3DXMatrixInverse(&invAbs, 0, &pJoints[boneIDs[i]].absolute);

			//do point
			vec[eX] = pVtx[i].x*invAbs._11+pVtx[i].y*invAbs._21+pVtx[i].z*invAbs._31+invAbs._41;
			vec[eY] = pVtx[i].x*invAbs._12+pVtx[i].y*invAbs._22+pVtx[i].z*invAbs._32+invAbs._42;
			vec[eZ] = pVtx[i].x*invAbs._13+pVtx[i].y*invAbs._23+pVtx[i].z*invAbs._33+invAbs._43;

			pVtx[i].x = vec[eX]; pVtx[i].y = vec[eY]; pVtx[i].z = vec[eZ];

			//do normal
			vec[eX] = pVtx[i].nX*invAbs._11+pVtx[i].nY*invAbs._21+pVtx[i].nZ*invAbs._31;
			vec[eY] = pVtx[i].nX*invAbs._12+pVtx[i].nY*invAbs._22+pVtx[i].nZ*invAbs._32;
			vec[eZ] = pVtx[i].nX*invAbs._13+pVtx[i].nY*invAbs._23+pVtx[i].nZ*invAbs._33;

			pVtx[i].nX = vec[eX]; pVtx[i].nY = vec[eY]; pVtx[i].nZ = vec[eZ];
		}
	}
}

/////////////////////////////////////
// Name:	JointSetObj
// Purpose:	set obj joint
// Output:	pObjJoint is set
// Return:	none
/////////////////////////////////////
PROTECTED void JointSetObj(gfxObjJoint *pObjJoint,
						   const gfxModelJoint *pMdlJoint)
{
	pObjJoint->final = pMdlJoint->absolute;
	D3DXMatrixIdentity(&pObjJoint->userMtx);
}

/////////////////////////////////////
// Name:	JointInterpolate
// Purpose:	interpolate within prev
//			to cur frame
//			pObjParentJoints != 0,
//			then all -1 is referred to
//			parentJointInd
// Output:	final matrices for each
//			joint is updated
// Return:	none
/////////////////////////////////////
PROTECTED void JointInterpolate(gfxObjJoint *pObjJoints,
								const gfxModelJoint *pMdlJoints,
								int numJoint, float t,
								int numFrame, int prevFrame, int curFrame,
								gfxObjJoint *pObjParentJoints,
								int parentJointInd)
{
	assert(prevFrame < numFrame);
	assert(curFrame < numFrame);

	float curVtx[eMaxPt];
	D3DXQUATERNION	curQuat;

	D3DXMATRIX transMtx, newRelative;
	jointFrame	*prevJFrame, *curJFrame;

	//go through each joint and update them
	for(int i = 0; i < numJoint; i++)
	{
		//if there are no frames, just continue
		if(numFrame == 0)
		{ pObjJoints[i].final = pMdlJoints[i].absolute; continue; }

		prevJFrame = &pMdlJoints[i].jointFrames[prevFrame];
		curJFrame = &pMdlJoints[i].jointFrames[curFrame];

		//////////////////////////////////////////////
		//do the translation
		if(prevFrame == curFrame)
		{
			curVtx[eX] = pMdlJoints[i].jointFrames[curFrame].trans[eX];
			curVtx[eY] = pMdlJoints[i].jointFrames[curFrame].trans[eY];
			curVtx[eZ] = pMdlJoints[i].jointFrames[curFrame].trans[eZ];
		}
		else
		{
			//interpolate between
			curVtx[eX] = prevJFrame->trans[eX] 
				+ ((curJFrame->trans[eX]-prevJFrame->trans[eX])*t);

			curVtx[eY] = prevJFrame->trans[eY] 
				+ ((curJFrame->trans[eY]-prevJFrame->trans[eY])*t);

			curVtx[eZ] = prevJFrame->trans[eZ] 
				+ ((curJFrame->trans[eZ]-prevJFrame->trans[eZ])*t);
		}
		//////////////////////////////////////////////

		//////////////////////////////////////////////
		//do the rotation
		if(prevFrame == curFrame)
			curQuat = pMdlJoints[i].jointFrames[curFrame].quat;
		else
		{
			//interpolate between
			D3DXQuaternionSlerp(&curQuat, &prevJFrame->quat, &curJFrame->quat, t);
		}
		//////////////////////////////////////////////

		//create the transformation
		_JointMtxTransformationQuat(&transMtx, &curQuat, curVtx);

		//create a new relative
		newRelative = transMtx*pMdlJoints[i].relative;

		if(pMdlJoints[i].parent != -1)
			pObjJoints[i].final = newRelative*pObjJoints[pMdlJoints[i].parent].final;
		else
		{
			if(pObjParentJoints && parentJointInd != -1)
				pObjJoints[i].final = newRelative*pObjParentJoints[parentJointInd].final;
			else
				pObjJoints[i].final = newRelative;
		}

		//user transformation
		pObjJoints[i].final = pObjJoints[i].userMtx*pObjJoints[i].final;
	}
}

/////////////////////////////////////
// Name:	JointGetIndex
// Purpose:	get index of given joint
//			name
// Output:	none
// Return:	joint index, -1 if not found
/////////////////////////////////////
PROTECTED int JointGetIndex(const gfxModelJoint *pMdlJoints, int numJoint,
							const char *jointName)
{
	//go through each joint and find the jointName
	for(int i = 0; i < numJoint; i++)
	{
		//found it!
		if(strcmp(pMdlJoints[i].name, jointName) == 0)
			return i;
	}

	return -1; //not found
}

/////////////////////////////////////
// Name:	JointGetTranslate
// Purpose:	get the joint location
//			(model space)
// Output:	ptOut filled
// Return:	none
/////////////////////////////////////
PROTECTED void JointGetTranslate(gfxObjJoint *pObjJoints, int ind,
								float ptOut[eMaxPt])
{
	ptOut[eX] = pObjJoints[ind].final._41;
	ptOut[eY] = pObjJoints[ind].final._42;
	ptOut[eZ] = pObjJoints[ind].final._43;
}

/////////////////////////////////////
// Name:	JointUserTranslate
// Purpose:	set obj user joint translate
// Output:	user stuff in obj joint is set
// Return:	success if joint name found and translated
/////////////////////////////////////
PROTECTED RETCODE JointUserTranslate(gfxObjJoint *pObjJoints, int ind,
								float trans[eMaxPt], bool bAdd)
{
	if(bAdd)
	{
		pObjJoints[ind].userTrans[eX] += trans[eX];
		pObjJoints[ind].userTrans[eY] += trans[eY];
		pObjJoints[ind].userTrans[eZ] += trans[eZ];
	}
	else
	{
		pObjJoints[ind].userTrans[eX] = trans[eX];
		pObjJoints[ind].userTrans[eY] = trans[eY];
		pObjJoints[ind].userTrans[eZ] = trans[eZ];
	}

	pObjJoints[ind].userMtx._41 = pObjJoints[ind].userTrans[eX];
	pObjJoints[ind].userMtx._42 = pObjJoints[ind].userTrans[eY];
	pObjJoints[ind].userMtx._43 = pObjJoints[ind].userTrans[eZ];

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	JointUserRotate
// Purpose:	set obj user joint rotate
// Output:	user stuff in obj joint is set
// Return:	success if joint name found and translated
/////////////////////////////////////
PROTECTED RETCODE JointUserRotate(gfxObjJoint *pObjJoints, int ind,
								float rot[eMaxPt], bool bAdd)
{
	if(bAdd)
	{
		pObjJoints[ind].userRot[eX] += rot[eX];
		pObjJoints[ind].userRot[eY] += rot[eY];
		pObjJoints[ind].userRot[eZ] += rot[eZ];
	}
	else
	{
		pObjJoints[ind].userRot[eX] = rot[eX];
		pObjJoints[ind].userRot[eY] = rot[eY];
		pObjJoints[ind].userRot[eZ] = rot[eZ];
	}

	D3DXMatrixRotationYawPitchRoll(&pObjJoints[ind].userMtx,
		pObjJoints[ind].userRot[eY],
		pObjJoints[ind].userRot[eX],
		pObjJoints[ind].userRot[eZ]); 

	pObjJoints[ind].userMtx._41 = pObjJoints[ind].userTrans[eX];
	pObjJoints[ind].userMtx._42 = pObjJoints[ind].userTrans[eY];
	pObjJoints[ind].userMtx._43 = pObjJoints[ind].userTrans[eZ];

	return RETCODE_SUCCESS;
}