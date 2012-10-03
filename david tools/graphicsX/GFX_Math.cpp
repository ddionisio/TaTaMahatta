#include "gdix.h"
#include "gdix_i.h"

//this is where all math related stuff happens

//return a random double-precision number between 0 and 1
PROTECTED double _GFXMathDRand()
{
	//make sure all bytes of the int are random
	unsigned int i = rand() + (rand() << 15); 
	unsigned int j = rand() + (rand() << 15); 
	double k = (double)i / (double)(unsigned int)(0x3fffffff) + (double)j / (double)(unsigned int)(0x3fffffff) / (double)(unsigned int)(0x3fffffff);

	//return a number between 0 and 1...
	return k;
}

PROTECTED float _GFXMathRand(float a, float b)
{
	int r;
    float	x;
    
	r = rand ();
    x = (float)(r & 0x7fff) /
		(float)0x7fff;
    return (x * (b - a) + a);
}

PROTECTED void _GFXMathMtxLoadIden(gfxWrldMtx *pWMtx)
{
	D3DXMatrixIdentity(&pWMtx->mtxWrld);
	D3DXMatrixIdentity(&pWMtx->mtxScale);
	D3DXMatrixIdentity(&pWMtx->mtxRot);
	D3DXMatrixIdentity(&pWMtx->mtxTrans);
}

//change later so it's not copyrighted
PROTECTED bool _GFXMathIsPowerOf2(int size)
{
    int i, bitcount = 0;

    /* Note this code assumes that (sizeof(int)*8) will yield the
       number of bits in an int. Should be portable to most
       platforms. */
    for (i=0; i<sizeof(int)*8; i++)
	if (size & (1<<i))
	    bitcount++;
    if (bitcount == 1)
	/* One bit. Must be a power of 2. */
	return true;
    else
	/* either size==0, or size not a power of 2. Sorry, Charlie. */
	return false;
}

/////////////////////////////////////
// Name:	GFXMathCreateRayFromLoc
// Purpose:	create a ray from given 
//			screen loc
// Output:	ray pos&dir is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXMathCreateRayFromLoc(const LOCATION *loc,
										D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir)
{
	D3DXMATRIX mtxProj;
    g_p3DDevice->GetTransform( D3DTS_PROJECTION, &mtxProj );

    //Compute the vector of the pick ray in screen space
    D3DXVECTOR3 v;
	SIZE s=GFXGetScrnSize();
    v.x =  (((2.0f*loc->x)/s.cx)-1)/mtxProj._11;
    v.y = -(((2.0f*loc->y)/s.cy)-1)/mtxProj._22;
    v.z =  1.0f;

    // Get the inverse view matrix
    D3DXMATRIX mtxView, m;
    g_p3DDevice->GetTransform( D3DTS_VIEW, &mtxView );
    D3DXMatrixInverse( &m, NULL, &mtxView );

    // Transform the screen space pick ray into 3D space
    pRayDir->x = v.x*m._11 + v.y*m._21 + v.z*m._31;
    pRayDir->y = v.x*m._12 + v.y*m._22 + v.z*m._32;
    pRayDir->z = v.x*m._13 + v.y*m._23 + v.z*m._33;
    pRayPos->x = m._41;
    pRayPos->y = m._42;
    pRayPos->z = m._43;
}

//create the world matrix inside gfxWrldMtx
//wMtx->mtxWrld = scale*rot*trans
PROTECTED void _GFXCreateWrld(gfxWrldMtx *wMtx)
{
	wMtx->mtxWrld._11 = wMtx->mtxScale._11*wMtx->mtxRot._11;
	wMtx->mtxWrld._21 = wMtx->mtxScale._11*wMtx->mtxRot._21;
	wMtx->mtxWrld._31 = wMtx->mtxScale._11*wMtx->mtxRot._31;
	wMtx->mtxWrld._41 = wMtx->mtxTrans._41;

    wMtx->mtxWrld._12 = wMtx->mtxScale._22*wMtx->mtxRot._12;
	wMtx->mtxWrld._22 = wMtx->mtxScale._22*wMtx->mtxRot._22;
	wMtx->mtxWrld._32 = wMtx->mtxScale._22*wMtx->mtxRot._32;
	wMtx->mtxWrld._42 = wMtx->mtxTrans._42;

    wMtx->mtxWrld._13 = wMtx->mtxScale._33*wMtx->mtxRot._13;
	wMtx->mtxWrld._23 = wMtx->mtxScale._33*wMtx->mtxRot._23;
	wMtx->mtxWrld._33 = wMtx->mtxScale._33*wMtx->mtxRot._33;
	wMtx->mtxWrld._43 = wMtx->mtxTrans._43;

    wMtx->mtxWrld._14 = 0;
	wMtx->mtxWrld._24 = 0;
	wMtx->mtxWrld._34 = 0;
	wMtx->mtxWrld._44 = 1;
}

