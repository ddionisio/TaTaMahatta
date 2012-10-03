#include "tata_main.h"
#include "tata_view.h"

#include "tata_creature.h"

#include "tata_globals.h"

View::View(const D3DXVECTOR3 &pt, const D3DXVECTOR3 &target)
: m_pt(pt), m_target(target), m_ptNext(pt), m_targetNext(target), m_t(0)
{
	ViewSetDelay(0);

	m_cam = CameraCreate((float*)m_pt, 0, 0, 0);

	if(m_cam)
		CameraSetTarget(m_cam, (float*)m_target);
	else
		ErrorMsg("View", "Unable to create camera");
}

View::View()
: m_pt(0,0,0), m_target(0,0,-1), m_ptNext(0,0,0), m_targetNext(0,0,-1), m_t(0)
{
	ViewSetDelay(0);

	m_cam = CameraCreate((float*)m_pt, 0, 0, 0);

	if(m_cam)
		CameraSetTarget(m_cam, (float*)m_target);
	else
		ErrorMsg("View", "Unable to create camera");
}

View::View(const View & other)
{
	m_pt = other.m_pt;
	m_target = other.m_target;
	m_ptNext = other.m_ptNext;
	m_targetNext = other.m_targetNext;
	m_t			 = other.m_t;

	m_delay = other.m_delay;

	m_cam = CameraCreate((float*)m_pt, 0, 0, 0);
	CameraSetTarget(m_cam, (float*)m_target);
}

View & View::operator =(const View & other)
{
	m_pt = other.m_pt;
	m_target = other.m_target;
	m_ptNext = other.m_ptNext;
	m_targetNext = other.m_targetNext;
	m_t			 = other.m_t;

	m_delay = other.m_delay;

	m_cam = CameraCreate((float*)m_pt, 0, 0, 0);
	CameraSetTarget(m_cam, (float*)m_target);

	return *this;
}

View::~View()
{
	if(m_cam)
	{ CameraDestroy(&m_cam); m_cam = 0; }
}

bool View::ViewUpdate()
{
	D3DXVECTOR3 oldPos(m_pt), oldTgt(m_target);

	if(!m_cam)
		return false;

	if(!ViewIsDone())
	{
		m_t = TimeGetTime(&m_delay)/TimeGetDelay(&m_delay);

		if(m_t >= 1)
		{
			m_t = 1;

			TimeReset(&m_delay);
		}

		D3DXVec3Lerp(&m_pt, &m_pt, &m_ptNext, m_t);
		D3DXVec3Lerp(&m_target, &m_target, &m_targetNext, m_t);
	}

	CameraSet(m_cam, (float*)m_pt);
	CameraSetTarget(m_cam, (float*)m_target);

	return true;
}

void View::ViewSetListener()
{
	if(!g_gameFlag.CheckFlag(GAME_FLAG_NOSOUND))
	{
		BASS_3DVECTOR pos, front, top={0,1,0};

		memcpy(&pos, (float*)m_pt, sizeof(pos)); pos.z *= -1;
		memcpy(&front, (float*)m_target, sizeof(front)); front.z *= -1;

		BASS_Set3DPosition(&pos, 0, &front, &top);
	}
}

bool View::ViewIsDone()
{
	return (m_pt == m_ptNext && m_target == m_targetNext);
}

void View::ViewSetDelay(double delay)
{
	TimeInit(&m_delay, delay);
}
	
void View::ViewSetScene()
{
	if(m_cam)
		CameraSetScene(m_cam);
}

void View::ViewSetPt(const D3DXVECTOR3 &pt) 
{ 
	if(m_ptNext != pt) 
	{ 
		D3DXVec3Lerp(&m_pt, &m_pt, &m_ptNext, m_t);

		TimeReset(&m_delay); 
	} 
	
	m_ptNext = pt; 
}
void View::ViewSetTarget(const D3DXVECTOR3 &target) 
{ 
	if(m_targetNext != target) 
	{ 
		D3DXVec3Lerp(&m_pt, &m_pt, &m_ptNext, m_t);

		TimeReset(&m_delay); 
	} 
	
	m_targetNext = target; 
}

void View::ViewCollideMap(hQBSP qbsp)
{
	gfxTrace trace={0};
	trace.t = 1;
	trace.leafIndex = -1;

	QBSPCollision(qbsp, 0, 0, (float*)m_targetNext, (float*)m_ptNext, &trace);

	hTXT txt = QBSPGetTexture(qbsp, trace.txtInd);
	if(trace.t < 1 && txt)
	{
		D3DXVec3Lerp(&m_ptNext, &m_targetNext, &m_ptNext, trace.t);
	}
}
