#include "gdix.h"
#include "gdix_i.h"
#include "gdiX_mesh_i.h"
#include "gdiX_joints_i.h"

//Milkshape 3D
//
// max values
//
#define MAX_VERTICES    8192
#define MAX_TRIANGLES   16384
#define MAX_GROUPS      128
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed



//
// flags
//
#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8



//
// types
//
#ifndef byte
typedef unsigned char byte;
#endif // byte

#ifndef word
typedef unsigned short word;
#endif // word

//
// First comes the header.
//
typedef struct
{
    char    id[10];                                     // always "MS3D000000"
    int     version;                                    // 3
} ms3d_header_t;

//
// Then comes the number of vertices
//
//word nNumVertices;

//
// Then comes nNumVertices * sizeof (ms3d_vertex_t)
//
typedef struct
{
    byte    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    float   vertex[3];                                  //
    char    boneId;                                     // -1 = no bone
    byte    referenceCount;
} ms3d_vertex_t;

//
// number of triangles
//
//word nNumTriangles;

//
// nNumTriangles * sizeof (ms3d_triangle_t)
//
typedef struct
{
    word    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    vertexIndices[3];                           //
    float   vertexNormals[3][3];                        //
    float   s[3];                                       //
    float   t[3];                                       //
    byte    smoothingGroup;                             // 1 - 32
    byte    groupIndex;                                 //
} ms3d_triangle_t;

//
// number of groups
//
//word nNumGroups;

//
// nNumGroups * sizeof (ms3d_group_t)
//
typedef struct
{
    byte            flags;                              // SELECTED | HIDDEN
    char            name[32];                           //
    word            numtriangles;                       //
    word            *triangleIndices;					// the groups group the triangles[numtriangles]
    char            materialIndex;                      // -1 = no material
} ms3d_group_t;

//
// number of materials
//
//word nNumMaterials;

//
// nNumMaterials * sizeof (ms3d_material_t)
//
typedef struct
{
    char            name[32];                           //
    float           ambient[4];                         //
    float           diffuse[4];                         //
    float           specular[4];                        //
    float           emissive[4];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[256];                        // texture.bmp
    char            alphamap[256];                       // alpha.bmp
} ms3d_material_t;

//
// save some keyframer data
//
//float fAnimationFPS;
//float fCurrentTime;
//int iTotalFrames;

//
// number of joints
//
//word nNumJoints;

//
// nNumJoints * sizeof (ms3d_joint_t)
//
//
typedef struct
{
    float           time;                               // time in seconds
    float           rotation[3];                        // x, y, z angles
} ms3d_keyframe_rot_t;

typedef struct
{
    float           time;                               // time in seconds
    float           position[3];                        // local position
} ms3d_keyframe_pos_t;

typedef struct
{
	byte            flags;                              // SELECTED | DIRTY
    char            name[32];                           //
    char            parentName[32];                     //
    float           rotation[3];                        // local reference matrix
    float           position[3];

    word            numKeyFramesRot;                    //
    word            numKeyFramesTrans;                  //

    ms3d_keyframe_rot_t *keyFramesRot;      // local animation matrices[numKeyFramesRot]
    ms3d_keyframe_pos_t *keyFramesTrans;  // local animation matrices[numKeyFramesTrans]
} ms3d_joint_t;

//
// Mesh Transformation:
// 
// 0. Build the transformation matrices from the rotation and position
// 1. Multiply the vertices by the inverse of local reference matrix (lmatrix0)
// 2. then translate the result by (lmatrix0 * keyFramesTrans)
// 3. then multiply the result by (lmatrix0 * keyFramesRot)
//
// For normals skip step 2.
//
//
//
// NOTE:  this file format may change in future versions!
//
//
// - Mete Ciragan
//

PRIVATE void _MS3DReadVtx(FILE *fp, word nNumVertices, ms3d_vertex_t *ms3dVtx)
{
	for(int i = 0; i < nNumVertices; i++)
	{
		fread(&ms3dVtx[i].flags, sizeof(byte), 1, fp);
		fread(ms3dVtx[i].vertex, sizeof(float), 3, fp);
		fread(&ms3dVtx[i].boneId, sizeof(char), 1, fp);
		fread(&ms3dVtx[i].referenceCount, sizeof(char), 1, fp);
	}
}

