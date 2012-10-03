#ifndef _tata_bound_h
#define _tata_bound_h

//This includes all interface with the bounds

class Bound {
public:
	Bound() : m_radius(0), m_min(0,0,0), m_max(0,0,0), m_modelInd(-1), m_planes(0), m_tPlanes(0) {}
	Bound(float radius, const D3DXVECTOR3 & min, const D3DXVECTOR3 & max) : m_radius(radius), m_min(min), m_max(max), m_modelInd(-1), m_planes(0), m_tPlanes(0) {}
	~Bound();

	void BoundSetMin(const D3DXVECTOR3 & pt) { m_min = pt; }
	void BoundSetMax(const D3DXVECTOR3 & pt) { m_max = pt; }
	void BoundSetRadius(float r) { m_radius = r; }

	D3DXVECTOR3 BoundGetMin() const { return m_min; }
	D3DXVECTOR3 BoundGetMax() const  { return m_max; }
	float BoundGetRadius() const  { return m_radius; }

	//collide a ray with given bound to this bound,
	//thisPt is the center of this bound.
	//NOTE: THIS WILL NOT MODIFY pt and vel, it's not 'const'
	//      for some reason...
	//testT is the 't' comparison for testing if we already
	//hit a minimum value.
	//NOTE: you can set qbsp to 0 to force box collision
	bool BoundCollide(hQBSP qbsp, const D3DXVECTOR3 & thisPt, const Bound & otherBound,
			D3DXVECTOR3 & pt, D3DXVECTOR3 & vel, gfxTrace *pTrace,
			float testT = 1);

	bool BoundCollide(hQBSP qbsp, const D3DXVECTOR3 & thisPt,
						 D3DXVECTOR3 & mins, D3DXVECTOR3 & maxs,
						 D3DXVECTOR3 & pt, D3DXVECTOR3 & vel, 
						 gfxTrace *pTrace, float testT = 1);

	//this will only check if sphere intersects
	//thisPt is the center of this bound and pt is 
	//the center of other bound.
	bool BoundSphereCollide(const D3DXVECTOR3 & thisPt, const Bound & otherBound,
		const D3DXVECTOR3 & pt);

	bool BoundSphereCollide(const D3DXVECTOR3 & thisPt, const D3DXVECTOR3 & pt, 
		float r);

	//AABB to AABB collide check
	bool BoundBoxCheck(const D3DXVECTOR3 & thisPt, const Bound & otherBound,
		const D3DXVECTOR3 & pt);

	bool BoundBoxCheck(const D3DXVECTOR3 & thisPt, const D3DXVECTOR3 & pt,
		const D3DXVECTOR3 & min, const D3DXVECTOR3 & max);

	//set the model index (-1 to disable)
	//should only be used when loading level objects!
	void BoundSetModelInd(int ind) { m_modelInd = ind; }
	int  BoundGetModelInd() { return m_modelInd; }

	//important!  Use this for all objects who don't use model index!!!
	//NOTE: SET THE BOUND BOX BEFORE CALLING THIS!!!
	void BoundGeneratePlanes();

	void BoundTranslate(const D3DXVECTOR3 & p);

	void BoundDestroyPlanes();
private:
	float m_radius;						//the radius
	D3DXVECTOR3 m_min,m_max;	//bounding box min/max

	D3DXVECTOR3 m_pt;

	D3DXPLANE	*m_planes;		//if we are not using model index (6 planes)
	D3DXPLANE	*m_tPlanes;		//transformation planes

	//the model index inside the world level (-1 if using m_obj)
	//this is only used by level objects!!!!
	int			m_modelInd;

	void _CollBrush(const D3DXVECTOR3 & mins, const D3DXVECTOR3 & maxs,
		D3DXVECTOR3 & pt, D3DXVECTOR3 & vel, gfxTrace *pTrace);
};

#endif