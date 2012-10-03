#include "gdix.h"
#include "gdix_i.h"
#include "gdiX_mesh_i.h"

#define HEIGHTITER		.5f
#define MIDDLEHEIGHT	.25f

#define GETHEIGHT(h) _GFXMathRand(-(h), (h))

//This is the midpoint fractal terrain generator

//model map generator stuff

PRIVATE float _MapGetAvgCorner(int i, int j, int stride, int size, float *points)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "*", and the four value we want to average are
       "X"s:
           X   .   X

           .   *   .

           X   .   X
       */
    return ((float) (points[((i-stride)*size) + j-stride] +
		     points[((i-stride)*size) + j+stride] +
		     points[((i+stride)*size) + j-stride] +
		     points[((i+stride)*size) + j+stride]) * MIDDLEHEIGHT);
}

PRIVATE float _MapGetAvgDiamond(int i, int j, int stride,
			     int size, int subSize, float *points)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "X", and the four value we want to average are
       "*"s:
           .   *   .

           *   X   *

           .   *   .
       */

    /* In order to support tiled surfaces which meet seamless at the
       edges (that is, they "wrap"), We need to be careful how we
       calculate averages when the i,j diamond center lies on an edge
       of the array. The first four 'if' clauses handle these
       cases. The final 'else' clause handles the general case (in
       which i,j is not on an edge).
     */
    if (i == 0)
	return ((float) (points[(i*size) + j-stride] +
			 points[(i*size) + j+stride] +
			 points[((subSize-stride)*size) + j] +
			 points[((i+stride)*size) + j]) * MIDDLEHEIGHT);
    else if (i == size-1)
	return ((float) (points[(i*size) + j-stride] +
			 points[(i*size) + j+stride] +
			 points[((i-stride)*size) + j] +
			 points[((0+stride)*size) + j]) * MIDDLEHEIGHT);
    else if (j == 0)
	return ((float) (points[((i-stride)*size) + j] +
			 points[((i+stride)*size) + j] +
			 points[(i*size) + j+stride] +
			 points[(i*size) + subSize-stride]) * MIDDLEHEIGHT);
    else if (j == size-1)
	return ((float) (points[((i-stride)*size) + j] +
			 points[((i+stride)*size) + j] +
			 points[(i*size) + j-stride] +
			 points[(i*size) + 0+stride]) * MIDDLEHEIGHT);
    else
	return ((float) (points[((i-stride)*size) + j] +
			 points[((i+stride)*size) + j] +
			 points[(i*size) + j-stride] +
			 points[(i*size) + j+stride]) * MIDDLEHEIGHT);
}

