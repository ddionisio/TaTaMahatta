#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

#define QBSP_VTXBUFF_USAGE		D3DUSAGE_WRITEONLY//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//D3DUSAGE_DYNAMIC//
#define QBSP_VTXBUFF_POOL		D3DPOOL_MANAGED//D3DPOOL_DEFAULT//D3DPOOL_DEFAULT//

#define QBSP_INDBUFF_USAGE		D3DUSAGE_WRITEONLY
#define QBSP_INDBUFF_POOL		D3DPOOL_MANAGED

#define GAMMA_DEFAULT_LIGHTMAP 3
#define GAMMA_DEFAULT_VTX	   4

#define LIGHTMAP_USAGE		   0

gfxVtxQBSP		*g_tempVtx=0;	//this is allocated during load
								//and is destroyed after load
								//THIS IS ONLY USED DURING LOAD

PRIVATE void _QBSPGammaRGB(BYTE & red, BYTE & green, BYTE & blue, float factor)
{
	float scale = 1.0f, temp = 0.0f;
	float r = 0, g = 0, b = 0;

	r = (float)red;
	g = (float)green;
	b = (float)blue;

	// Multiply the factor by the RGB values, while keeping it to a 255 ratio
	r = r * factor / 255.0f;
	g = g * factor / 255.0f;
	b = b * factor / 255.0f;
	
	// Check if the the values went past the highest value
	if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
	if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
	if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

	// Get the scale for this pixel and multiply it by our pixel values
	scale*=255.0f;		
	r*=scale;	g*=scale;	b*=scale;

	// Assign the new gamma'nized RGB values to our image
	red = (byte)r;
	green = (byte)g;
	blue = (byte)b;
}

