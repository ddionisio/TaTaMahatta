#include "tata_main.h"
#include "tata_waypoint.h"

//The Waypoint List Interface

int Waypoint::GetNumWayPt()
{
	return m_wayPts.size();
}

void Waypoint::AddWayPt(const wayPt & pt)
{
	//append the waypoint and sort
	m_wayPts.push_back(pt);

	//simple bubble sort
	for(int i = 0; i < m_wayPts.size(); i++)
	{
		for(int j = m_wayPts.size()-1; j > i; j--)
		{
			if(m_wayPts[j-1].ind > m_wayPts[j].ind)
			{
				wayPt tmp = m_wayPts[j-1];
				m_wayPts[j-1] = m_wayPts[j];
				m_wayPts[j] = tmp;
			}
		}
	}

	//done
}

void Waypoint::SetWayPt(const wayPt & pt, int ind)
{
	m_wayPts[ind] = pt;
}

wayPt Waypoint::GetWayPt(int ind)
{
	assert(ind < m_wayPts.size()); //better not be!

	return m_wayPts[ind];
}

WayptNode::WayptNode(const Id & wayptID)
: m_wayptID(wayptID), m_wayptInd(0), m_wayptIndNext(0), m_mode(WAYPT_STOPATEND), m_bReverse(false), m_t(0)
{
	SetDelaySpd(0);
	SetDelayWait(0);
}

WayptNode::~WayptNode()
{
}

void WayptNode::GetCurrentIndLoc(D3DXVECTOR3 *ptOut) //grabs the current index loc
{
	Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

	if(pWayPt)
	{
		wayPt pt1 = pWayPt->GetWayPt(m_wayptInd);

		(*ptOut) = pt1.pt;
	}
}

float WayptNode::GetCurrentLinearLoc(D3DXVECTOR3 *ptOut, D3DXVECTOR3 *targetOut)
{
	Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

	if(pWayPt)
	{
		wayPt pt1 = pWayPt->GetWayPt(m_wayptInd), pt2 = pWayPt->GetWayPt(m_wayptIndNext);

		D3DXVec3Lerp(ptOut, &pt1.pt, &pt2.pt, m_t);

		if(targetOut)
			D3DXVec3Lerp(targetOut, &pt1.target, &pt2.target, m_t);
	}

	return m_t;
}

float WayptNode::GetCurrentCurvedLoc(D3DXVECTOR3 *ptOut, D3DXVECTOR3 *targetOut)
{
	Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

	if(pWayPt)
	{
		int prevInd = m_wayptInd - 1; if(prevInd < 0) prevInd = 0;
		int next2Ind = m_wayptIndNext + 1; if(next2Ind == pWayPt->GetNumWayPt()) next2Ind = pWayPt->GetNumWayPt()-1;

		wayPt pt0 = pWayPt->GetWayPt(prevInd), pt1 = pWayPt->GetWayPt(m_wayptInd), 
			pt2 = pWayPt->GetWayPt(m_wayptIndNext), pt3 = pWayPt->GetWayPt(next2Ind);

		D3DXVec3CatmullRom(ptOut, &pt0.pt, &pt1.pt, &pt2.pt, &pt3.pt, m_t);

		if(targetOut)
			D3DXVec3CatmullRom(targetOut, &pt0.target, &pt1.target, &pt2.target, &pt3.target, m_t);
			//D3DXVec3Lerp(targetOut, &pt1.target, &pt2.target, m_t);
	}

	return m_t;
}

void WayptNode::_MoveNext()
{
	m_t = 0;

	Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

	if(pWayPt)
	{
		if(m_bReverse)
			m_wayptIndNext--;
		else
			m_wayptIndNext++;

		if(m_wayptIndNext < 0 || m_wayptIndNext == pWayPt->GetNumWayPt())
		{
			switch(m_mode)
			{
			case WAYPT_STOPATEND:
				m_wayptIndNext = m_wayptInd;
				break;
			case WAYPT_REPEAT:
				if(m_wayptIndNext < 0)
					m_wayptIndNext = pWayPt->GetNumWayPt() - 1;
				else
					m_wayptIndNext = 0;
				break;
			case WAYPT_BACKTRACE:
				m_wayptIndNext = m_wayptInd;
				m_bReverse     = !m_bReverse;
				break;
			}
		}
	}
}