PRIVATE void _MS3DReadTriangle(FILE *fp, word nNumTriangles, ms3d_triangle_t *ms3dTri)
{
	for(int i = 0; i < nNumTriangles; i++)
	{
		fread(&ms3dTri[i].flags, sizeof(word), 1, fp);
		fread(ms3dTri[i].vertexIndices, sizeof(word), 3, fp);
		fread(ms3dTri[i].vertexNormals, sizeof(float), 9, fp);
		fread(ms3dTri[i].s, sizeof(float), 3, fp);
		fread(ms3dTri[i].t, sizeof(float), 3, fp);
		fread(&ms3dTri[i].smoothingGroup, sizeof(byte), 1, fp);
		fread(&ms3dTri[i].groupIndex, sizeof(byte), 1, fp);
	}
}

PRIVATE RETCODE _MS3DReadGroups(FILE *fp, word nNumGroups, ms3d_group_t *ms3dGrp)
{
	for(int i = 0; i < nNumGroups; i++)
	{
		//read flags
		fread(&ms3dGrp[i].flags, sizeof(byte), 1, fp);

		//read name
		fread(ms3dGrp[i].name, sizeof(char), 32, fp);

		//read number of triangles
		fread(&ms3dGrp[i].numtriangles, sizeof(word), 1, fp);

		//allocate triangle indices
		ms3dGrp[i].triangleIndices = (word*)GFX_MALLOC(sizeof(word)*ms3dGrp[i].numtriangles);
		if(!ms3dGrp[i].triangleIndices)
		{ ASSERT_MSG(0, "Unable to allocate triangle indices", "_MS3DReadGroups"); return RETCODE_FAILURE; }

		//read triangle indices
		fread(ms3dGrp[i].triangleIndices, sizeof(word), ms3dGrp[i].numtriangles, fp);

		//read material index
		fread(&ms3dGrp[i].materialIndex, sizeof(char), 1, fp);
	}

	return RETCODE_SUCCESS;
}

PRIVATE void _MS3DReadMaterials(const char *mdlFilepath, FILE *fp, word nNumMaterials, 
								ms3d_material_t *ms3dMaterials)
{
	char path[128];

	for(int i = 0; i < nNumMaterials; i++)
	{
		fread(ms3dMaterials[i].name, sizeof(char), 32, fp);
		fread(ms3dMaterials[i].ambient, sizeof(float), 4, fp);
		fread(ms3dMaterials[i].diffuse, sizeof(float), 4, fp);
		fread(ms3dMaterials[i].specular, sizeof(float), 4, fp);
		fread(ms3dMaterials[i].emissive, sizeof(float), 4, fp);
		fread(&ms3dMaterials[i].shininess, sizeof(float), 1, fp);
		fread(&ms3dMaterials[i].transparency, sizeof(float), 1, fp);

		fread(&ms3dMaterials[i].mode, sizeof(char), 1, fp);

		fread(path, sizeof(char), 128, fp);

		ConcatinateFilePath(mdlFilepath, path,
								ms3dMaterials[i].texture);

		fread(path, sizeof(char), 128, fp);

		ConcatinateFilePath(mdlFilepath, path,
								ms3dMaterials[i].alphamap);
	}
}