#define EPSILON 1.0e-10 //0.03125f
#define CLOSE_TOLERANCE 1.0e-6
#define DEPTH_TOLERANCE 1.0e-6

//normals for the box
float	g_boxNorm[6][eMaxPt] =
{{-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1}};

/////////////////////////////////////
// Name:	GFXIntersectBox
// Purpose:	not really related to
//			anything Graphics but nice
//			to have.  Intersect given
//			ray to axis aligned bound box.
// Output:	tOut and nOut filled
// Return:	true if tOut < 1
/////////////////////////////////////
PUBLIC bool GFXIntersectBox(float p[eMaxPt], float dir[eMaxPt], 
				  float boxMin[eMaxPt], float boxMax[eMaxPt],
				  float *tOut, float nOut[eMaxPt])
{
	float t, tmin, tmax;
	int minSide=0, maxSide=0;

	tmin = 0.0;
	//tmax = 100000000;
	tmax = 1;

	/*
	* Sides first.
	*/

	if (dir[eX] < -EPSILON)
	{
		t = (boxMin[eX] - p[eX]) / dir[eX];

		if (t < tmin) return false;

		if (t <= tmax)
		{
			maxSide = 0;
			tmax = t;
		}

		t = (boxMax[eX] - p[eX]) / dir[eX];

		if (t >= tmin)
		{
			if (t > tmax) return false;

			minSide = 1;
			tmin = t;
		}
	}
	else
	{
		if (dir[eX] > EPSILON)
		{
			t = (boxMax[eX] - p[eX]) / dir[eX];

			if (t < tmin) return false;

			if (t <= tmax)
			{
				maxSide = 1;
				tmax = t;
			}

			t = (boxMin[eX] - p[eX]) / dir[eX];

			if (t >= tmin)
			{
				if (t > tmax) return false;

				minSide = 0;
				tmin = t;
			}
		}
		else
		{
			if ((p[eX] < boxMin[eX]) || (p[eX] > boxMax[eX]))
			{
				return false;
			}
		}
	}

	/*
	* Check Top/Bottom.
	*/

	if (dir[eY] < -EPSILON)
	{
		t = (boxMin[eY] - p[eY]) / dir[eY];

		if (t < tmin) return false;

		if (t <= tmax - CLOSE_TOLERANCE)
		{
			maxSide = 2;
			tmax = t;
		}

		t = (boxMax[eY] - p[eY]) / dir[eY];

		if (t >= tmin + CLOSE_TOLERANCE)
		{
			if (t > tmax) return false;

			minSide = 3;
			tmin = t;
		}
	}
	else
	{
		if (dir[eY] > EPSILON)
		{
			t = (boxMax[eY] - p[eY]) / dir[eY];

			if (t < tmin) return false;

			if (t <= tmax - CLOSE_TOLERANCE)
			{
				maxSide = 3;
				tmax = t;
			}

			t = (boxMin[eY] - p[eY]) / dir[eY];

			if (t >= tmin + CLOSE_TOLERANCE)
			{
				if (t > tmax) return false;

				minSide = 2;
				tmin = t;
			}
		}
		else
		{
			if ((p[eY] < boxMin[eY]) || (p[eY] > boxMax[eY]))
			{
				return false;
			}
		}
	}

	/* Now front/back */

	if (dir[eZ] < -EPSILON)
	{
		t = (boxMin[eZ] - p[eZ]) / dir[eZ];

		if (t < tmin) return false;

		if (t <= tmax - CLOSE_TOLERANCE)
		{
			maxSide = 4;
			tmax = t;
		}

		t = (boxMax[eZ] - p[eZ]) / dir[eZ];

		if (t >= tmin + CLOSE_TOLERANCE)
		{
			if (t > tmax) return false;

			minSide = 5;
			tmin = t;
		}
	}
	else
	{
		if (dir[eZ] > EPSILON)
		{
			t = (boxMax[eZ] - p[eZ]) / dir[eZ];

			if (t < tmin) return false;

			if (t <= tmax - CLOSE_TOLERANCE)
			{
				maxSide = 5;
				tmax = t;
			}

			t = (boxMin[eZ] - p[eZ]) / dir[eZ];

			if (t >= tmin + CLOSE_TOLERANCE)
			{
				if (t > tmax) return false;

				minSide = 4;
				tmin = t;
			}
		}
		else
		{
			if ((p[eZ] < boxMin[eZ]) || (p[eZ] > boxMax[eZ]))
			{
				return false;
			}
		}
	}

	if (tmax < DEPTH_TOLERANCE)
	{
		return false;
	}

	//if(minSide>= 0)
	{
		*tOut = tmin;

		if(nOut)
			memcpy(nOut, g_boxNorm[minSide], sizeof(float)*eMaxPt);
	}
	//else
	//{
	//	*tOut = tmax;

		//if(nOut)
			//memcpy(nOut, g_boxNorm[maxSide], sizeof(float)*eMaxPt);
	//}


	return true;
}