void WayptNode::GotoNext()
{
	if(m_wayptInd != m_wayptIndNext)
	{
		m_wayptInd = m_wayptIndNext;

		Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

		if(pWayPt)
		{
			SetDelayWait(pWayPt->GetWayPt(m_wayptInd).delay);
		}
	}
	else if(m_wayptInd == m_wayptIndNext)
	{
		if(TimeElapse(&m_delayWait))
			_MoveNext();
	}
}

//set the current waypoint to the previous
void WayptNode::SetToPrev()
{
	m_wayptIndNext = m_wayptInd;

	m_t = 0;

	Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

	if(pWayPt)
	{
		if(m_bReverse)
			m_wayptInd++;
		else
			m_wayptInd--;

		if(m_wayptInd < 0 || m_wayptInd == pWayPt->GetNumWayPt())
		{
			switch(m_mode)
			{
			case WAYPT_STOPATEND:
				if(m_wayptInd < 0)
					m_wayptInd = 0;
				else
					m_wayptInd = pWayPt->GetNumWayPt() - 1;
				break;
			case WAYPT_REPEAT:
				if(m_wayptInd < 0)
					m_wayptInd = pWayPt->GetNumWayPt() - 1;
				else
					m_wayptInd = 0;
				break;
			case WAYPT_BACKTRACE:
				m_wayptInd = m_wayptInd;
				m_bReverse     = !m_bReverse;
				break;
			}
		}
	}

	SetDelayWait(pWayPt->GetWayPt(m_wayptInd).delay);
}

//set the next waypoint immediately
void WayptNode::SetNext()
{
	m_t = 0;
	_MoveNext();
}

bool WayptNode::Update()
{
	bool ret = false;

	//go through the current waypoint path
	if(m_wayptInd != m_wayptIndNext)
	{
		//get the time interval between the waypoint path
		m_t = TimeGetTime(&m_delaySpd)/TimeGetDelay(&m_delaySpd);

		//that means it's time to move on
		if(m_t >= 1)
		{
			m_t = 1;

			TimeReset(&m_delaySpd);

			m_wayptInd = m_wayptIndNext;

			Waypoint *pWayPt = (Waypoint *)IDPageQuery(m_wayptID);

			if(pWayPt)
			{
				SetDelayWait(pWayPt->GetWayPt(m_wayptInd).delay);

				if(m_wayptInd == pWayPt->GetNumWayPt()-1
					|| ((m_wayptInd == 0) && m_bReverse))
					ret = true;
			}
		}
	}
	//change to next waypoint path
	else if(TimeElapse(&m_delayWait))
	{
		_MoveNext();
	}

	return ret;
}

void WayptNode::SetDelaySpd(double delay)
{
	TimeInit(&m_delaySpd, delay);
	TimeReset(&m_delaySpd);
}

void WayptNode::SetDelayWait(double delay)
{
	TimeInit(&m_delayWait, delay);
	TimeReset(&m_delayWait);
}

bool WayptNode::IsDone()
{
	return (m_wayptInd == m_wayptIndNext && m_mode == WAYPT_STOPATEND);
}

//waypoint data if you want a waypoint manager
//mostly used by level objects and creatures
WaypointDat::WaypointDat() : m_pWayPt(0) 
{
}

WaypointDat::~WaypointDat() 
{ 
	RemoveWayPt(); 
}

//removes current waypoint
void WaypointDat::RemoveWayPt() 
{ 
	if(m_pWayPt) 
	{ 
		delete m_pWayPt; 
		m_pWayPt = 0; 
	} 
}

//set waypoint, this will remove the previous waypoint
void WaypointDat::SetWayPt(const Id & wayPtID)
{ 
	if(GAME_IDVALID(wayPtID))
	{
		RemoveWayPt();
		m_pWayPt = new WayptNode(wayPtID);
	}
}