PRIVATE RETCODE _MS3DReadJoints(FILE *fp, word nNumJoints, ms3d_joint_t *ms3dJoints,
								float fTotalFrame, float fAnimationFPS)
{
	//grab joints
	for(int i = 0; i < nNumJoints; i++)
	{
		//grab basic data
		fread(&ms3dJoints[i].flags, sizeof(byte), 1, fp);
		fread(ms3dJoints[i].name, sizeof(char), 32, fp);
		fread(ms3dJoints[i].parentName, sizeof(char), 32, fp);

		fread(ms3dJoints[i].rotation, sizeof(float), 3, fp);
		fread(ms3dJoints[i].position, sizeof(float), 3, fp);

		fread(&ms3dJoints[i].numKeyFramesRot, sizeof(word), 1, fp);
		fread(&ms3dJoints[i].numKeyFramesTrans, sizeof(word), 1, fp);
		

		//allocate keyFramesRot
		if(ms3dJoints[i].numKeyFramesRot > 0)
		{
			ms3dJoints[i].keyFramesRot = (ms3d_keyframe_rot_t*)GFX_MALLOC(sizeof(ms3d_keyframe_rot_t)*ms3dJoints[i].numKeyFramesRot);
			if(!ms3dJoints[i].keyFramesRot)
			{ ASSERT_MSG(0, "Unable to allocate key frame rot", "_MS3DReadJoints"); return RETCODE_FAILURE; }
		}

		//allocate keyFramesTrans
		if(ms3dJoints[i].numKeyFramesTrans > 0)
		{
			ms3dJoints[i].keyFramesTrans = (ms3d_keyframe_pos_t*)GFX_MALLOC(sizeof(ms3d_keyframe_pos_t)*ms3dJoints[i].numKeyFramesTrans);
			if(!ms3dJoints[i].keyFramesTrans)
			{ ASSERT_MSG(0, "Unable to allocate key frame trans", "_MS3DReadJoints"); return RETCODE_FAILURE; }
		}

		//grab key frame rot and trans
		int j;

		//we need to convert it to frame time (0~totalframe), ex: 0~30
		float t;
		float tMax = fTotalFrame/fAnimationFPS;

		for(j = 0; j < ms3dJoints[i].numKeyFramesRot; j++)
		{
			fread(&t, sizeof(float), 1, fp);

			ms3dJoints[i].keyFramesRot[j].time = (t/tMax)*fTotalFrame;

			fread(ms3dJoints[i].keyFramesRot[j].rotation, sizeof(float), 3, fp);
		}

		for(j = 0; j < ms3dJoints[i].numKeyFramesTrans; j++)
		{
			fread(&t, sizeof(float), 1, fp);

			ms3dJoints[i].keyFramesTrans[j].time = (t/tMax)*fTotalFrame;

			fread(ms3dJoints[i].keyFramesTrans[j].position, sizeof(float), 3, fp);
		}
	}

	return RETCODE_SUCCESS;
}

//returns index in ms3dJoints from given name, otherwise, -1
PRIVATE int __MS3DGetJointIndex(ms3d_joint_t *ms3dJoints, word nNumJoints,
								const char *name, int nameInd)
{
	if(!name || name[0] == 0)
		return -1;

	for(int i = 0; i < nNumJoints; i++)
	{
		if(nameInd != i)
		{
			if(stricmp(ms3dJoints[i].name, name) == 0)
				return i;
		}
	}

	//not found
	return -1;
}

PRIVATE int __MS3DGetTransFrameInd(const ms3d_joint_t *ms3dJoint, float tFrame)
{
	for(int frame = 0; frame < ms3dJoint->numKeyFramesTrans; frame++)
	{
		if(tFrame < ms3dJoint->keyFramesTrans[frame].time)
			return frame;
	}

	return 0;
}

PRIVATE int __MS3DGetRotFrameInd(const ms3d_joint_t *ms3dJoint, float tFrame)
{
	for(int frame = 0; frame < ms3dJoint->numKeyFramesRot; frame++)
	{
		if(tFrame < ms3dJoint->keyFramesRot[frame].time)
			return frame;
	}

	return 0;
}

