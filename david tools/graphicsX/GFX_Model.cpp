#include "gdix.h"
#include "gdix_i.h"
#include "gdix_mesh_i.h"
#include "gdiX_joints_i.h"

//All model creation/destruction/manipulation are done here

PRIVATE void _destroyMdl(hMDL *model) //this will destroy the model, will not search list
{
	if(*model)
	{
		MeshDestroy(&(*model)->mesh);

		if((*model)->frames)
		{
			for(int i = 0; i < (*model)->numFrames; i++)
				MeshDestroy(&(*model)->frames[i].frameMesh);

			GFX_FREE((*model)->frames);
		}

		if((*model)->textures)
		{
			for(int i = 0; i < (*model)->numMaterial; i++)
				TextureDestroy(&(*model)->textures[i]);
			
			GFX_FREE((*model)->textures);
		}

		if((*model)->materials)
			GFX_FREE((*model)->materials);

		if((*model)->joints)
		{
			for(int i = 0; i < (*model)->numJoints; i++)
				JointDestroy(&(*model)->joints[i]);

			GFX_FREE((*model)->joints);
		}

		GFX_FREE(*model);

		model=0;
	}
}

PRIVATE void _MDLCenterModel(hMDL theMdl)
{
	//center vertices in model space

	//set up the base model
	MeshNegativeTranslate(&theMdl->mesh, theMdl->bound.center);

	//go through the frames and center 'em as well
	if(TESTFLAGS(theMdl->status, MDL_FRAME))
	{
		for(int i = 0; i < theMdl->numFrames; i++)
			MeshNegativeTranslate(&theMdl->frames[i].frameMesh, theMdl->bound.center);
	}

	//zero out center since model is centered
	memset(theMdl->bound.center, 0, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLCreate(unsigned int newID, const char *filename)
{
	char theName[NAMEMAX]={0};
	//GetFilename(filename, theName, NAMEMAX-1);
	strcpy(theName, filename);

	//See if this model already exists...
	hMDL newMdl = MDLSearchName(theName);

	if(newMdl)
	{
		MDLAddRef(newMdl);
		return newMdl;
	}

	newMdl = (hMDL)GFX_MALLOC(sizeof(gfxModel));

	if(!newMdl)
	{ ASSERT_MSG(0, "Unable to allocate new model", "Error in GFXMdlCreate"); return 0; }

	newMdl->ID = newID;

	//load model from file
	char ext[MAXCHARBUFF]; //file extension

	strcpy(ext, GetExtension(filename));

	if(stricmp(ext, ".md2") == 0) //md2 files
	{
		if(_ModelLoadDataFromFileMD2(newMdl, filename) == RETCODE_FAILURE)
		{ _destroyMdl(&newMdl); return 0; }

		SETFLAG(newMdl->status, MDL_YDEPTH);
	}
	else if(stricmp(ext, ".ms3d") == 0) //ms3d files
	{
		if(_ModelLoadDataFromFileMS3D(newMdl, filename) == RETCODE_FAILURE)
		{ _destroyMdl(&newMdl); return 0; }

		//SETFLAG(newMdl->status, MDL_YDEPTH);
	}

	//load up the model FX if it exists
	//assume the FX to be at the same place with the same name,
	//but with the '.mfx' extension
	char mdlFXPath[MAXCHARBUFF];

	strcpy(mdlFXPath, filename);
	strcpy(GetExtension(mdlFXPath), ".mfx");
	MDLFXLoad(newMdl, mdlFXPath);

	//get the name
	strcpy(newMdl->name, theName);

	//caliculate the bounding sphere
	//we will only calculate the base mesh
	MeshComputeBound(&newMdl->mesh, &newMdl->bound);

	//center model
	//_MDLCenterModel(newMdl);

	//append to list
	g_MDLLIST.insert(g_MDLLIST.end(), (unsigned int)newMdl);

	return newMdl;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLAddRef(hMDL model)
{
	model->ref++;
}

/////////////////////////////////////
// Name:	MDLSetListFile
// Purpose:	sets the model list file
// Output:	the model list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLSetListFile(const char *filename)
{
	//just copy it
	strcpy(g_MDLLISTFILE, filename);
}

PRIVATE hMDL _MDLSearchList(unsigned int ID)
{
	hMDL thisMdl;

	for(LISTPTR::iterator i = g_MDLLIST.begin(); i != g_MDLLIST.end(); ++i)
	{
		unsigned int ptr = *i;
		thisMdl = (hMDL)ptr;

		if(thisMdl->ID == ID)
			return thisMdl;
	}

	return 0;
}

/////////////////////////////////////
// Name:	MDLSearchName
// Purpose:	search model by name
// Output:	none
// Return:	the model
/////////////////////////////////////
PUBLIC hMDL MDLSearchName(const char *name)
{
	hMDL thisMdl;

	for(LISTPTR::iterator i = g_MDLLIST.begin(); i != g_MDLLIST.end(); ++i)
	{
		unsigned int ptr = *i;
		thisMdl = (hMDL)ptr;

		if(stricmp(thisMdl->name, name) == 0)
			return thisMdl;
	}

	return 0;
}

/////////////////////////////////////
// Name:	MDLSearch
// Purpose:	load model via list file 
//			with ID.  You must give the
//			texture list file as well...
// Output:	model is added if not loaded
//			yet
// Return:	the model if ID is found
/////////////////////////////////////
PUBLIC hMDL MDLSearch(unsigned int ID)
{
	hMDL model=_MDLSearchList(ID); //check to see if it exist on the list...

	if(!model)
	{
		char mdlFName[MAXCHARBUFF], useMat;
		unsigned int mdlID;
		//unsigned int bndID;
		int txtID;
		GFXMATERIAL mat={0};

		//load up file
		FILE *fp  = fopen(g_MDLLISTFILE, "rt");

		if(!fp)
			return 0;

		while(!feof(fp))
		{
			fscanf(fp, "%d %s img=%d material:%c", &mdlID, mdlFName, &txtID, &useMat);

			if(useMat == 'Y')
				fscanf(fp, " D:%f,%f,%f,%f A:%f,%f,%f,%f S:%f,%f,%f,%f E:%f,%f,%f,%f P:%f",
				&mat.Specular.r,&mat.Specular.g,&mat.Specular.b,&mat.Specular.a,
				&mat.Ambient.r,&mat.Ambient.g,&mat.Ambient.b,&mat.Ambient.a,
				&mat.Specular.r,&mat.Specular.g,&mat.Specular.b,&mat.Specular.a,
				&mat.Emissive.r,&mat.Emissive.g,&mat.Emissive.b,&mat.Emissive.a,
				&mat.Power);
			else
				fscanf(fp, "\n");

			if(mdlID == ID) //does it match???
			{
				//load this puppy up!
				model = MDLCreate(ID, mdlFName);

				if(model)
				{
					if(txtID >= 0) //set texture if ID > 0
						MDLSetTexture(model, 0, TextureSearch(txtID));

//					if ( bndID >= 0 )
						//model->pBound = BoundSearch ( bndID );

					if(useMat == 'Y') //set material if useMat == 'Y'
						MDLSetMaterial(model, 0, mat);
				}

				fclose ( fp );

				break;
			}
		}
	}
	else
		MDLAddRef(model);

	return model;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _MDLDestroyAll()
{
	hMDL thisMdl;

	for(LISTPTR::iterator i = g_MDLLIST.begin(); i != g_MDLLIST.end(); ++i)
	{
		thisMdl = (hMDL)(*i);
		_destroyMdl(&thisMdl);
	}

	g_MDLLIST.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLDestroy(hMDL *model)
{
	if(*model)
	{
		if((*model)->ref <= 0)
		{
			//just remove the pointer of model from list
			g_MDLLIST.remove((unsigned int)(*model));

			_destroyMdl(model);
		}
		else
			(*model)->ref--;
	}

	model=0;
}

/////////////////////////////////////
// Name:	MDLGetTexture
// Purpose:	get model's
//			texture within material
//			index
// Output:	none
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT MDLGetTexture(hMDL model, int materialInd)
{
	if(materialInd < model->numMaterial)
		return model->textures[materialInd];
	return 0;
}

/////////////////////////////////////
// Name:	MDLSetTexture
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetTexture(hMDL model, int materialInd, hTXT texture)
{
	//allocate textures if there is none
	//set it automatically to one
	if(model->numMaterial == 0)
	{
		model->numMaterial = 1;
		model->textures = (hTXT*)GFX_MALLOC(sizeof(hTXT)*model->numMaterial);
		if(!model->textures)
		{ ASSERT_MSG(0, "Unable to allocate model textures", "Error in MDLSetTexture"); return RETCODE_FAILURE; }

		model->textures[0] = texture;
		TextureAddRef(model->textures[0]);
	}
	else
	{
		if(materialInd >= model->numMaterial)
		{ ASSERT_MSG(0, "Bad material index", "Error in MDLSetTexture"); return RETCODE_FAILURE; }

		TextureDestroy(&model->textures[materialInd]);

		model->textures[materialInd] = texture;
		TextureAddRef(model->textures[materialInd]);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MDLSetMaterial
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetMaterial(hMDL model, int materialInd, const GFXMATERIAL &material)
{
	//allocate materials if there is none
	//set it automatically to one
	if(model->numMaterial == 0)
	{
		model->numMaterial = 1;
		model->materials = (GFXMATERIAL*)GFX_MALLOC(sizeof(GFXMATERIAL)*model->numMaterial);
		if(!model->materials)
		{ ASSERT_MSG(0, "Unable to allocate model materials", "Error in MDLSetMaterial"); return RETCODE_FAILURE; }

		memcpy(&model->materials[0], &material, sizeof(GFXMATERIAL));
	}
	else
	{
		if(materialInd >= model->numMaterial)
		{ ASSERT_MSG(0, "Bad material index", "Error in MDLSetMaterial"); return RETCODE_FAILURE; }

		memcpy(&model->materials[materialInd], &material, sizeof(GFXMATERIAL));
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MDLGetBound
// Purpose:	grabs model's bound info
// Output:	none
// Return:	the model's bound
/////////////////////////////////////
PUBLIC gfxBound MDLGetBound(hMDL model)
{
	return model->bound;
}

/////////////////////////////////////
// Name:	MDLGetNumVtx
// Purpose:	grabs the number of vtx
//			the model has
// Output:	none
// Return:	number of vtx
/////////////////////////////////////
PUBLIC DWORD MDLGetNumVtx(hMDL model)
{
	return MeshGetNumVtx(&model->mesh);
}

/////////////////////////////////////
// Name:	MDLGetVtx
// Purpose:	grabs the vtx of model with
//			given number.
// Output:	none
// Return:	number of vtx copied
/////////////////////////////////////
PUBLIC DWORD MDLGetVtx(hMDL model, float vtx[100][eMaxPt], int numVtx)
{
	memset(vtx, 0, sizeof(float)*numVtx*eMaxPt);

	DWORD max = 0;

	gfxVtx *pVtx;

	if(MeshLockVtx(&model->mesh, D3DLOCK_READONLY, &pVtx) != RETCODE_SUCCESS)
		return max;
	
	DWORD numMdlVtx = MeshGetNumVtx(&model->mesh);
	max = numMdlVtx < numVtx ? numMdlVtx : numVtx;

	for(int i = 0; i < max; i++)
		memcpy(vtx[i], &pVtx[i], sizeof(float)*eMaxPt);

	MeshUnlockVtx(&model->mesh);

	return max;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int MDLGetID(hMDL model)
{
	return model->ID;
}

/////////////////////////////////////
// Name:	GFXUpdateModels
// Purpose:	update all models (used for Model FXs)
// Output:	stuff updated
// Return:	none
/////////////////////////////////////
PUBLIC void GFXUpdateModels()
{
	hMDL thisMdl;

	for(LISTPTR::iterator i = g_MDLLIST.begin(); i != g_MDLLIST.end(); ++i)
	{
		thisMdl = (hMDL)(*i);

		MeshUpdate(&thisMdl->mesh);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void MDLInvalidateAll()
{
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void MDLRestoreAll()
{
}