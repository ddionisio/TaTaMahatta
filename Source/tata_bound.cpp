#include "tata_main.h"

#include "tata_bound.h"

#define BOUND_DIST_EPSILON  0.03125f
#define MAX_PLANE_SIDE		6

Bound::~Bound() 
{
	BoundDestroyPlanes();
}

void Bound::_CollBrush(const D3DXVECTOR3 & mins,
					   const D3DXVECTOR3 & maxs,
		D3DXVECTOR3 & pt, D3DXVECTOR3 & vel, gfxTrace *pTrace)
{
	int txtInd = -1;

	//go through each half-space and determine the interval t[min,max]
	float tMin=0, tMax=1;

	float vDotn, pDotn, tInt;

	float *pPlane, ofs[eMaxPt],ofs2[eMaxPt], dist;

	int indMin=0, indMax=0, j;

	//go through the sides
	for(int i = 0; i < MAX_PLANE_SIDE; i++)
	{
		if(tMin > tMax) //no intersect!
			break;

		pPlane = (float*)m_tPlanes[i];

		if(mins != maxs)
		{	// general box case
			// push the plane out apropriately for mins/maxs

			for (j=0 ; j<eMaxPt ; j++)
			{
				if (pPlane[j] < 0)
				{
					dist = pPlane[j]*maxs[j] - (pPlane[j]*(m_min[j] + m_pt[j]));

					break;
				}
				else if(pPlane[j] > 0)
				{
					dist = pPlane[j]*mins[j] - (pPlane[j]*(m_max[j] + m_pt[j]));

					break;
				}
			}

			/*for (j=0 ; j<eMaxPt ; j++)
			{
				if (pPlane[j] > 0)
					ofs2[j] = m_max[j];// + [j];
				else
					ofs2[j] = m_min[j];// + [j];
			}

			dist = pPlane[eX]*ofs[eX] + pPlane[eY]*ofs[eY] + pPlane[eZ]*ofs[eZ];
			dist -= m_pt.x*pPlane[eX] + m_pt.y*pPlane[eY] + m_pt.z*pPlane[eZ];*/
			//dist += pPlane[ePD];//-((-pPlane[ePD]) - dist);
			//dist -= (m_pt.x+ofs2[eX])*pPlane[eX] + (m_pt.y+ofs2[eY])*pPlane[eY] + (m_pt.z+ofs2[eZ])*pPlane[eZ];
		}
		else
		{	// special point case
			dist = pPlane[ePD] - (m_pt.x*pPlane[eX] + m_pt.y*pPlane[eY] + m_pt.z*pPlane[eZ]);
		}

		//normal dot ray vector
		vDotn = pPlane[eX]*vel.x + pPlane[eY]*vel.y + pPlane[eZ]*vel.z;

		pDotn = pPlane[eX]*pt.x + pPlane[eY]*pt.y + pPlane[eZ]*pt.z;

		if(vDotn < 0)
		{
			tInt = -(pDotn + dist - BOUND_DIST_EPSILON)/vDotn;

			if (tInt > tMin) {
			  tMin = tInt;
			  indMin = i;
			}
			
		}
		else if(vDotn > 0)
		{
			tInt = -(pDotn + dist + BOUND_DIST_EPSILON)/vDotn;

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
	if(tMin < tMax && tMin < pTrace->t)
	{
		pTrace->t = tMin;

		pTrace->norm[eX] = m_tPlanes[indMin].a;
		pTrace->norm[eY] = m_tPlanes[indMin].b;
		pTrace->norm[eZ] = m_tPlanes[indMin].c;
		pTrace->d		 = m_tPlanes[indMin].d;

		pTrace->txtInd   = -1;
		
		pTrace->bStartSolid = tMin < 0 ? true : false;
		pTrace->bAllSolid = (tMax < 0 && pTrace->bStartSolid) ? true : false;

		pTrace->brushIndex = -1;
	}
	else
	{
		if(tMin < 0)
		{
			pTrace->bStartSolid = true;

			if(tMax < 0)
				pTrace->bAllSolid = true;

			pTrace->brushIndex = -1;
		}
	}
}

//collide a ray with given bound to this bound,
//thisPt is the center of this bound.
//NOTE: THIS WILL NOT MODIFY pt and vel, it's not 'const'
//      for some reason...
//testT is the 't' comparison for testing if we already
//hit a minimum value.
//NOTE: you can set qbsp to 0 to force box collision
bool Bound::BoundCollide(hQBSP qbsp, const D3DXVECTOR3 & thisPt, const Bound & otherBound,
		D3DXVECTOR3 & pt, D3DXVECTOR3 & vel, gfxTrace *pTrace,
		float testT)
{
	D3DXVECTOR3 minOfs = thisPt + m_min + otherBound.m_min;
	D3DXVECTOR3 maxOfs = thisPt + m_max + otherBound.m_max;

	float t, norm[eMaxPt];

	if(BoundGetModelInd() != -1 && qbsp)
	{
		QBSPModelCollision(qbsp, BoundGetModelInd(), 
				  (float*)otherBound.BoundGetMin(), 
				  (float*)otherBound.BoundGetMax(),
				  (float*)pt,
				  (float*)(pt+vel), 
				  pTrace, testT);
	}
	else if(m_tPlanes && GFXIntersectBox((float*)pt, (float*)vel, (float*)minOfs, (float*)maxOfs, &t, norm))//&& t < 1)
	{
		_CollBrush(otherBound.m_min, otherBound.m_max, pt, vel, pTrace);
	}
	else if(BoundBoxCheck(thisPt, otherBound, pt))
	{
		pTrace->txtInd = -1;
		pTrace->brushIndex = -1;
		pTrace->leafIndex = -1;
		pTrace->d = 0;
		pTrace->t = 0;
		memset(pTrace->norm, 0, sizeof(pTrace->norm));

		pTrace->bStartSolid = true;

		return true;
	}

	if(pTrace->t < testT)
		return true;

	return false;
}

bool Bound::BoundCollide(hQBSP qbsp, const D3DXVECTOR3 & thisPt,
						 D3DXVECTOR3 & mins, D3DXVECTOR3 & maxs,
						 D3DXVECTOR3 & pt, D3DXVECTOR3 & vel, 
						 gfxTrace *pTrace, float testT)
{
	D3DXVECTOR3 minOfs = thisPt + m_min + mins;
	D3DXVECTOR3 maxOfs = thisPt + m_max + maxs;

	float t, norm[eMaxPt];

	if(BoundGetModelInd() != -1 && qbsp)
	{
		QBSPModelCollision(qbsp, BoundGetModelInd(), 
				  (float*)mins, 
				  (float*)maxs,
				  (float*)pt,
				  (float*)(pt+vel), 
				  pTrace, testT);
	}
	else if(m_tPlanes && GFXIntersectBox((float*)pt, (float*)vel, (float*)minOfs, (float*)maxOfs, &t, norm) && t < 1)
	{
		_CollBrush(mins, maxs, pt, vel, pTrace);
	}
	
	if(pTrace->t < testT)
		return true;

	return false;
}

//this will only check if sphere intersects
//thisPt is the center of this bound and pt is 
//the center of other bound.
bool Bound::BoundSphereCollide(const D3DXVECTOR3 & thisPt, const Bound & otherBound,
							   const D3DXVECTOR3 & pt)
{
	D3DXVECTOR3 d;
	float R = m_radius + otherBound.m_radius;

	d = thisPt - pt;

	float lSqr = D3DXVec3LengthSq(&d);

	if(lSqr <= R*R )
		return true;

	return false;
}

//AABB to AABB collide check
bool Bound::BoundBoxCheck(const D3DXVECTOR3 & thisPt, const Bound & otherBound, 
						  const D3DXVECTOR3 & pt)
{
	//just check to see if pt. is in bound box
	D3DXVECTOR3 minOfs = thisPt + m_min + otherBound.m_min;
	D3DXVECTOR3 maxOfs = thisPt + m_max + otherBound.m_max;

	if(pt.x > minOfs.x && pt.x < maxOfs.x
		&& pt.y > minOfs.y && pt.y < maxOfs.y
		&& pt.z > minOfs.z && pt.z < maxOfs.z)
		return true;

	return false;
}

//this will only check if sphere intersects
//thisPt is the center of this bound and pt is 
//the center of other bound.
bool Bound::BoundSphereCollide(const D3DXVECTOR3 & thisPt, 
							   const D3DXVECTOR3 & pt, float r)
{
	D3DXVECTOR3 d;
	float R = m_radius + r;

	d = thisPt - pt;

	float lSqr = D3DXVec3LengthSq(&d);

	if(lSqr <= R*R )
		return true;

	return false;
}

//AABB to AABB collide check
bool Bound::BoundBoxCheck(const D3DXVECTOR3 & thisPt, const D3DXVECTOR3 & pt,
						  const D3DXVECTOR3 & min, const D3DXVECTOR3 & max)
{
	//just check to see if pt. is in bound box
	D3DXVECTOR3 minOfs = thisPt + m_min + min;
	D3DXVECTOR3 maxOfs = thisPt + m_max + max;

	if(pt.x > minOfs.x && pt.x < maxOfs.x
		&& pt.y > minOfs.y && pt.y < maxOfs.y
		&& pt.z > minOfs.z && pt.z < maxOfs.z)
		return true;

	return false;
}

void Bound::BoundGeneratePlanes()
{
	BoundDestroyPlanes();

	m_planes  = new D3DXPLANE[MAX_PLANE_SIDE];
	m_tPlanes = new D3DXPLANE[MAX_PLANE_SIDE];

	//m_min,m_max
	D3DXVECTOR3 pt,n;

	//front
	pt.x = 0; pt.y = 0; pt.z = 1;//m_min.z;
	n.x = 0; n.y = 0; n.z = -1;
	D3DXPlaneFromPointNormal(&m_planes[0], &pt, &n);

	//back
	pt.x = 0; pt.y = 0; pt.z = 0;//m_max.z;
	n.x = 0; n.y = 0; n.z = 1;
	D3DXPlaneFromPointNormal(&m_planes[1], &pt, &n);

	//left
	pt.y = 0; pt.z = 0; pt.x = 0;//m_min.x;
	n.y = 0; n.z = 0; n.x = -1;
	D3DXPlaneFromPointNormal(&m_planes[2], &pt, &n);

	//right
	pt.y = 0; pt.z = 0; pt.x = 0;//m_max.x;
	n.y = 0; n.z = 0; n.x = 1;
	D3DXPlaneFromPointNormal(&m_planes[3], &pt, &n);

	//top
	pt.x = 0; pt.z = 0; pt.y = 0;//m_max.y;
	n.x = 0; n.z = 0; n.y = 1;
	D3DXPlaneFromPointNormal(&m_planes[4], &pt, &n);

	//bottom
	pt.x = 0; pt.z = 0; pt.y = 0;//m_min.y;
	n.x = 0; n.z = 0; n.y = -1;
	D3DXPlaneFromPointNormal(&m_planes[5], &pt, &n);

	memcpy(m_tPlanes, m_planes, sizeof(D3DXPLANE)*MAX_PLANE_SIDE);
}

void Bound::BoundTranslate(const D3DXVECTOR3 & p)
{
	m_pt = p;
	/*
	if(m_planes && m_tPlanes)
	{
		for(int i = 0; i < MAX_PLANE_SIDE; i++)
		{
			m_tPlanes[i].d = m_planes[i].d - (m_planes[i].a*p.x + m_planes[i].b*p.y + m_planes[i].c*p.z);
		}
	}*/
}

void Bound::BoundDestroyPlanes()
{
	if(m_planes)
	{ delete [] m_planes; m_planes = 0; }

	if(m_tPlanes)
	{ delete [] m_tPlanes; m_tPlanes = 0; }
}