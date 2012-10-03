#ifndef _gdix_joints_i_h
#define _gdix_joints_i_h

//THIS IS FOR INTERNAL GFX USE ONLY!

//quaternion rotation
PROTECTED void JointQuaternionRotation(D3DXQUATERNION *pQuat, 
									  const float rot[eMaxPt]);

/////////////////////////////////////
// Name:	JointInit
// Purpose:	initialize given joint
// Output:	joint initialized
// Return:	success if successful
/////////////////////////////////////
PROTECTED RETCODE JointInit(gfxModelJoint *pJoint, const char name[NAMEMAX2],
						 int parent, float rotate[eMaxPt], float trans[eMaxPt],
						 int numFrames);

/////////////////////////////////////
// Name:	JointDestroy
// Purpose:	destroy joint data
// Output:	joint is clean
// Return:	none
/////////////////////////////////////
PROTECTED void JointDestroy(gfxModelJoint *pJoint);

/////////////////////////////////////
// Name:	JointSetMatrices
// Purpose:	set the matrices of the
//			joint array
//			NOTE: call JointCreate for
//			all joints first!
// Output:	joint matrices are set
// Return:	none
/////////////////////////////////////
PROTECTED void JointSetMatrices(gfxModelJoint *pJoints, int numJoints);

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
								  gfxVtx *pVtx, int numVtx);

/////////////////////////////////////
// Name:	JointSetObj
// Purpose:	set obj joint
// Output:	pObjJoint is set
// Return:	none
/////////////////////////////////////
PROTECTED void JointSetObj(gfxObjJoint *pObjJoint,
						   const gfxModelJoint *pMdlJoint);

/////////////////////////////////////
// Name:	MeshTranslateVtxJoint
// Purpose:	do the final translation
//			if joint to vertices
// Output:	vtx buffer joint is filled
// Return:	success if success
/////////////////////////////////////
PROTECTED RETCODE MeshTranslateVtxJoint(gfxMesh *mesh, gfxObjJoint *joints);

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
								gfxObjJoint *pObjParentJoints=0,
								int parentJointInd=-1);

/////////////////////////////////////
// Name:	JointGetIndex
// Purpose:	get index of given joint
//			name
// Output:	none
// Return:	joint index, -1 if not found
/////////////////////////////////////
PROTECTED int JointGetIndex(const gfxModelJoint *pMdlJoints, int numJoint,
							const char *jointName);

/////////////////////////////////////
// Name:	JointGetTranslate
// Purpose:	get the joint location
//			(model space)
// Output:	ptOut filled
// Return:	none
/////////////////////////////////////
PROTECTED void JointGetTranslate(gfxObjJoint *pObjJoints, int ind,
								float ptOut[eMaxPt]);

/////////////////////////////////////
// Name:	JointUserTranslate
// Purpose:	set obj user joint translate
// Output:	user stuff in obj joint is set
// Return:	success if joint name found and translated
/////////////////////////////////////
PROTECTED RETCODE JointUserTranslate(gfxObjJoint *pObjJoints, int ind,
								float trans[eMaxPt], bool bAdd);

/////////////////////////////////////
// Name:	JointUserRotate
// Purpose:	set obj user joint rotate
// Output:	user stuff in obj joint is set
// Return:	success if joint name found and translated
/////////////////////////////////////
PROTECTED RETCODE JointUserRotate(gfxObjJoint *pObjJoints, int ind,
								float rot[eMaxPt], bool bAdd);

#endif