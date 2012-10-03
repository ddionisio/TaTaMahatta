#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

D3DXVECTOR3	g_traceExt;	//trace extension
bool		g_bIsPt;	//is it a pt?
float		g_mins[eMaxPt], g_maxs[eMaxPt];
float		g_p[eMaxPt], g_dir[eMaxPt];
gfxTrace	*g_pTrace;
float		g_tMax;

/////////////////////////////////////
// Name:	QBSPCollBrush
// Purpose:	collide with brush
//			NOTE: pTrace->t must have
//			a value (usu. 1)!
// Output:	pTrace, if t < 1, then
//			you better do something
//			about it!
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPCollBrush(hQBSP qbsp, int brushIndex)
{
	if(VIS_IS_ON(qbsp->brushVis, brushIndex))
	{ return; }

	VIS_SET(qbsp->brushVis, brushIndex);

	//go through each half-space and determine the interval t[min,max]
	float tMin=0, tMax=1;

	float vDotn, pDotn, tInt;

	int plInd;

	float *pPlane, ofs[eMaxPt], dist;

	int indMin=0, indMax=0, j;

	//go through the sides
	int max =qbsp->brushes[brushIndex].brushSide+qbsp->brushes[brushIndex].numOfBrushSides;

	for(int i = qbsp->brushes[brushIndex].brushSide; i < max; i++)
	{
		if(tMin > tMax) //no intersect!
			break;

		pPlane = (float*)qbsp->planes[qbsp->brushSides[i].plane];

		if(!g_bIsPt)
		{	// general box case
			// push the plane out apropriately for mins/maxs

			for (j=0 ; j<eMaxPt ; j++)
			{
				if (pPlane[j] < 0)
					ofs[j] = g_maxs[j];
				else
					ofs[j] = g_mins[j];
			}
			dist = pPlane[eX]*ofs[eX] + pPlane[eY]*ofs[eY] + pPlane[eZ]*ofs[eZ];
			dist -= pPlane[ePD];//-((-pPlane[ePD]) - dist);
		}
		else
		{	// special point case
			dist = -pPlane[ePD];
		}

		//normal dot ray vector
		vDotn = pPlane[eX]*g_dir[eX] + pPlane[eY]*g_dir[eY] + pPlane[eZ]*g_dir[eZ];

		pDotn = pPlane[eX]*g_p[eX] + pPlane[eY]*g_p[eY] + pPlane[eZ]*g_p[eZ];

		if(vDotn < 0)
		{
			tInt = -(pDotn + dist - DIST_EPSILON)/vDotn;

			if (tInt > tMin) {
			  tMin = tInt;
			  indMin = i;
			}
			
		}
		else if(vDotn > 0)
		{
			tInt = -(pDotn + dist + DIST_EPSILON)/vDotn;

			if(tInt < tMax)
			{
				tMax = tInt;
				indMax = i;
			}

		}
		else
		{
			if(pDotn + dist > 0)
				tMin = tMax+1;
		}
	}

	//now let's see if we intersect something...
	if(tMin < tMax && tMin < g_pTrace->t)
	{
		g_pTrace->t = tMin;

		plInd = qbsp->brushSides[indMin].plane;

		g_pTrace->norm[eX] = qbsp->planes[plInd].a;
		g_pTrace->norm[eY] = qbsp->planes[plInd].b;
		g_pTrace->norm[eZ] = qbsp->planes[plInd].c;
		g_pTrace->d		 = qbsp->planes[plInd].d;

		g_pTrace->txtInd   = qbsp->brushSides[indMin].textureID;
		
		g_pTrace->bStartSolid = tMin < 0 ? true : false;
		g_pTrace->bAllSolid = (tMax < 0 && g_pTrace->bStartSolid) ? true : false;

		g_pTrace->brushIndex = brushIndex;
	}
	else
	{
		if(tMin < 0)
		{
			g_pTrace->bStartSolid = true;

			if(tMax < 0)
				g_pTrace->bAllSolid = true;

			g_pTrace->brushIndex = brushIndex;
		}
	}
}