PRIVATE void __JointCalculateFrameTransformation(jointFrame *jFrame,
												 const ms3d_joint_t *ms3dJoint,
												 int transFrame, int rotFrame,
												 float tFrame)
{
	int frame;
	float t;

	ms3d_keyframe_rot_t *prevFrameRot, *curFrameRot;
    ms3d_keyframe_pos_t *prevFrameTrans, *curFrameTrans;

	//if there are no key frames, do nothing!
	if(ms3dJoint->numKeyFramesTrans == 0 && ms3dJoint->numKeyFramesRot == 0)
		return;

	//////////////////////////////////////////////
	//do translation
	
	//get to the correct frame
	frame = transFrame;

	/*while ( frame < ms3dJoint->numKeyFramesTrans &&
		ms3dJoint->keyFramesTrans[frame].time < tFrame)
	{ frame++; }*/

	if(frame == 0)
		memcpy(jFrame->trans, ms3dJoint->keyFramesTrans[0].position, sizeof(float)*eMaxPt);
	else if(frame == ms3dJoint->numKeyFramesTrans)
		memcpy(jFrame->trans, ms3dJoint->keyFramesTrans[frame-1].position, sizeof(float)*eMaxPt);
	else
	{
		assert(frame > 0 && frame < ms3dJoint->numKeyFramesTrans);

		prevFrameTrans = &ms3dJoint->keyFramesTrans[frame-1];
		curFrameTrans = &ms3dJoint->keyFramesTrans[frame];

		t = (tFrame-prevFrameTrans->time)/(curFrameTrans->time-prevFrameTrans->time);

		//interpolate between
		jFrame->trans[eX] = prevFrameTrans->position[eX] 
			+ ((curFrameTrans->position[eX]-prevFrameTrans->position[eX])*t);

		jFrame->trans[eY] = prevFrameTrans->position[eY] 
			+ ((curFrameTrans->position[eY]-prevFrameTrans->position[eY])*t);

		jFrame->trans[eZ] = prevFrameTrans->position[eZ] 
			+ ((curFrameTrans->position[eZ]-prevFrameTrans->position[eZ])*t);
	}
	//////////////////////////////////////////////

	//////////////////////////////////////////////
	//do rotation

	//get to the correct frame
	frame = rotFrame;

	/*while ( frame < ms3dJoint->numKeyFramesRot &&
		ms3dJoint->keyFramesRot[frame].time < tFrame)
	{ frame++; }*/

	if(frame == 0)
		JointQuaternionRotation(&jFrame->quat, ms3dJoint->keyFramesRot[0].rotation);
	else if(frame == ms3dJoint->numKeyFramesTrans)
		JointQuaternionRotation(&jFrame->quat, ms3dJoint->keyFramesRot[frame-1].rotation);
	else
	{
		assert(frame > 0 && frame < ms3dJoint->numKeyFramesRot);

		prevFrameRot = &ms3dJoint->keyFramesRot[frame-1];
		curFrameRot = &ms3dJoint->keyFramesRot[frame];

		t = (tFrame-prevFrameRot->time)/(curFrameRot->time-prevFrameRot->time);

		D3DXQUATERNION prevQ, curQ;
		
		JointQuaternionRotation(&prevQ, prevFrameRot->rotation);
		JointQuaternionRotation(&curQ, curFrameRot->rotation);

		//interpolate between
		D3DXQuaternionSlerp(&jFrame->quat, &prevQ, &curQ, t);
	}
}

PRIVATE void __JointSetFrames(gfxModelJoint *joints, float fAnimationFPS, 
							  int iTotalFrames, word nNumJoints, 
							  ms3d_joint_t *ms3dJoints)
{
	//go through each joints
	for(int i = 0; i < nNumJoints; i++)
	{
		//set each frame of the joint
		int transFrame, rotFrame;

		for(int j = 0; j < iTotalFrames; j++)
		{
			transFrame = __MS3DGetTransFrameInd(&ms3dJoints[i], (float)j);
			rotFrame   = __MS3DGetRotFrameInd(&ms3dJoints[i], (float)j);

			//calculate the transformation data for this frame
			__JointCalculateFrameTransformation(&joints[i].jointFrames[j],
												 &ms3dJoints[i],
												 transFrame, rotFrame,
												 (float)j);
		}
	}
}

