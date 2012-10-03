#ifndef _tata_waypoint_h
#define _tata_waypoint_h

#include "tata_ID.h"
#include "tata_basic.h"

//Waypoint interface

typedef struct _wayPt {
	int			ind;
	double		delay;	//delay for pausing
	D3DXVECTOR3	pt;
	D3DXVECTOR3 target;
} wayPt;

//waypoint list
class Waypoint : public GameID, public Name {
public:
	Waypoint(const char *name) : GameID(this), Name(name) {}
	~Waypoint() {}

	int GetNumWayPt();					//get the number of waypoints

	void AddWayPt(const wayPt & pt);	//add waypoint depending on ind

	void SetWayPt(const wayPt & pt, int ind); //NOTE: make sure to use GetWayPt(ind) first
	wayPt GetWayPt(int ind);

private:
	vector<wayPt> m_wayPts;				//the waypoints
};

//use this to go through the waypoint list
class WayptNode {
public:
	WayptNode(const Id & wayptID);
	~WayptNode();

	void GetCurrentIndLoc(D3DXVECTOR3 *ptOut); //grabs the current index loc

	float GetCurrentLinearLoc(D3DXVECTOR3 *ptOut, D3DXVECTOR3 *targetOut=0); //optional: target
	float GetCurrentCurvedLoc(D3DXVECTOR3 *ptOut, D3DXVECTOR3 *targetOut=0); //optional: target

	//set waypoint to next node
	void GotoNext();

	//set the current waypoint to the previous
	void SetToPrev();

	//set the next waypoint immediately
	void SetNext();

	bool Update(); //update path to waypoints, return true if end reached

	eWayPtMode GetMode() { return m_mode; }
	void SetMode(eWayPtMode mode) { m_mode = mode; }

	void Reverse(bool bYes) { m_bReverse = bYes; }

	bool IsReverse() { return m_bReverse; }

	void SetDelaySpd(double delay);

	bool IsDone();

protected:
	void _MoveNext();

private:
	Id			m_wayptID;			//the waypoint list reference
	int			m_wayptInd;			//current waypoint index
	int			m_wayptIndNext;		//next waypoint index

	eWayPtMode  m_mode;				//waypoint mode (WAYPT_STOPATEND, WAYPT_REPEAT, WAYPT_BACKTRACE)

	bool		m_bReverse;			//are we going reverse?

	win32Time	m_delaySpd;			//delay speed per waypoint
	win32Time	m_delayWait;		//delay wait per waypoint

	double		m_t;				//current time interval between current waypt and next waypt (0-1)

	void SetDelayWait(double delay);
};

//waypoint data if you want a waypoint manager
//mostly used by level objects and creatures
class WaypointDat {
public:
	WaypointDat();
	~WaypointDat();

	//removes current waypoint
	void RemoveWayPt();

	//set waypoint, this will remove the previous waypoint
	void SetWayPt(const Id & wayPtID);

	//grabs the waypoint interface, only use this to set stuff up,
	//don't delete
	WayptNode  *GetWayPoint() { return m_pWayPt; }

protected:
	WayptNode  *m_pWayPt;			//the path (automatically updated if not null)
};

#endif