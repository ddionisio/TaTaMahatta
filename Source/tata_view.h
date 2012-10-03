#ifndef _tata_view_h
#define _tata_view_h

//3D camera view interface

class View {
public:
	View(const D3DXVECTOR3 &pt, const D3DXVECTOR3 &target);
	View();
	View(const View & other);
	View & operator =(const View & other);
	~View();

	bool ViewUpdate();			//update to get to the next pt/target (returns true if the view did move)

	bool ViewIsDone();	//is the view done moving?  Find out by calling this function!

	void ViewSetPt(const D3DXVECTOR3 &pt);
	void ViewSetTarget(const D3DXVECTOR3 &target);

	void ViewSetNewPt(const D3DXVECTOR3 &pt) { m_pt = m_ptNext = pt; } //set and don't move to new pt.
	void ViewSetNewTarget(const D3DXVECTOR3 &target) { m_target = m_targetNext = target; } //set and don't move to new pt.

	D3DXVECTOR3 ViewGetPt() { return m_pt; }
	D3DXVECTOR3 ViewGetTarget() { return m_target; }

	void ViewSetDelay(double delay);	//set the delay to get to the next pt.
	
	void ViewSetScene();				//set the 3D scene to use this view

	void ViewSetListener();				//set the sound listener to view's location

	void ViewCollideMap(hQBSP qbsp);	//collide view to the given map

	hCAMERA ViewGetCam() { return m_cam; }
private:
	D3DXVECTOR3 m_pt;		//current pt
	D3DXVECTOR3 m_target;	//current target

	D3DXVECTOR3 m_ptNext;		//next pt
	D3DXVECTOR3 m_targetNext;	//next target

	win32Time	m_delay;	//delay to get to next pt/target

	double		m_t;

	hCAMERA		m_cam;
};

#endif