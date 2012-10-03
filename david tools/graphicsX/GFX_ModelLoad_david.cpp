#include "gdix.h"
#include "gdix_i.h"
#include "gdiX_mesh_i.h"

struct txc {
	float s,t;
};

PRIVATE void _ModelComputeNormPlane(D3DXPLANE *plane, gfxVtx *vtx1, gfxVtx *vtx2, gfxVtx *vtx3)
{
	//create a plane and store it to first index to plane buffer
	D3DXVECTOR3 v1(vtx1->x,vtx1->y,vtx1->z);
	D3DXVECTOR3 v2(vtx2->x,vtx2->y,vtx2->z);
	D3DXVECTOR3 v3(vtx3->x,vtx3->y,vtx3->z);

	D3DXPlaneFromPoints(plane, &v1, &v2, &v3);
	D3DXPlaneNormalize(plane, plane);
}

//my own model file!
PROTECTED RETCODE _ModelLoadDataFromFile(hMDL model, const char *filename)
{
	assert(model);

	RETCODE ret = RETCODE_SUCCESS;

	//data structures
	gfxVtx *vtxBuff=0; //it's gonna be biG!, bIG!!, BIG!!!
	
	txc *txcList=0;		//allocated later (numTxc)
	unsigned short *indList=0;		//allocated later (indNum inside model)
	int numTxc;
	int vtxInd[NUMPTFACE], txtInd[NUMPTFACE];

	int vtxCount, faceCount, indCount;

	//ready the file
	FILE *theFile;
	char buff[MAXCHARBUFF];
	bool bLoading = false;

	theFile = fopen(filename, "rt");
	if(!theFile)
	{ ASSERT_MSG(0, "Unable to open file", "Error in _ModelLoadDataFromFile"); return RETCODE_FAILURE; }

	do
	{
		fscanf(theFile, "%s\n", buff);

		if(strcmp(buff, "START") == 0) //start loading stuff if we found the START
			bLoading = true;
		else if(bLoading)
		{
			if(strcmp("[VERTEX]", buff) == 0)
			{
				fscanf(theFile, "numvertex=%d\n", &vtxCount);

				//create the array
				if(MemAlloc((void**)&vtxBuff, sizeof(gfxVtx)*vtxCount, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to allocate vertex buffer", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				//fill 'em up!
				int r,g,b,a;

				for(int i = 0; i < vtxCount; i++)
				{
					fscanf(theFile, "vertex=%f,%f,%f color=%d,%d,%d,%d\n", 
						&vtxBuff[i].x,&vtxBuff[i].y,&vtxBuff[i].z,
						&r,&g,&b,&a);
					vtxBuff[i].color = D3DCOLOR_RGBA(r,g,b,a);

					//get the normalized vector and put it as the normal for lighting
					//D3DXVECTOR3 vect(vtxBuff[i].x, vtxBuff[i].y, vtxBuff[i].z);
					//D3DXVec3Normalize(&vect,&vect);
					//float *nV = (float*)vect;
					//vtxBuff[i].nX = -nV[eX]; vtxBuff[i].nY = -nV[eY]; vtxBuff[i].nZ = -nV[eZ];
				}
			}
			else if(strcmp("[TEXTURE]", buff) == 0)
			{
				fscanf(theFile, "image=%s\n", buff);

				int filtermode;

				fscanf(theFile, "filtermode=%d\n", &filtermode);

				//we will only have one texture for now...
				model->numMaterial = 1;

				//allocate texture array
				if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to allocate textures", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }
				MemSetPattern(model->textures, "MDLTXTS");

				model->textures[0] = TextureCreate(MDLTXTID, buff, false, 0);
				TextureAddRef(model->textures[0]);
				//D3DXCreateTextureFromFile(g_p3DDevice, buff, &model->texture);
				
				if(!model->textures[0])
				{ ASSERT_MSG(0,"Unable to load texture", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				//now get the texture locations
				fscanf(theFile, "numtextureloc=%d\n", &numTxc);

				//create the array
				if(MemAlloc((void**)&txcList, sizeof(txc)*numTxc, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to create texture location list", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				MemSetPattern(txcList, "GFXTXTL");

				//fill 'er up
				for(int i = 0; i < numTxc; i++)
				{
					fscanf(theFile, "texloc=%f,%f\n", &txcList[i].s, &txcList[i].t);
				}
			}
			else if(strcmp("[FACE]", buff) == 0)
			{
				fscanf(theFile, "numface=%d\n", &faceCount);

				//create the BIG @$$ array
				indCount = faceCount*NUMPTFACE;
				if(MemAlloc((void**)&indList, sizeof(unsigned short)*indCount, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to allocate index buffer, It's not so HUGE!!!", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				MemSetPattern(vtxBuff, "GFXINDB");

				//fill 'em up
				for(int i = 0, j = 0; i < faceCount; i++, j+=NUMPTFACE)
				{
					fscanf(theFile, "vertexIndex=%d,%d,%d textureIndex=%d,%d,%d\n", 
						&vtxInd[0], &vtxInd[1], &vtxInd[2],
						&txtInd[0], &txtInd[1], &txtInd[2]);

					indList[j] = vtxInd[0];
					indList[j+1] = vtxInd[1];
					indList[j+2] = vtxInd[2];

					//do some messed up texture assign
					vtxBuff[vtxInd[0]].s = txcList[txtInd[0]].s;
					vtxBuff[vtxInd[0]].t = txcList[txtInd[0]].t;
					vtxBuff[vtxInd[1]].s = txcList[txtInd[1]].s;
					vtxBuff[vtxInd[1]].t = txcList[txtInd[1]].t;
					vtxBuff[vtxInd[2]].s = txcList[txtInd[2]].s;
					vtxBuff[vtxInd[2]].t = txcList[txtInd[2]].t;
				}
			}
			else if(strcmp("END", buff) == 0)
				bLoading = false;
		}
	}while(bLoading);

	//////////////////////////////////////////////////////////////
	//now time to stuff it all in the D3D vertex buffer
	//create the mesh
	MeshCreate(&model->mesh, faceCount, vtxCount);
	//////////////////////////////////////////////////////////////

	/////////////////
	//now fill 'er up
	gfxVtx *ptrVtx;

	if(MeshLockVtx(&model->mesh, 0, &ptrVtx) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; goto BADMOJO; }

	memcpy(ptrVtx, vtxBuff, sizeof(gfxVtx)*vtxCount);

	MeshUnlockVtx(&model->mesh);
	/////////////////

	/////////////////
	//now fill the index buffer up
	unsigned short *ptrInd;

	if(MeshLockInd(&model->mesh, 0, &ptrInd) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; goto BADMOJO; }

	memcpy(ptrInd, indList, sizeof(unsigned short)*indCount);

	MeshUnlockInd(&model->mesh);
	/////////////////

	/////////////////////////////
	//create the adjacency buffer
	//and do some funky stuff
	MeshCreateAdjacency(&model->mesh);
	//////////////////////////////

	//This outta make lighting easy
	MeshComputeNormals(&model->mesh);

BADMOJO:

	if(theFile) fclose(theFile);
	if(indList) MemFree((void**)&indList);
	if(vtxBuff) MemFree((void**)&vtxBuff);
	if(txcList) MemFree((void**)&txcList);

	return ret;
}