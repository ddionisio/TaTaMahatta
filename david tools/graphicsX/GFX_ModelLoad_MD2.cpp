#include "gdix.h"
#include "gdix_i.h"
#include "gdiX_mesh_i.h"
//All model file loading are done here

#define NUMVERTEXNORMALS	162

double	r_avertexnormals[NUMVERTEXNORMALS][eMaxPt] = {
#include "normTbl.h"
};

struct md2Header {
	int magic;				//magic number: 844121161
	int version;			//version: 8
	int skinWidth;			//width of skin
	int skinHeight;			//height of skin
	int frameSize;			//size of each frame of model
	int numSkins;			//number of skin used
	int numVertices;		//number of vertices
	int numTexcoords;		//number of texture coord
	int numTriangles;		//number of triangles (face)
	int numGlCommands;		//some GL optimization for triangle_strip/fan
	int numFrames;			//number of key frame animations
	int offsetSkins;
	int offsetTexcoords;
	int offsetTriangles;
	int offsetFrames;
	int offsetGlCommands;
	int offsetEnd;
};

struct md2Vtx
{
	unsigned char vertex[3];		//Scaled version of the model's 'real' vertex coordinate
	unsigned char lightNormalIndex;	//An index into the table of normals, kept by Quake 2
};

struct md2Frame
{
	float	   scale[3];		//The scale used by the model's 'fake' vertex structure
	float	   translate[3];	//The translation used by the model's 'fake' vertex structure
	char	   name[16];		//The name for the frame
	md2Vtx vertices[1];		//An array of MD2_VERTEX structures
};

struct md2Face
{
   short vertexIndices[3];
   short textureIndices[3];
};

struct md2TxtCoord
{
   short s, t;
};

struct md2GlCommandVtx
{
   //int numVtx;		//0 = terminate, < 0 = tri-fan, > 0 = tri-strip
   float s, t;
   int vertexIndex;
};

typedef char md2SkinFile[64];