PRIVATE void QBSPCollRecursive(hQBSP qbsp, int node, float p1f, float p2f, 
							   float p1[eMaxPt], float p2[eMaxPt])
{
	int side = 1;
	float t1, t2, d = 0.0f;
	float		frac, frac2;
	float		idist;
	float		mid[eMaxPt];
	float		midf;

	gfxBSPLeaf *pLeaf;

	if(g_pTrace->t <= p1f) //got something better already
		return;

	if(node < 0) //are we there yet?
	{
		pLeaf = &qbsp->leaves[~node];

		//now go through brushes
		int brushCount = pLeaf->numOfLeafBrushes;

		if(brushCount > 0)
		{
			float t = 1;

			// If the current leaf is not hit by ray
			if(g_bIsPt)
			{
				GFXIntersectBox(g_p, g_dir, pLeaf->min, pLeaf->max, &t);
			}
			else
			{
				//expand bounding box
				float bbMin[eMaxPt] = {pLeaf->min[eX]+g_mins[eX],pLeaf->min[eY]+g_mins[eY],pLeaf->min[eZ]+g_mins[eZ]};
				float bbMax[eMaxPt] = {pLeaf->max[eX]+g_maxs[eX],pLeaf->max[eY]+g_maxs[eY],pLeaf->max[eZ]+g_maxs[eZ]};

				GFXIntersectBox(g_p, g_dir, bbMin, bbMax, &t);
			}

			if(t == 1.0f)
				return;

			// If we get here, check for brushes
			// Loop through and check all of the brushes in this leaf
			while(brushCount--)
			{
				// Grab the current brush index from our leaf brushes array
				int brushIndex = qbsp->leafBrushes[pLeaf->leafBrush + brushCount];

				QBSPCollBrush(qbsp, brushIndex);
			}
		}

		return;
	}

	float *plane = (float*)qbsp->planes[qbsp->nodes[node].plane];

	t1 = p1[eX]*plane[eX] + p1[eY]*plane[eY] + p1[eZ]*plane[eZ] - plane[ePD];
	t2 = p2[eX]*plane[eX] + p2[eY]*plane[eY] + p2[eZ]*plane[eZ] - plane[ePD];

	if(!g_bIsPt)
		d = fabsf(g_traceExt.x*plane[eX]) +
			fabsf(g_traceExt.y*plane[eY]) +
			fabsf(g_traceExt.z*plane[eZ]);

	//see which sides we need to go
	if(t1 >= d && t2 >= d) //front
	{
		QBSPCollRecursive(qbsp, qbsp->nodes[node].child[0], p1f, p2f, p1, p2);
		return;
	}
	if(t1 < -d && t2 < -d) //back
	{
		QBSPCollRecursive(qbsp, qbsp->nodes[node].child[1], p1f, p2f, p1, p2);
		return;
	}

	//damn, that means we need to go through two path
	if (t1 < t2)
	{
		idist = 1.0f/(t1-t2);
		side = 1;
		frac2 = (t1 + d + DIST_EPSILON)*idist;
		frac = (t1 - d + DIST_EPSILON)*idist;
	}
	else if (t1 > t2)
	{
		idist = 1.0f/(t1-t2);
		side = 0;
		frac2 = (t1 - d - DIST_EPSILON)*idist;
		frac = (t1 + d + DIST_EPSILON)*idist;
	}
	else
	{
		side = 0;
		frac = 1;
		frac2 = 0;
	}

	if(frac < 0) frac = 0;
	else if(frac > 1) frac = 1;

	if(frac2 < 0) frac2 = 0;
	else if(frac2 > 1) frac2 = 1;

	//lerp to first fraction
	midf = p1f + (p2f - p1f)*frac;
	
	mid[eX] = p1[eX] + frac*(p2[eX] - p1[eX]);
	mid[eY] = p1[eY] + frac*(p2[eY] - p1[eY]);
	mid[eZ] = p1[eZ] + frac*(p2[eZ] - p1[eZ]);

	QBSPCollRecursive(qbsp, qbsp->nodes[node].child[side], p1f, midf, p1, mid);

	//lerp to second fraction
	midf = p1f + (p2f - p1f)*frac2;
	
	mid[eX] = p1[eX] + frac2*(p2[eX] - p1[eX]);
	mid[eY] = p1[eY] + frac2*(p2[eY] - p1[eY]);
	mid[eZ] = p1[eZ] + frac2*(p2[eZ] - p1[eZ]);

	QBSPCollRecursive(qbsp, qbsp->nodes[node].child[!side], midf, p2f, mid, p2);
}

/////////////////////////////////////
// Name:	QBSPCollision
// Purpose:	find collision with the given
//			map.  Will collide if 't' is
//			within [0,1)
//			NOTE: testT is the starting
//				  't' value (starting minimum)
//				  usu. set as 't = 1'
//				  startLeafIndex is index 
//				  in leaf (-1 to rescan).
// Output:	pTrace filled
// Return:	true if collide
/////////////////////////////////////
PUBLIC void QBSPCollision(hQBSP qbsp, float mins[eMaxPt], float maxs[eMaxPt],
						  float pt1[eMaxPt], float pt2[eMaxPt], gfxTrace *pTrace,
						  float testT, int startLeafIndex)
{
	QBSPVisBitClearAll(qbsp->brushVis, qbsp->numBrushVis);

	g_pTrace = pTrace;

	memset(g_pTrace, 0, sizeof(gfxTrace));
	g_pTrace->t = testT;

	g_tMax = testT;

	g_dir[eX] = pt2[eX]-pt1[eX];
	g_dir[eY] = pt2[eY]-pt1[eY];
	g_dir[eZ] = pt2[eZ]-pt1[eZ];

	memcpy(g_p, pt1, sizeof(g_p));

	//determine if we are colliding with bbox or pt.
	g_bIsPt = (mins && maxs) ? false : true;

	//create trace extend

	if(!g_bIsPt)
	{
		memcpy(g_mins, mins, sizeof(g_mins));
		memcpy(g_maxs, maxs, sizeof(g_maxs));

		g_traceExt.x = -mins[eX] > maxs[eX] ? -mins[eX] : maxs[eX];
		g_traceExt.y = -mins[eY] > maxs[eY] ? -mins[eY] : maxs[eY];
		g_traceExt.z = -mins[eZ] > maxs[eZ] ? -mins[eZ] : maxs[eZ];
	}

	//go through the bsp tree
	QBSPCollRecursive(qbsp, 0, 0, testT, pt1, pt2);
}