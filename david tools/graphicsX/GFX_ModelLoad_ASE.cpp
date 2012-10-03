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

//3ds ascii file textured
PROTECTED RETCODE _ModelLoadDataFromFileASE(hMDL model, const char *filename)
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

	theFile = fopen(filename, "rt"); //FileOpenFile(filename, FILEREAD | FILETEXT);
	if(!theFile)
	{ ASSERT_MSG(0,"Unable to open file", "Error in _ModelLoadDataFromFileASE"); return RETCODE_FAILURE; }

	//now let's parse this baby!
	if(ParserSkipString(theFile, '*', ' ', "BITMAP"))
	{
		//let's get the filename
		if(ParserReadStringFile(theFile, buff, MAXCHARBUFF, '"', '"') == RETCODE_FAILURE) //we have reached end of file
		{ ASSERT_MSG(0,"Error reading ASE model file, cannot get BITMAP file!", "Error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

		//we will only have one texture for now...
		model->numMaterial = 1;

		//allocate texture array
		if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to allocate textures", "error in _ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
		MemSetPattern(model->textures, "MDLTXTS");

		model->textures[0] = TextureCreate(MDLTXTID, buff, false, 0);
		TextureAddRef(model->textures[0]);
		//D3DXCreateTextureFromFile(g_p3DDevice, buff, &model->texture);
		
		if(!model->textures[0])
		{ ASSERT_MSG(0,"Unable to load texture", "error in _ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

	//now let's find the num vertices
	if(ParserSkipString(theFile, '*', ' ', "MESH_NUMVERTEX"))
	{
		//get the number
		fscanf(theFile, "%d", &vtxCount);

		//create the array
		if(MemAlloc((void**)&vtxBuff, sizeof(gfxVtx)*vtxCount, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to allocate vertex buffer", "error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

		MemSetPattern(vtxBuff, "GFXVTXB");
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

	//now get the num faces
	if(ParserSkipString(theFile, '*', ' ', "MESH_NUMFACES"))
	{
		//get the number
		fscanf(theFile, "%d", &faceCount);

		//create the BIG @$$ array
		indCount = faceCount*NUMPTFACE;
		if(MemAlloc((void**)&indList, sizeof(unsigned short)*indCount, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to allocate vertex buffer, It's not HUGE!!!", "error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

		MemSetPattern(indList, "GFXINDB");
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

	int i, j, dummy;

	//load up the vertex list
	for(i = 0; i < vtxCount; i++)
	{
		//skip "*MESH_VERTEX "
		if(ParserSkipString(theFile, '*', ' ', "MESH_VERTEX"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, 9) == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file 'dummy'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need
			if(ParserReadDataFile(theFile, &vtxBuff[i].x, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file 'vtx X'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxBuff[i].y, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file 'vtx Y'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxBuff[i].z, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file 'vtx Z'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
			
			vtxBuff[i].color = 0xFFFFFFFF;

			//get the normalized vector and put it as the normal for lighting
			D3DXVECTOR3 vect(vtxBuff[i].x, vtxBuff[i].y, vtxBuff[i].z);
			D3DXVec3Normalize(&vect,&vect);
			float *nV = (float*)vect;
			vtxBuff[i].nX = -nV[eX]; vtxBuff[i].nY = -nV[eY]; vtxBuff[i].nZ = -nV[eZ];
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file '*MESH_VERTEX '", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}

	//now to fill in the face index
	for(i = 0, j = 0; i < faceCount; i++, j+=NUMPTFACE)
	{
		//skip "*MESH_FACE "
		if(ParserSkipString(theFile, '*', ' ', "MESH_FACE"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, ':') == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need

			//skip this junk A:
			if(ParserReadWordFile(theFile, buff, MAXCHARBUFF, ':') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxInd[0], dataINT, ' ') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//skip this junk B:
			if(ParserReadWordFile(theFile, buff, MAXCHARBUFF, ':') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxInd[1], dataINT, ' ') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//skip this junk C:
			if(ParserReadWordFile(theFile, buff, MAXCHARBUFF, ':') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxInd[2], dataINT, ' ') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			indList[j] = vtxInd[0];
			indList[j+1] = vtxInd[1];
			indList[j+2] = vtxInd[2];
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}

	//now let's get the number of texture vertices
	if(ParserSkipString(theFile, '*', ' ', "MESH_NUMTVERTEX"))
	{
		//get the number
		fscanf(theFile, "%d", &numTxc);

		//create the array
		if(MemAlloc((void**)&txcList, sizeof(txc)*numTxc, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to create texture location list", "error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

		MemSetPattern(txcList, "GFXTXTL");
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

	float junk; //we don't need the depth in texture for now...

	//let's fill 'er up
	for(i = 0; i < numTxc; i++)
	{
		//skip "*MESH_TVERT "
		if(ParserSkipString(theFile, '*', ' ', "MESH_TVERT"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, 9) == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need
			if(ParserReadDataFile(theFile, &txcList[i].s, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &txcList[i].t, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &junk, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
			
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}

	//let's fill in the face index
	//number of face to have texture will be the number of faces created...for now...
	for(i = 0, j = 0; i < faceCount; i++, j+=NUMPTFACE)
	{
		//skip "*MESH_TFACE "
		if(ParserSkipString(theFile, '*', ' ', "MESH_TFACE"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, 9) == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need
			if(ParserReadDataFile(theFile, &txtInd[0], dataINT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &txtInd[1], dataINT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &txtInd[2], dataINT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			vtxBuff[indList[j]].s = txcList[txtInd[0]].s;
			vtxBuff[indList[j]].t = txcList[txtInd[0]].t;

			vtxBuff[indList[j+1]].s = txcList[txtInd[0]].s;
			vtxBuff[indList[j+1]].t = txcList[txtInd[0]].t;

			vtxBuff[indList[j+2]].s = txcList[txtInd[2]].s;
			vtxBuff[indList[j+2]].t = txcList[txtInd[2]].t;
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); fclose(theFile); return RETCODE_FAILURE; }
	}

	//////////////////////////////////////////////////////////////
	//now time to stuff it all in the D3D vertex buffer
	//create the mesh
	MeshCreate(&model->mesh, faceCount, vtxCount);
	//////////////////////////////////////////////////////////////

	/////////////////
	//now fill 'er up
	gfxVtx *ptrVtx;

	if(MeshLockVtx(&model->mesh, 0, &ptrVtx) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; goto BADMOJO2; }

	memcpy(ptrVtx, vtxBuff, sizeof(gfxVtx)*vtxCount);

	MeshUnlockVtx(&model->mesh);
	/////////////////

	/////////////////
	//now fill the index buffer up
	unsigned short *ptrInd;

	if(MeshLockInd(&model->mesh, 0, &ptrInd) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; goto BADMOJO2; }

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

BADMOJO2:

	if(theFile) fclose(theFile);
	if(indList) MemFree((void**)&indList);
	if(vtxBuff) MemFree((void**)&vtxBuff);
	if(txcList) MemFree((void**)&txcList);

	return ret;
}