PRIVATE void _MapGeneratePoints(float *points, float height, float r, int size)
{
	int subSize = size;
	size++;

	float hRatio = (float)powf(2.,-r); //height ratio
	float hScale = height*hRatio;

	//generate the first four value
	//let the first four corner have the same value

	int stride = subSize/2;
    points[(0*size)+0] =
      points[(subSize*size)+0] =
        points[(subSize*size)+subSize] =
          points[(0*size)+subSize] = 0.f;

	//now go through each of the 4 subdivisions and create midpoint heights
	//the first iteration would produce the center (the diamond process) then
	//we will take each four corners and randomize the height (the square process)
	//then we go back to (the diamond process) and so on...until stride reaches 0
	int i, j, oddLine;
	while (stride)
	{
		//Do the diamond process
		//Creating the center point
		for(i = stride; i < subSize; i += stride)
		{
			for(j = stride; j < subSize; j += stride)
			{
				points[(i * size) + j] =
					hScale * GETHEIGHT(HEIGHTITER) +
					_MapGetAvgCorner(i, j, stride, size, points);
				j += stride;
			}
			i += stride;
		}

		//Do the square process
		//Split to four regions and generate corner heights
		oddLine = 0;
		for(i = 0; i < subSize; i += stride)
		{
		    oddLine = (oddLine == 0);
			for(j = 0; j < subSize; j += stride)
			{
				if((oddLine) && !j) j += stride;

				/* i and j are setup. Call avgDiamondVals with the
				   current position. It will return the average of the
				   surrounding diamond data points. */
				points[(i * size) + j] =
					hScale * GETHEIGHT(HEIGHTITER) +
					_MapGetAvgDiamond(i, j, stride, size, subSize, points);

				/* To wrap edges seamlessly, copy edge values around
				   to other side of array */
				if (i==0)
					points[(subSize*size) + j] =
						points[(i * size) + j];
				if (j==0)
					points[(i*size) + subSize] =
						points[(i * size) + j];

				j+=stride;
			}
		}

		//reduce height range and move on to other corners
		hScale *= hRatio;
		stride >>= 1;
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLGenMap(unsigned int newID, float mapSizeX, float mapSizeZ, float height, float r,
				 unsigned int numIter, hTXT texture, GFXMATERIAL *mat)
{
	int size = 1<<numIter;
	int numVtx = (size+1)*(size+1);

	//allocate the points
	float *points;

	//HACK for now...
	points = (float*)GFX_MALLOC(sizeof(float)*(numVtx+2));
	if(!points)
	{ ASSERT_MSG(0, "Unable to allocate points", "Error in MDLGenMap"); return 0; }

	//generate the points!
	_MapGeneratePoints(points, height, r, size);

	hMDL newMdl = (hMDL)GFX_MALLOC(sizeof(gfxModel));

	if(!newMdl)
	{ ASSERT_MSG(0, "Unable to allocate new model", "Error in ModelGenMap"); return 0; }

	//fill 'er up
	newMdl->ID = newID;
	newMdl->numMaterial = 1;

	if(texture)
	{
		newMdl->textures = (hTXT*)GFX_MALLOC(sizeof(hTXT)*newMdl->numMaterial);
		if(!newMdl->textures)
		{ ASSERT_MSG(0, "Unable to allocate model textures", "Error in ModelGenMap"); return 0; }

		newMdl->textures[0] = texture; 
		TextureAddRef(newMdl->textures[0]);
	}

	newMdl->materials = (GFXMATERIAL*)GFX_MALLOC(sizeof(GFXMATERIAL)*newMdl->numMaterial);
	if(!newMdl->materials)
	{ ASSERT_MSG(0, "Unable to allocate materials", "Error in ModelGenMap"); return 0; }

	if(!mat)
	{
		newMdl->materials[0].Diffuse.r = 1.0f;
		newMdl->materials[0].Diffuse.g = 1.0f;
		newMdl->materials[0].Diffuse.b = 1.0f;
		newMdl->materials[0].Diffuse.a = 1.0f;
		//newMdl->materials[0].Specular = newMdl->materials[0].Emissive = newMdl->materials[0].Ambient = newMdl->materials[0].Diffuse;
		newMdl->materials[0].Ambient = newMdl->materials[0].Diffuse;
		//newMdl->materials[0].Power = 1.0f;
	}
	else
		memcpy(newMdl->materials, mat, sizeof(GFXMATERIAL));	

	//////////////////////////////////////////////////////////////
	//now time to stuff it all in the D3D vertex buffer
	//create the mesh
	MeshCreate(&newMdl->mesh, 1, numVtx);
	MeshInitGroup(&newMdl->mesh, 0, "RANDOMMAP", 0, size*size*2);
	//////////////////////////////////////////////////////////////

	/////////////////
	//now fill 'er up
	gfxVtx *ptrVtx, *thisVtx;

	

	if(MeshLockVtx(&newMdl->mesh, 0, &ptrVtx) != RETCODE_SUCCESS)
	{ return 0; }

	int maxV = size+1;

	//fill in data, just go through the lists and fill in stuff
	for(int rowV = 0; rowV < maxV; rowV++)
	{
		for(int colV = 0; colV < maxV; colV++)
		{
			thisVtx = &CELL(ptrVtx, rowV, colV, maxV);

			thisVtx->x = colV*mapSizeX;
			thisVtx->y = CELL(points, rowV, colV, maxV);
			thisVtx->z = rowV*mapSizeZ;

			_GFXVtxSetTxtCoordAll(thisVtx, thisVtx->x/TextureGetWidth(texture), 
										   thisVtx->z/TextureGetHeight(texture));
		}
	}

	MeshUnlockVtx(&newMdl->mesh);
	/////////////////

	/////////////////
	//now fill the index buffer up
	unsigned short *ptrInd;

	if(MeshLockInd(&newMdl->mesh, 0, 0, &ptrInd) != RETCODE_SUCCESS)
	{ return 0; }

	//fill in data
	int numCol = size; //number of col
	int numRow = size; //number of row
	int max = numCol+1;
	int numDot = MeshGetNumVtx(&newMdl->mesh)-max; //number of dots to iterate (minus last row)
	int indIter = NUMPTFACE*2; //we are going to make two faces per cel

	for(int row = 0, ind = 0; row < numRow; row++)
	{
		for(int col = 0; col < numCol; col++)
		{
			/*
			0-----3
			|\    |
			|  \  |
			|    \|
			1-----2
			*/

			int dot = col+(row*max);
			
			//first face
			ptrInd[ind] = dot; ptrInd[ind+1] = dot+max; ptrInd[ind+2] = ptrInd[ind+1]+1;

			//second face
			ptrInd[ind+3] = ptrInd[ind+2]; ptrInd[ind+4] = dot+1; ptrInd[ind+5] = ptrInd[ind];
			
			ind += indIter;
		}
	}

	MeshUnlockInd(&newMdl->mesh, 0);
	/////////////////

	//This outta make lighting easy
	MeshComputeNormals(&newMdl->mesh);

	//clean up stuff
	GFX_FREE(points);

	//append to list
	g_MDLLIST.insert(g_MDLLIST.end(), (unsigned int)newMdl);

	return newMdl;
}