PRIVATE RETCODE _MD2CreateFrame(gfxMesh *pMesh, char *frameName, const md2TxtCoord *txtcoords,
								const BYTE *frames, const md2Face *faces,
								const md2Header &header, int frame)
{
	//create mesh
	MeshCreate(pMesh, 1, header.numVertices);
	MeshInitGroup(pMesh, 0, "MD2", 0, header.numTriangles);

	//go to the correct array of vertices, THE FRAME!
	md2Frame *theFrame = (md2Frame *) ((BYTE*)frames+header.frameSize*frame);

	if(frameName)
		strcpy(frameName, theFrame->name);
	
	////////////////////////////////////////////
	//lock vertex buffer and start putting stuff
	gfxVtx *ptrVtx;
	
	if(MeshLockVtx(pMesh, 0, &ptrVtx) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	int count = 0;

	//copy all the frame's vertices
	for(int j = 0; j < header.numVertices; j++)
	{
		ptrVtx[j].x = (theFrame->vertices[j].vertex[0]*theFrame->scale[0])
					  + theFrame->translate[0];
		ptrVtx[j].y = (theFrame->vertices[j].vertex[1]*theFrame->scale[1])
					  + theFrame->translate[1];
		ptrVtx[j].z = (theFrame->vertices[j].vertex[2]*theFrame->scale[2])
					  + theFrame->translate[2];

		ptrVtx[j].nX = r_avertexnormals[theFrame->vertices[j].lightNormalIndex][eX];
		ptrVtx[j].nY = r_avertexnormals[theFrame->vertices[j].lightNormalIndex][eY];
		ptrVtx[j].nZ = r_avertexnormals[theFrame->vertices[j].lightNormalIndex][eZ];
	}

	//we will not lock the vertices yet...
	////////////////////////////////////////////

	/////////////////
	//now fill the index buffer up
	unsigned short *ptrInd;

	if(MeshLockInd(pMesh, 0, 0, &ptrInd) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	for(int i = 0, k = 0; k < header.numTriangles; k++, i+=3)
	{
		ptrInd[i] = faces[k].vertexIndices[2];
		ptrInd[i+1] = faces[k].vertexIndices[1];
		ptrInd[i+2] = faces[k].vertexIndices[0];

		_GFXVtxSetTxtCoordAll(&ptrVtx[faces[k].vertexIndices[2]], 
			((float)txtcoords[faces[k].textureIndices[2]].s)/header.skinWidth, 
			((float)txtcoords[faces[k].textureIndices[2]].t)/header.skinHeight);

		_GFXVtxSetTxtCoordAll(&ptrVtx[faces[k].vertexIndices[1]], 
			((float)txtcoords[faces[k].textureIndices[1]].s)/header.skinWidth, 
			((float)txtcoords[faces[k].textureIndices[1]].t)/header.skinHeight);

		_GFXVtxSetTxtCoordAll(&ptrVtx[faces[k].vertexIndices[0]], 
			((float)txtcoords[faces[k].textureIndices[0]].s)/header.skinWidth, 
			((float)txtcoords[faces[k].textureIndices[0]].t)/header.skinHeight);
	}

	MeshUnlockVtx(pMesh);
	MeshUnlockInd(pMesh, 0);
	/////////////////

	MeshCopyVtxToVtxSys(pMesh);

	return RETCODE_SUCCESS;
}

//md2 loader
PROTECTED RETCODE _ModelLoadDataFromFileMD2(hMDL model, const char *filename)
{
	assert(model);

	RETCODE ret = RETCODE_SUCCESS;

	md2Header header;			   //the md2 header file
	BYTE            *frames=0;     //the frames (used later to be typecast)
	md2Face         *faces=0;      //3 indices per face
	long			*glCommands=0; //stuff
	md2SkinFile		*skins=0;		   //skin files associated with this model
	md2TxtCoord		*txtcoords=0;  //texture coords in pixel
	int i;

	//ready the file
	FILE *theFile;

	theFile = fopen(filename, "rb");
	if(!theFile)
	{ ASSERT_MSG(0, "Unable to open file", "Error in _ModelLoadDataFromFileMD2"); return RETCODE_FAILURE; }

	//load up the header
	fread(&header, sizeof(md2Header), 1, theFile);

	//confirm format
	if(header.magic != 844121161)
	{ ASSERT_MSG(0, "Bad MD2 magic number, invalid file!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	if(header.version != 8)
	{ ASSERT_MSG(0, "Bad MD2 version number, invalid file!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	////////////////
	//load up frames
	frames = (BYTE*)GFX_MALLOC(sizeof(BYTE)*header.frameSize*header.numFrames);
	if(!frames)
	{ ASSERT_MSG(0, "Unable to allocate frames!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetFrames, SEEK_SET);
	fread(frames, sizeof(BYTE)*header.frameSize*header.numFrames, 1, theFile);
	////////////////

	/////////////////////
	//load up gl commands
	glCommands=(long*)GFX_MALLOC(sizeof(long)*header.numGlCommands);
	if(!glCommands)
	{ ASSERT_MSG(0, "Unable to allocate gl Commands!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetGlCommands, SEEK_SET);
	fread(glCommands, header.numGlCommands*sizeof(long), 1, theFile);
	/////////////////////

	///////////////////////////
	//load up triangles (faces)
	faces = (md2Face*)GFX_MALLOC(sizeof(md2Face)*header.numTriangles);
	if(!faces)
	{ ASSERT_MSG(0, "Unable to allocate triangles!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetTriangles, SEEK_SET);
	fread(faces, sizeof(md2Face)*header.numTriangles, 1, theFile);
	///////////////////////////

	////////////////////////
	//load up the skin files
	if(header.numSkins > 0)
	{
		skins = (md2SkinFile*)GFX_MALLOC(sizeof(md2SkinFile)*header.numSkins);
		if(!skins)
		{ ASSERT_MSG(0, "Unable to allocate skin files!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

		fseek(theFile, header.offsetSkins, SEEK_SET);
		fread(skins, sizeof(md2SkinFile)*header.numSkins, 1, theFile);
	}
	///////////////////////////

	///////////////////////////
	//load up txtcoords
	txtcoords = (md2TxtCoord*)GFX_MALLOC(sizeof(md2TxtCoord)*header.numTexcoords);
	if(!txtcoords)
	{ ASSERT_MSG(0, "Unable to allocate texture coords!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetTexcoords, SEEK_SET);
	fread(txtcoords, sizeof(md2TxtCoord)*header.numTexcoords, 1, theFile);
	///////////////////////////

	/////////////////////////
	//Get some important info
	model->numMaterial = 1;

	//allocate textures
	model->textures = (hTXT*)GFX_MALLOC(sizeof(hTXT)*model->numMaterial);
	if(!model->textures)
	{ ASSERT_MSG(0, "Unable to allocate model textures!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	//allocate materials
	model->materials = (GFXMATERIAL*)GFX_MALLOC(sizeof(GFXMATERIAL)*model->numMaterial);
	if(!model->materials)
	{ ASSERT_MSG(0, "Unable to allocate model materials!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	//create mesh for main mesh and insert frame 0
	_MD2CreateFrame(&model->mesh, 0, txtcoords, frames, faces, header, 0);

	//We will only create frames if numframes is greater than 1

	if(header.numFrames > 1)
	{
		model->numFrames = header.numFrames;

		//allocate frame meshes
		model->frames = (gfxModelFrame*)GFX_MALLOC(sizeof(gfxModelFrame)*model->numFrames);
		if(!model->frames)
		{ ASSERT_MSG(0, "Unable to allocate model frames!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

		//create mesh for each frame
		for(i = 0; i < model->numFrames; i++)
		{
			_MD2CreateFrame(&model->frames[i].frameMesh, model->frames[i].name, 
				txtcoords, frames, faces, header, i);
		}

		SETFLAG(model->status, MDL_FRAME);
	}

BADMOJO3:
	if(frames)
		GFX_FREE(frames);

	if(glCommands)
		GFX_FREE(glCommands);

	if(faces)
		GFX_FREE(faces);

	if(skins)
		GFX_FREE(skins);

	if(txtcoords)
		GFX_FREE(txtcoords);

	//set model as CCW backface culling
	//SETFLAG(model->status, MDL_BACKFACECCW);

	fclose(theFile);
	return ret;
}