PRIVATE RETCODE _QBSPLoadVert(hQBSP qbsp, FILE *fp, BSPLump *lumps)
{
	if(g_tempVtx)
	{ GFX_FREE(g_tempVtx); g_tempVtx = 0; }

	BSPVertex vert;

	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[eLumpVertices].offset, SEEK_SET);

	//now allocate our vertices
	if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(gfxVtxQBSP)*qbsp->numVtx,
		QBSP_VTXBUFF_USAGE, GFXVERTEXQBSPFLAG, QBSP_VTXBUFF_POOL, &qbsp->vtx, 0),
		true, "_QBSPLoadVert"))
		return RETCODE_FAILURE;

	//allocate temporary vertices
	g_tempVtx = (gfxVtxQBSP *)GFX_MALLOC(sizeof(gfxVtxQBSP)*qbsp->numVtx);

	//allocate the vertex color array
	qbsp->vtxClr = (BYTE *)GFX_MALLOC(sizeof(BYTE)*qbsp->numVtx*eMaxClr);

	if(!qbsp->vtxClr) { ASSERT_MSG(0, "Unable to allocate vertex colors", "_QBSPLoadVert");  return RETCODE_FAILURE; }

	gfxVtxQBSP *pVtx = g_tempVtx;//=qbsp->vtx;
	BYTE	   *pVtxClr = qbsp->vtxClr;

	//now fill our stuff
	//we will copy the temporary buffer to the actual buffer later
	for(int i = 0; i < qbsp->numVtx; i++)
	{
		fread(&vert, 1, sizeof(BSPVertex), fp);

		pVtx[i].pt.x = vert.pos[eX];

		// Swap the y and z values, and negate the new z so Y is up.
		pVtx[i].pt.y = vert.pos[eZ];
		pVtx[i].pt.z = -vert.pos[eY];

		// Negate the V texture coordinate because it is upside down otherwise...
		pVtx[i].s = vert.txtCoord[eU];
		pVtx[i].t = vert.txtCoord[eV];

		//put lightmap
		pVtx[i].ls = vert.lightmapCoord[eU];
		pVtx[i].lt = vert.lightmapCoord[eV];

		pVtx[i].n.x = vert.vNormal[eX];
		pVtx[i].n.y = vert.vNormal[eZ];
		pVtx[i].n.z = -vert.vNormal[eY];

		pVtxClr[eR] = vert.color[eR];
		pVtxClr[eG] = vert.color[eG];
		pVtxClr[eB] = vert.color[eB];
		pVtxClr[eA] = vert.color[eA];

		_QBSPGammaRGB(pVtxClr[eR],pVtxClr[eG],pVtxClr[eB], GAMMA_DEFAULT_VTX);

		pVtxClr += eMaxClr;

		pVtx[i].dls = pVtx[i].dlt = 0;
	}

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE _QBSPLoadTxts(hQBSP qbsp, FILE *fp, BSPLump *lumps, const char *mainPath)
{
	BSPTexture bspTxt;

	char buff[MAXCHARBUFF];

	// Seek to the position in the file that stores the texture information
	fseek(fp, lumps[eLumpTextures].offset, SEEK_SET);

	FILE *fpTxt; //use for checking file extension

	//allocate textures
	qbsp->textures = (gfxTxtQBSP*)GFX_MALLOC(sizeof(gfxTxtQBSP)*qbsp->numTxt);
	if(!qbsp->textures)
	{ ASSERT_MSG(0, "Unable to allocate textures", "_QBSPLoadTxts"); return RETCODE_FAILURE; }

	//go through and load up the textures
	for(int i = 0; i < qbsp->numTxt; i++)
	{
		// Read in the texture information
		fread(&bspTxt, 1, sizeof(BSPTexture), fp);

		////////////////////////////////////////
		//try .JPEG
		if(mainPath)
		{
			strcpy(buff, mainPath);
			strcat(buff, "\\");
			strcat(buff, bspTxt.strName);
		}
		else
			strcpy(buff, bspTxt.strName);

		strcat(buff, ".jpg");

		fpTxt = fopen(buff, "rb");

		if(!fpTxt)
		{
			//check if it's a tga
			if(mainPath)
			{
				strcpy(buff, mainPath);
				strcat(buff, "\\");
				strcat(buff, bspTxt.strName);
			}
			else
				strcpy(buff, bspTxt.strName);

			strcat(buff, ".tga");

			fpTxt = fopen(buff, "rb");

			//must be special...to be continued!
			if(!fpTxt)
			{
				qbsp->textures[i].texture=0;
				continue;
			}
			else
				fclose(fpTxt);
		}
		//assume it's a jpeg
		else
			fclose(fpTxt);
		////////////////////////////////////////

		//load the texture
		qbsp->textures[i].flags = bspTxt.flags;
		qbsp->textures[i].contents = bspTxt.contents;

		//determine if invisible QBSP_TXT_INVISIBLE
		char s[128]={0};
		GetFilename(buff, s, 127);

		if(stricmp(s, STR_INVISIBLE) == 0)
		{
			SETFLAG(qbsp->textures[i].flags, QBSP_TXT_INVISIBLE);
			qbsp->textures[i].texture = 0;
		}
		else
		{
			qbsp->textures[i].texture = TextureCreate(0, buff, false, 0);

			if(!qbsp->textures[i].texture)
				SETFLAG(qbsp->textures[i].flags, QBSP_TXT_INVISIBLE);
		}

	}

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE _QBSPLoadPlanes(hQBSP qbsp, FILE *fp, BSPLump *lumps)
{
	//allocate the planes
	qbsp->planes = (D3DXPLANE*)GFX_MALLOC(sizeof(D3DXPLANE)*qbsp->numPlanes);
	if(!qbsp->planes)
	{ ASSERT_MSG(0, "Unable to allocate planes", "_QBSPLoadPlanes"); return RETCODE_FAILURE; }

	BSPPlane bspPlane;

	// Seek to the planes lump in the file
	fseek(fp, lumps[eLumpPlanes].offset, SEEK_SET);

	for(int i = 0; i < qbsp->numPlanes; i++)
	{
		fread(&bspPlane, 1, sizeof(BSPPlane), fp);

		qbsp->planes[i].a = bspPlane.a;
		qbsp->planes[i].b = bspPlane.c;
		qbsp->planes[i].c = -bspPlane.b;

		qbsp->planes[i].d = bspPlane.d;
	}

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE _QBSPLoadMdls(hQBSP qbsp, FILE *fp, BSPLump *lumps)
{
	if(qbsp->numModels > 0)
	{
		qbsp->models = (gfxBSPModel*)GFX_MALLOC(sizeof(gfxBSPModel)*qbsp->numModels);
		if(!qbsp->models)
		{ ASSERT_MSG(0, "Unable to allocate models", "_QBSPLoadMdls"); return RETCODE_FAILURE; }

		// Seek to the models lump in the file
		fseek(fp, lumps[eLumpModels].offset, SEEK_SET);

		BSPModel qbspMdl;

		for(int i = 0; i < qbsp->numModels; i++)
		{
			fread(&qbspMdl, 1, sizeof(BSPModel), fp);

			//copy data stuff
			qbsp->models[i].min[eX] = qbspMdl.min[eX];

			//flip y/z
			qbsp->models[i].min[eY] = MIN(qbspMdl.min[eZ], qbspMdl.max[eZ]);
			qbsp->models[i].min[eZ] = MIN(-qbspMdl.min[eY], -qbspMdl.max[eY]);

			qbsp->models[i].max[eX] = qbspMdl.max[eX];

			//flip y/z
			qbsp->models[i].max[eY] = MAX(qbspMdl.min[eZ], qbspMdl.max[eZ]);
			qbsp->models[i].max[eZ] = MAX(-qbspMdl.min[eY], -qbspMdl.max[eY]);

			qbsp->models[i].faceIndex = qbspMdl.faceIndex;
			qbsp->models[i].numOfFaces = qbspMdl.numOfFaces;
			qbsp->models[i].brushIndex = qbspMdl.brushIndex;
			qbsp->models[i].numOfBrushes = qbspMdl.numOfBrushes;

			//set the transformation stuff
			memset(qbsp->models[i].wrldRot, 0, sizeof(qbsp->models[i].wrldRot));

			if(i != 0)
			{
				///////////////////////////////////////////
				//create the polyhedrons
				qbsp->models[i].brushes = (ConvexPolyhedron *)GFX_MALLOC(sizeof(ConvexPolyhedron)*qbsp->models[i].numOfBrushes);
				assert(qbsp->models[i].brushes);

				for(int b = 0, c = qbsp->models[i].brushIndex; b < qbsp->models[i].numOfBrushes; b++, c++)
				{
					qbsp->models[i].brushes[b].planes   = (D3DXPLANE*)GFX_MALLOC(sizeof(D3DXPLANE)*qbsp->brushes[c].numOfBrushSides);
					qbsp->models[i].brushes[b].tPlanes  = (D3DXPLANE*)GFX_MALLOC(sizeof(D3DXPLANE)*qbsp->brushes[c].numOfBrushSides);

					qbsp->models[i].brushes[b].txtID    = (int*)GFX_MALLOC(sizeof(int)*qbsp->brushes[c].numOfBrushSides);
					qbsp->models[i].brushes[b].numPlane = qbsp->brushes[c].numOfBrushSides;
					
					int plInd;
					
					for(int d = 0, e = qbsp->brushes[c].brushSide; d < qbsp->brushes[c].numOfBrushSides; d++, e++)
					{
						qbsp->models[i].brushes[b].txtID[d] = qbsp->brushSides[e].textureID;
						
						plInd = qbsp->brushSides[e].plane;
						qbsp->models[i].brushes[b].planes[d] = qbsp->planes[plInd];
						qbsp->models[i].brushes[b].tPlanes[d] = qbsp->planes[plInd];
					}
				}
				///////////////////////////////////////////

				qbsp->models[i].wrldTrans[eX] = (qbsp->models[i].min[eX]+qbsp->models[i].max[eX])/2;
				qbsp->models[i].wrldTrans[eY] = (qbsp->models[i].min[eY]+qbsp->models[i].max[eY])/2;
				qbsp->models[i].wrldTrans[eZ] = (qbsp->models[i].min[eZ]+qbsp->models[i].max[eZ])/2;

				D3DXMatrixTranslation(&qbsp->models[i].wrldMtx,
					qbsp->models[i].wrldTrans[eX],
					qbsp->models[i].wrldTrans[eY],
					qbsp->models[i].wrldTrans[eZ]);

				//inverse translate vertices
				float invTrans[eMaxPt] = {-qbsp->models[i].wrldTrans[eX],-qbsp->models[i].wrldTrans[eY],-qbsp->models[i].wrldTrans[eZ]};
				_QBSPModelTranslate(qbsp, g_tempVtx, &qbsp->models[i], invTrans);

				//inverse translate the bbox
				qbsp->models[i].min[eX] += invTrans[eX];
				qbsp->models[i].min[eY] += invTrans[eY];
				qbsp->models[i].min[eZ] += invTrans[eZ];

				qbsp->models[i].max[eX] += invTrans[eX];
				qbsp->models[i].max[eY] += invTrans[eY];
				qbsp->models[i].max[eZ] += invTrans[eZ];

				qbsp->models[i].radius = MAX(qbsp->models[i].max[eX], qbsp->models[i].max[eY]);
				qbsp->models[i].radius = MAX(qbsp->models[i].radius, qbsp->models[i].max[eZ]);
			}
			else
			{
				memset(qbsp->models[i].wrldTrans, 0, sizeof(qbsp->models[i].wrldTrans));

				D3DXMatrixIdentity(&qbsp->models[i].wrldMtx);
			}
		}
	}
	else
		qbsp->models = 0;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PRIVATE void _ChangeGammaBits(BYTE *pImage, int w, int h, int pitch, float factor)
{
	// Go through every pixel in the lightmap
	BYTE *thisBit;

	for(int y = 0; y < h; y++)
	{
		thisBit = pImage + (y*pitch);

		for(int x = 0; x < w; x++)
		{
			_QBSPGammaRGB(thisBit[2], thisBit[1], thisBit[0], factor);

			thisBit += 4; //assume 24-bit
		}
	}
}

PRIVATE RETCODE _QBSPLoadLightmaps(hQBSP qbsp, FILE *fp, BSPLump *lumps)
{
	char lightmapName[MAXCHARBUFF];

	BSPLightmap lightmap;

	if(qbsp->numLightMap > 0)
	{
		//allocate lightmap texture handles
		qbsp->lightmaps = (hTXT*)GFX_MALLOC(sizeof(hTXT)*qbsp->numLightMap);
		if(!qbsp->lightmaps)
		{ ASSERT_MSG(0, "Unable to allocate faces", "_QBSPLoadLightmaps"); return RETCODE_FAILURE; }

		// Seek to the position in the file that stores the lightmap information
		fseek(fp, lumps[eLumpLightmaps].offset, SEEK_SET);

		for(int i = 0; i < qbsp->numLightMap; i++)
		{
			sprintf(lightmapName, "QBSPLightMap%d", i);

			//read lightmap data
			fread(&lightmap, 1, sizeof(BSPLightmap), fp);

			//assume 128x128
			qbsp->lightmaps[i] =
				TextureCreateCustom(0, lightmapName,
									128, 128, 0, LIGHTMAP_USAGE, BPP_32);

			//fill up the new texture
			int pitch;
			BYTE *bitsOut, *thisBit;

			if(TextureLock(qbsp->lightmaps[i], 0, 0, 0,
							   &pitch, (void **)&bitsOut) == RETCODE_SUCCESS)
			{
				for(int y = 0; y < 128; y++)
				{
					thisBit = bitsOut + (y*pitch);

					for(int x = 0; x < 128; x++)
					{
						thisBit[3] = 255;
						thisBit[2] = lightmap.imageBits[y][x][eR];
						thisBit[1] = lightmap.imageBits[y][x][eG];
						thisBit[0] = lightmap.imageBits[y][x][eB];

						thisBit+=4;
					}
				}

				_ChangeGammaBits(bitsOut, 128, 128, pitch, GAMMA_DEFAULT_LIGHTMAP);

				TextureUnlock(qbsp->lightmaps[i], 0);
			}
			else
			{ ASSERT_MSG(0, "Unable to lock lightmap texture!", "_QBSPLoadLightmaps"); return RETCODE_FAILURE; }
		}
	}

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE _QBSPLoadVertInd(hQBSP qbsp, FILE *fp, BSPLump *lumps)
{
	int ind;

	unsigned short *buff;

	//create the index buffer
	//create index buffer
	if(_GFXCheckError(g_p3DDevice->CreateIndexBuffer(sizeof(unsigned short)*qbsp->numMeshVtxInd,
		QBSP_INDBUFF_USAGE, D3DFMT_INDEX16, QBSP_INDBUFF_POOL, &qbsp->meshIndBuff, 0),
		true, "_QBSPLoadVertInd"))
		return RETCODE_FAILURE;

	// Seek to the mesh vertex indices lump in the file
	fseek(fp, lumps[eLumpMeshVerts].offset, SEEK_SET);

	if(SUCCEEDED(qbsp->meshIndBuff->Lock(0, 0, (void**)&buff, D3DLOCK_DISCARD)))
	{
		for(int i = 0; i < qbsp->numMeshVtxInd; i++)
		{
			fread(&ind, 1, sizeof(int), fp);
			buff[i] = (unsigned short)ind;
		}

		qbsp->meshIndBuff->Unlock();
	}

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE _QBSPLoadEntities(hQBSP qbsp, FILE *fp, BSPLump *lumps, ENTITYPARSEFUNC parseFunc, void *userDat)
{
	// Seek to the entity lump in the file
	fseek(fp, lumps[eLumpEntities].offset, SEEK_SET);

	char *entityStr;

	int strSize = 0;
	int entityPos = 1;
	int entitySize = lumps[eLumpEntities].length/sizeof(char);

	while(entityPos < entitySize)
	{
		if(ParserCountStringFile(fp, &strSize, '{', '}') == RETCODE_ENDREACHED && strSize > 0)
		{
			strSize++;
			entityStr = (char *)GFX_MALLOC(sizeof(char)*strSize);

			ParserReadStringFile(fp, entityStr, strSize, '{', '}');
			ParserSkipCharFile(fp, '\n');

			//do whatever
			parseFunc(entityStr, qbsp, userDat);

			GFX_FREE(entityStr);

			entityPos += strSize + 2; //plus 2 for '{' and '}'
		}
		else
		{ ASSERT_MSG(0, "Error parsing!!!", "_QBSPLoadEntities"); return RETCODE_FAILURE; }
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	QBSPLoad
// Purpose:	loads a .bsp file and
//			create a qbsp map
// Output:	none
// Return:	new qbsp, 0 if failure
/////////////////////////////////////
PUBLIC hQBSP QBSPLoad(const char *filename, const char *mainPath,
					  ENTITYPARSEFUNC parseFunc, void *userDat)
{
	int i;

	float temp;

	hQBSP newQBSP = (hQBSP)GFX_MALLOC(sizeof(gfxQBSP));

	FILE *fp;

	BSPHeader header = {0};
	BSPLump lumps[eNumLumps] = {0};

	//allocate qbsp
	if(!newQBSP)
	{ ASSERT_MSG(0, "Unable to allocate qbsp", "QBSPLoad"); QBSPDestroy(&newQBSP); return 0; }

	fp = fopen(filename, "rb");

	if(!fp)
	{ ASSERT_MSG(0, "Unable to open qbsp map", "QBSPLoad"); QBSPDestroy(&newQBSP); return 0; }

	/*************************************************************************/
	// allocate and get sizes first

	// Read in the header and lump data
	fread(&header, 1, sizeof(BSPHeader), fp);
	fread(&lumps, eNumLumps, sizeof(BSPLump), fp);

	////////////////////////////////////////
	//get size of vertices
	newQBSP->numVtx = lumps[eLumpVertices].length / sizeof(BSPVertex);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of faces
	newQBSP->numFaces = lumps[eLumpFaces].length / sizeof(BSPFace);

	//allocate faces
	newQBSP->faces = (BSPFace*)GFX_MALLOC(sizeof(BSPFace)*newQBSP->numFaces);
	if(!newQBSP->faces)
	{ ASSERT_MSG(0, "Unable to allocate faces", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }

	//create vis buffer
	newQBSP->faceVis = QBSPVisBitCreate(newQBSP->numFaces, newQBSP->numFaceVis);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of textures
	newQBSP->numTxt = lumps[eLumpTextures].length / sizeof(BSPTexture);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of lightmaps
	newQBSP->numLightMap = lumps[eLumpLightmaps].length / sizeof(BSPLightmap);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of nodes
	newQBSP->numNodes = lumps[eLumpNodes].length / sizeof(BSPNode);

	//allocate nodes (filled in later)
	newQBSP->nodes = (gfxBSPNode*)GFX_MALLOC(sizeof(gfxBSPNode)*newQBSP->numNodes);
	if(!newQBSP->nodes)
	{ ASSERT_MSG(0, "Unable to allocate nodes", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of leaves
	newQBSP->numLeaves = lumps[eLumpLeafs].length / sizeof(BSPLeaf);

	//allocate nodes (filled in later)
	newQBSP->leaves = (gfxBSPLeaf*)GFX_MALLOC(sizeof(gfxBSPLeaf)*newQBSP->numLeaves);
	if(!newQBSP->leaves)
	{ ASSERT_MSG(0, "Unable to allocate leaves", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of leaf faces
	newQBSP->numLeafFaces = lumps[eLumpLeafFaces].length / sizeof(int);

	//allocate nodes (filled in later)
	newQBSP->leafFaces = (int*)GFX_MALLOC(sizeof(int)*newQBSP->numLeafFaces);
	if(!newQBSP->leafFaces)
	{ ASSERT_MSG(0, "Unable to allocate leaf faces", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of planes
	newQBSP->numPlanes = lumps[eLumpPlanes].length / sizeof(BSPPlane);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of mesh vertices
	newQBSP->numMeshVtxInd = lumps[eLumpMeshVerts].length / sizeof(int);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of models
	newQBSP->numModels = lumps[eLumpModels].length / sizeof(BSPModel);
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of brushes
	newQBSP->numBrushes = lumps[eLumpBrushes].length / sizeof(BSPBrush);

	//allocate brushes (filled in later)
	newQBSP->brushes = (BSPBrush*)GFX_MALLOC(sizeof(BSPBrush)*newQBSP->numBrushes);
	if(!newQBSP->brushes)
	{ ASSERT_MSG(0, "Unable to allocate brushes", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }

	newQBSP->brushVis = QBSPVisBitCreate(newQBSP->numBrushes, newQBSP->numBrushVis);

	//unsigned int		   *brushVis;	 //has brush been processed yet?
	//int						numBrushVis; //number of brush vis
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of leaf brushes
	newQBSP->numLeafBrushes = lumps[eLumpLeafBrushes].length / sizeof(int);

	//allocate brushes (filled in later)
	newQBSP->leafBrushes = (int*)GFX_MALLOC(sizeof(int)*newQBSP->numLeafBrushes);
	if(!newQBSP->leafBrushes)
	{ ASSERT_MSG(0, "Unable to allocate leaf brushes", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//get size of brush sides
	newQBSP->numBrushSides = lumps[eLumpBrushSides].length / sizeof(BSPBrushSide);

	//allocate brushes (filled in later)
	newQBSP->brushSides = (BSPBrushSide*)GFX_MALLOC(sizeof(BSPBrushSide)*newQBSP->numBrushSides);
	if(!newQBSP->brushSides)
	{ ASSERT_MSG(0, "Unable to allocate brush sides", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	/*************************************************************************/

	/*************************************************************************/
	//Now let's parse the files and fill in our map

	////////////////////////////////////////
	//Load up the vertices
	if(_QBSPLoadVert(newQBSP, fp, lumps) != RETCODE_SUCCESS)
	{ QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up faces
	// Seek to the position in the file that stores the face information
	fseek(fp, lumps[eLumpFaces].offset, SEEK_SET);

	// Read in all the face information
	for(i = 0; i < newQBSP->numFaces; i++)
	{
		fread(&newQBSP->faces[i], 1, sizeof(BSPFace), fp);

		temp = -newQBSP->faces[i].vNormal.y;
		newQBSP->faces[i].vNormal.y = newQBSP->faces[i].vNormal.z;
		newQBSP->faces[i].vNormal.z = temp;
	}
	////////////////////////////////////////

	////////////////////////////////////////
	//load up textures
	if(_QBSPLoadTxts(newQBSP, fp, lumps, mainPath) != RETCODE_SUCCESS)
	{ QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up lightmaps
	_QBSPLoadLightmaps(newQBSP, fp, lumps);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up nodes
	BSPNode bspNode;

	// Seek to the position in the file that hold the nodes
	fseek(fp, lumps[eLumpNodes].offset, SEEK_SET);

	for(i = 0; i < newQBSP->numNodes; i++)
	{
		fread(&bspNode, 1, sizeof(BSPNode), fp);

		//copy stuff
		newQBSP->nodes[i].child[1] = bspNode.back;
		newQBSP->nodes[i].child[0] = bspNode.front;
		newQBSP->nodes[i].plane = bspNode.plane;

		newQBSP->nodes[i].min[eX] = bspNode.min[eX];
		newQBSP->nodes[i].min[eY] = MIN(bspNode.min[eZ], bspNode.max[eZ]);
		newQBSP->nodes[i].min[eZ] = MIN(-bspNode.min[eY], -bspNode.max[eY]);

		newQBSP->nodes[i].max[eX] = bspNode.max[eX];
		newQBSP->nodes[i].max[eY] = MAX(bspNode.min[eZ], bspNode.max[eZ]);
		newQBSP->nodes[i].max[eZ] = MAX(-bspNode.min[eY], -bspNode.max[eY]);
	}
	////////////////////////////////////////

	////////////////////////////////////////
	//load up leaves
	BSPLeaf bspLeaf;

	// Seek to the position in the file that holds the leafs
	fseek(fp, lumps[eLumpLeafs].offset, SEEK_SET);

	for(i = 0; i < newQBSP->numLeaves; i++)
	{
		fread(&bspLeaf, 1, sizeof(BSPLeaf), fp);

		//copy stuff
		newQBSP->leaves[i].area = bspLeaf.area;
		newQBSP->leaves[i].cluster = bspLeaf.cluster;
		newQBSP->leaves[i].leafBrush = bspLeaf.leafBrush;
		newQBSP->leaves[i].leafface = bspLeaf.leafface;
		newQBSP->leaves[i].numOfLeafBrushes = bspLeaf.numOfLeafBrushes;
		newQBSP->leaves[i].numOfLeafFaces = bspLeaf.numOfLeafFaces;

		newQBSP->leaves[i].min[eX] = bspLeaf.min[eX];
		newQBSP->leaves[i].min[eY] = MIN(bspLeaf.min[eZ], bspLeaf.max[eZ]);
		newQBSP->leaves[i].min[eZ] = MIN(-bspLeaf.min[eY], -bspLeaf.max[eY]);

		newQBSP->leaves[i].max[eX] = bspLeaf.max[eX];
		newQBSP->leaves[i].max[eY] = MAX(bspLeaf.min[eZ], bspLeaf.max[eZ]);
		newQBSP->leaves[i].max[eZ] = MAX(-bspLeaf.min[eY], -bspLeaf.max[eY]);

		newQBSP->leaves[i].loc.x = (newQBSP->leaves[i].min[eX]+newQBSP->leaves[i].max[eX])/2;
		newQBSP->leaves[i].loc.y = (newQBSP->leaves[i].min[eY]+newQBSP->leaves[i].max[eY])/2;
		newQBSP->leaves[i].loc.z = (newQBSP->leaves[i].min[eZ]+newQBSP->leaves[i].max[eZ])/2;
	}
	////////////////////////////////////////

	////////////////////////////////////////
	// load up leaf faces
	// Seek to the leaf faces lump, then read it's data
	fseek(fp, lumps[eLumpLeafFaces].offset, SEEK_SET);
	fread(newQBSP->leafFaces, newQBSP->numLeafFaces, sizeof(int), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up planes
	if(_QBSPLoadPlanes(newQBSP, fp, lumps) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to load vertex indices", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up vis
	if(lumps[eLumpVisData].length)
	{
		// Seek to the position in the file that holds the visibility lump
		fseek(fp, lumps[eLumpVisData].offset, SEEK_SET);

		// Read in the number of vectors and each vector's size
		fread(&newQBSP->clusters.numOfClusters,	 1, sizeof(int), fp);
		fread(&newQBSP->clusters.bytesPerCluster, 1, sizeof(int), fp);

		// Allocate the memory for the cluster bitsets
		int size = newQBSP->clusters.numOfClusters * newQBSP->clusters.bytesPerCluster;
		
		newQBSP->clusters.pBitsets = (BYTE*)GFX_MALLOC(sizeof(BYTE)*size);
		if(!newQBSP->clusters.pBitsets)
		{ ASSERT_MSG(0, "Unable to allocate PVS", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }

		// Read in the all the visibility bitsets for each cluster
		fread(newQBSP->clusters.pBitsets, 1, sizeof(BYTE)*size, fp);
	}
	else
		newQBSP->clusters.pBitsets = 0;
	////////////////////////////////////////

	////////////////////////////////////////
	//load up mesh vertex indices
	if(_QBSPLoadVertInd(newQBSP, fp, lumps) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to load vertex indices", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up brushes
	// Seek to the brush lump in the file
	fseek(fp, lumps[eLumpBrushes].offset, SEEK_SET);

	//read them all
	fread(newQBSP->brushes, newQBSP->numBrushes, sizeof(BSPBrush), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up leaf brushes
	// Seek to the leaf brush lump in the file
	fseek(fp, lumps[eLumpLeafBrushes].offset, SEEK_SET);

	//read them all
	fread(newQBSP->leafBrushes, newQBSP->numLeafBrushes, sizeof(int), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up brush sides
	// Seek to the brush side lump in the file
	fseek(fp, lumps[eLumpBrushSides].offset, SEEK_SET);

	//read them all
	fread(newQBSP->brushSides, newQBSP->numBrushSides, sizeof(BSPBrushSide), fp);
	////////////////////////////////////////

	////////////////////////////////////////
	//load up models
	if(_QBSPLoadMdls(newQBSP, fp, lumps) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to load models", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	////////////////////////////////////////

	////////////////////////////////////////
	//load up the entities
	
	if(parseFunc)
	{
		if(_QBSPLoadEntities(newQBSP, fp, lumps, parseFunc, userDat) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to load entities", "QBSPLoad"); QBSPDestroy(&newQBSP); goto DIEQUAKEDIE; }
	}
	////////////////////////////////////////

	//copy temporary vertices to actual vertices
	gfxVtxQBSP *pVtx;

	if(SUCCEEDED(newQBSP->vtx->Lock(0,0, (void**)&pVtx, 0)))
	{
		memcpy(pVtx, g_tempVtx, sizeof(gfxVtxQBSP)*newQBSP->numVtx);
		newQBSP->vtx->Unlock();
	}

	//set other stuff
	D3DXMatrixIdentity(&newQBSP->wrldMtx);	

DIEQUAKEDIE:
	fclose(fp);

	//destroy temporary vertices
	if(g_tempVtx)
	{ GFX_FREE(g_tempVtx); g_tempVtx = 0; }

	//set to lightmap by default
	QBSPUseVertexLighting(newQBSP, false);

	return newQBSP;
}