PRIVATE RETCODE __MS3DSetJoints(hMDL model, float fAnimationFPS, int iTotalFrames,
								word nNumJoints, ms3d_joint_t *ms3dJoints)
{
	SETFLAG(model->status, MDL_JOINT);

	//get num frame joints
	model->numFrames = iTotalFrames;

/*********************************JOINT***********************************/
	model->numJoints = nNumJoints;

	//allocate the joints
	model->joints = (gfxModelJoint*)GFX_MALLOC(sizeof(gfxModelJoint)*nNumJoints);
	if(!model->joints)
	{ ASSERT_MSG(0, "Unable to allocate joints", "__MS3DSetJoints"); return RETCODE_FAILURE; }

	//go through and initialize each joints
	for(int i = 0; i < nNumJoints; i++)
	{
		int parentInd = __MS3DGetJointIndex(ms3dJoints, nNumJoints,
								ms3dJoints[i].parentName, i);

		//initialize this joint
		if(JointInit(&model->joints[i], ms3dJoints[i].name,
						 parentInd, ms3dJoints[i].rotation, ms3dJoints[i].position,
						 iTotalFrames)
						 != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Error creating joint", "__MS3DSetJoints"); return RETCODE_FAILURE; }
	}

	//now set their matrices
	JointSetMatrices(model->joints, nNumJoints);

/****************************JOINT FRAMES*********************************/
	__JointSetFrames(model->joints, fAnimationFPS, iTotalFrames, nNumJoints, ms3dJoints);


	//last but not least, set the vertices
	gfxVtx *ptrVtx;

	if(MeshLockVtx(&model->mesh, 0, &ptrVtx) == RETCODE_SUCCESS)
	{
		char *boneIDs = MeshGetBoneIDs(&model->mesh);
		JointTransVertices(model->joints, boneIDs,
								  ptrVtx, MeshGetNumVtx(&model->mesh));

		MeshUnlockVtx(&model->mesh);
	}

	//duplicate the vertices to system
	MeshCopyVtxToVtxSys(&model->mesh);

	//finally...

	return RETCODE_SUCCESS;
}

struct VtxDat {
	ms3d_vertex_t ms3dVtx;
	float		  s;
	float		  t;
};

typedef vector< VtxDat >  VtxArray;

PRIVATE bool _MS3DCompareVtx(ms3d_vertex_t *v1, ms3d_vertex_t *v2)
{
	if(v1->vertex[eX] == v2->vertex[eX] 
		&& v1->vertex[eY] == v2->vertex[eY]
		&& v1->vertex[eZ] == v2->vertex[eZ]
		&& v1->boneId == v2->boneId)
		return true;

	return false;
}

//returns non negative index if there is a match found
PRIVATE int _MS3DMatchPtTxt(VtxArray &vtx,
							ms3d_vertex_t *ms3dVtx,
							float s, float t,
							int ind)
{
	for(int i = 0; i < vtx.size(); i++)
	{
		if(vtx[i].s == s && vtx[i].t == t
			&& _MS3DCompareVtx(&vtx[i].ms3dVtx, &ms3dVtx[ind]))
			return i;
	}

	return -1;
}

PRIVATE void _MS3DAssembleVertices(VtxArray &vtxOut, word nNumVertices, ms3d_vertex_t *ms3dVtx,
							  word nNumTriangles, ms3d_triangle_t *ms3dTri)
{
	int i, vtxInd;

	//now to fill up the vtxIndData and add stuff to vtxOut if there are
	//any different texture coordinate
	for(i = 0; i < nNumTriangles; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			vtxInd = _MS3DMatchPtTxt(vtxOut, ms3dVtx,
							ms3dTri[i].s[j], ms3dTri[i].t[j],
							ms3dTri[i].vertexIndices[j]);

			if(vtxInd == -1)
			{
				//append a new vertex
				VtxDat newVtx;

				newVtx.ms3dVtx = ms3dVtx[ms3dTri[i].vertexIndices[j]];
				newVtx.s       = ms3dTri[i].s[j];
				newVtx.t       = ms3dTri[i].t[j];

				vtxOut.push_back(newVtx);

				ms3dTri[i].vertexIndices[j] = vtxOut.size() - 1;
			}
			else
				ms3dTri[i].vertexIndices[j] = vtxInd;
		}
	}
}

PRIVATE RETCODE _MS3DSetModel(hMDL model, word nNumVertices, ms3d_vertex_t *ms3dVtx,
							  word nNumTriangles, ms3d_triangle_t *ms3dTri,
							  word nNumGroups, ms3d_group_t *ms3dGrp,
							  word nNumMaterials, ms3d_material_t *ms3dMaterials,
							  word nNumJoints, ms3d_joint_t	*ms3dJoints,
							  float fAnimationFPS, int iTotalFrames)
{
	int i;

	//assemble the vertices
	VtxArray ourVtx;

	_MS3DAssembleVertices(ourVtx, nNumVertices, ms3dVtx, nNumTriangles, ms3dTri);

	//first, create the mesh
	if(MeshCreate(&model->mesh, nNumGroups, ourVtx.size()) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	/////////////////////////////////////////////////
	//fill in the vertices
	gfxVtx *ptrVtx;
	char   *vtxBoneIDs = MeshGetBoneIDs(&model->mesh);

	if(MeshLockVtx(&model->mesh, 0, &ptrVtx) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	for(i = 0; i < ourVtx.size(); i++)
	{
		ptrVtx[i].x   = ourVtx[i].ms3dVtx.vertex[eX];
		ptrVtx[i].y   = ourVtx[i].ms3dVtx.vertex[eY];
		ptrVtx[i].z   = ourVtx[i].ms3dVtx.vertex[eZ];
		vtxBoneIDs[i] = ourVtx[i].ms3dVtx.boneId;

		_GFXVtxSetTxtCoordAll(&ptrVtx[i], ourVtx[i].s, ourVtx[i].t);
	}

	//we will not unlock the vertices yet...

	/////////////////////////////////////////////////
	//fill in the groups
	for(i = 0; i < nNumGroups; i++)
	{
		//ms3dGrp
		MeshInitGroup(&model->mesh, i, ms3dGrp[i].name, ms3dGrp[i].materialIndex,
			ms3dGrp[i].numtriangles);

		/////////////////
		//now fill the index buffer up
		unsigned short *ptrInd;

		if(MeshLockInd(&model->mesh, i, 0, &ptrInd) != RETCODE_SUCCESS)
			return RETCODE_FAILURE;

		for(int j = 0, k = 0; j < ms3dGrp[i].numtriangles; j++, k += 3)
		{
			ptrInd[k] = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexIndices[0];
			ptrInd[k+1] = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexIndices[1];
			ptrInd[k+2] = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexIndices[2];

			//pain in the ass crap...we will assume there is only one smoothing group
			ptrVtx[ptrInd[k]].nX = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[0][eX];
			ptrVtx[ptrInd[k]].nY = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[0][eY];
			ptrVtx[ptrInd[k]].nZ = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[0][eZ];

			ptrVtx[ptrInd[k+1]].nX = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[1][eX];
			ptrVtx[ptrInd[k+1]].nY = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[1][eY];
			ptrVtx[ptrInd[k+1]].nZ = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[1][eZ];

			ptrVtx[ptrInd[k+2]].nX = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[2][eX];
			ptrVtx[ptrInd[k+2]].nY = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[2][eY];
			ptrVtx[ptrInd[k+2]].nZ = ms3dTri[ms3dGrp[i].triangleIndices[j]].vertexNormals[2][eZ];

			//I still don't know what you did last summer
			/*_GFXVtxSetTxtCoordAll(&ptrVtx[ptrInd[k]], 
			ms3dTri[ms3dGrp[i].triangleIndices[j]].s[0], 
			ms3dTri[ms3dGrp[i].triangleIndices[j]].t[0]);

			_GFXVtxSetTxtCoordAll(&ptrVtx[ptrInd[k+1]], 
			ms3dTri[ms3dGrp[i].triangleIndices[j]].s[1], 
			ms3dTri[ms3dGrp[i].triangleIndices[j]].t[1]);

			_GFXVtxSetTxtCoordAll(&ptrVtx[ptrInd[k+2]], 
			ms3dTri[ms3dGrp[i].triangleIndices[j]].s[2], 
			ms3dTri[ms3dGrp[i].triangleIndices[j]].t[2]);*/
		}

		MeshUnlockInd(&model->mesh, i);
	}

	//now we can unlock the vertices
	MeshUnlockVtx(&model->mesh);

	/////////////////////////////////////////////////
	//fill in the materials
	if(nNumMaterials > 0)
	{
		model->numMaterial = nNumMaterials;

		//allocate textures
		model->textures = (hTXT*)GFX_MALLOC(sizeof(hTXT)*model->numMaterial);
		if(!model->textures)
		{ ASSERT_MSG(0, "Unable to allocate model textures!", "_MS3DSetModel"); return RETCODE_FAILURE; }

		//allocate materials
		model->materials = (GFXMATERIAL*)GFX_MALLOC(sizeof(GFXMATERIAL)*model->numMaterial);
		if(!model->materials)
		{ ASSERT_MSG(0, "Unable to allocate model materials!", "_MS3DSetModel"); return RETCODE_FAILURE; }

		for(i = 0; i < model->numMaterial; i++)
		{
			//bunch 'o gobble dee gook
			model->materials[i].Ambient.r = ms3dMaterials[i].ambient[eR];
			model->materials[i].Ambient.g = ms3dMaterials[i].ambient[eG];
			model->materials[i].Ambient.b = ms3dMaterials[i].ambient[eB];
			model->materials[i].Ambient.a = ms3dMaterials[i].ambient[eA];

			model->materials[i].Diffuse.r = ms3dMaterials[i].diffuse[eR];
			model->materials[i].Diffuse.g = ms3dMaterials[i].diffuse[eG];
			model->materials[i].Diffuse.b = ms3dMaterials[i].diffuse[eB];
			model->materials[i].Diffuse.a = ms3dMaterials[i].diffuse[eA];

			model->materials[i].Emissive.r = ms3dMaterials[i].emissive[eR];
			model->materials[i].Emissive.g = ms3dMaterials[i].emissive[eG];
			model->materials[i].Emissive.b = ms3dMaterials[i].emissive[eB];
			model->materials[i].Emissive.a = ms3dMaterials[i].emissive[eA];

			model->materials[i].Specular.r = ms3dMaterials[i].specular[eR];
			model->materials[i].Specular.g = ms3dMaterials[i].specular[eG];
			model->materials[i].Specular.b = ms3dMaterials[i].specular[eB];
			model->materials[i].Specular.a = ms3dMaterials[i].specular[eA];

			model->materials[i].Power = ms3dMaterials[i].shininess;

			if(ms3dMaterials[i].texture[0] != 0)
				model->textures[i] = TextureCreate(0, ms3dMaterials[i].texture, false, 0);
		}
	}

	/////////////////////////////////////////////////
	//fill in the joints
	if(nNumJoints > 0)
	{
		if(__MS3DSetJoints(model, fAnimationFPS, iTotalFrames, nNumJoints, ms3dJoints) != RETCODE_SUCCESS)
			return RETCODE_FAILURE;
	}

	return RETCODE_SUCCESS;
}

PROTECTED RETCODE _ModelLoadDataFromFileMS3D(hMDL model, const char *filename)
{
	RETCODE ret = RETCODE_SUCCESS;

	int	i;

	char			header[MAXCHARBUFF]={0};
	int				ver;
	
	word			nNumVertices=0;
	ms3d_vertex_t	*ms3dVtx=0;

	word			nNumTriangles=0;
	ms3d_triangle_t	*ms3dTri=0;

	word			nNumGroups=0;
	ms3d_group_t	*ms3dGrp=0;

	word			nNumMaterials=0;
	ms3d_material_t *ms3dMaterials=0;

	float			fAnimationFPS;
	float			fCurrentTime;
	int				iTotalFrames;

	word			nNumJoints=0;
	ms3d_joint_t	*ms3dJoints=0;

	//first off, load the file
	FILE *fp = fopen(filename, "rb");

	if(!fp) { ASSERT_MSG(0, "Unable to open file", "_ModelLoadDataFromFileMS3D"); return RETCODE_FAILURE; }

	//load up the header
	fread(&header, sizeof(char), 10, fp);
	fread(&ver, sizeof(int), 1, fp);

	//check header
	if(stricmp(header, "MS3D000000") != 0)
	{ ASSERT_MSG(0, "Bad MS3D version!", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

	if(ver < 3 || ver > 4 )
	{ ASSERT_MSG(0, "Bad MS3D version!", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

	//so far so good...
	//now get the number of vertices
	fread(&nNumVertices, sizeof(word), 1, fp);

	//allocate vertices
	ms3dVtx = (ms3d_vertex_t*)GFX_MALLOC(sizeof(ms3d_vertex_t)*nNumVertices);
	if(!ms3dVtx)
	{ ASSERT_MSG(0, "Unable to allocate vertices", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

	//grab the vertices
	_MS3DReadVtx(fp, nNumVertices, ms3dVtx);
	//fread(ms3dVtx, sizeof(ms3d_vertex_t), nNumVertices, fp);

	//now get the number of triangles
	fread(&nNumTriangles, sizeof(word), 1, fp);

	//allocate triangles
	ms3dTri = (ms3d_triangle_t*)GFX_MALLOC(sizeof(ms3d_triangle_t)*nNumTriangles);
	if(!ms3dTri)
	{ ASSERT_MSG(0, "Unable to allocate triangles", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

	//grab the triangles
	_MS3DReadTriangle(fp, nNumTriangles, ms3dTri);

	//now get the number of groups
	fread(&nNumGroups, sizeof(word), 1, fp);

	if(nNumGroups > 0)
	{
		//allocate the groups
		ms3dGrp = (ms3d_group_t*)GFX_MALLOC(sizeof(ms3d_group_t)*nNumGroups);
		if(!ms3dGrp)
		{ ASSERT_MSG(0, "Unable to allocate groups", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

		//grab the groups
		ret=_MS3DReadGroups(fp, nNumGroups, ms3dGrp);
		if(ret != RETCODE_SUCCESS) {goto BADSHIT;}
	}
	else { ASSERT_MSG(0, "There should be at least one group!", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

	//get number of materials
	fread(&nNumMaterials, sizeof(word), 1, fp);

	if(nNumMaterials > 0)
	{
		//allocate materials
		ms3dMaterials = (ms3d_material_t*)GFX_MALLOC(sizeof(ms3d_material_t)*nNumMaterials);
		if(!ms3dMaterials)
		{ ASSERT_MSG(0, "Unable to allocate groups", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }

		//grab materials
		_MS3DReadMaterials(filename, fp, nNumMaterials, ms3dMaterials);
	}

	// save some keyframer data (I don't know what this is for...)
	fread(&fAnimationFPS, sizeof(float), 1, fp);
	fread(&fCurrentTime, sizeof(float), 1, fp);
	fread(&iTotalFrames, sizeof(int), 1, fp);

	//get number of joints 
	fread(&nNumJoints, sizeof(word), 1, fp);

	//allocate joints 
	if(nNumJoints > 0)
	{
		ms3dJoints = (ms3d_joint_t*)GFX_MALLOC(sizeof(ms3d_joint_t)*nNumJoints);
		if(!ms3dJoints)
		{ ASSERT_MSG(0, "Unable to allocate joints", "_ModelLoadDataFromFileMS3D"); ret=RETCODE_FAILURE; goto BADSHIT; }
	}

	_MS3DReadJoints(fp, nNumJoints, ms3dJoints, (float)iTotalFrames, fAnimationFPS);

/******************************************************************************/
/******************************************************************************/
	//ok, now that we have the raw data, let's start building up the model
	if(_MS3DSetModel(model, nNumVertices, ms3dVtx,
							  nNumTriangles, ms3dTri,
							  nNumGroups, ms3dGrp,
							  nNumMaterials, ms3dMaterials,
							  nNumJoints, ms3dJoints,
							  fAnimationFPS, iTotalFrames) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; goto BADSHIT; }

BADSHIT:
	//free up stuff...
	if(ms3dVtx)
		GFX_FREE(ms3dVtx);

	if(ms3dTri)
		GFX_FREE(ms3dTri);

	if(ms3dGrp)
	{
		for(i = 0; i < nNumGroups; i++)
		{
			if(ms3dGrp[i].triangleIndices)
				GFX_FREE(ms3dGrp[i].triangleIndices);
		}

		GFX_FREE(ms3dGrp);
	}

	if(ms3dMaterials)
		GFX_FREE(ms3dMaterials);

	if(ms3dJoints)
	{
		for(i = 0; i < nNumJoints; i++)
		{
			if(ms3dJoints[i].keyFramesRot)
				GFX_FREE(ms3dJoints[i].keyFramesRot);

			if(ms3dJoints[i].keyFramesTrans)
				GFX_FREE(ms3dJoints[i].keyFramesTrans);
		}

		GFX_FREE(ms3dJoints);
	}

	fclose(fp);
	return ret;
}