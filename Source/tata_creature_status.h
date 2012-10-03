#ifndef _tata_creature_status_h
#define _tata_creature_status_h

class CreatureStatus
{
public:
	CreatureStatus(eCreStatus type, const Id & owner) : m_status(type), m_owner(owner) {}
	virtual ~CreatureStatus() {}

	eCreStatus GetStatus() { return m_status; }

	//update status effect, returns true if done...so that you can destroy it.
	//RETCODE_STATUS_DONE		 - status is done, ready to be deleted
	//RETCODE_STATUS_NOUPDATE	 - status says there should be no further update
	virtual RETCODE Update() { return RETCODE_STATUS_DONE; }

protected:
	Id			m_owner;			//the owner(must be a creature!)
private:
	eCreStatus	m_status;			//status of creature: invulnerable...etc.
};

/////////////////////////////////////////////////////////////////////////////
//Teleportation
/////////////////////////////////////////////////////////////////////////////
class StatusTeleport : public CreatureStatus
{
public:
	StatusTeleport(const Id & owner, const char *targetName);
	StatusTeleport(const Id & owner, const D3DXVECTOR3 & loc);

	~StatusTeleport();

	void GO();

	RETCODE Update();
private:
	bool			m_bGO;				//do we want to GO!!!!!!!!!!?????

	D3DXVECTOR3		m_dest;				//teleport destination

	bool			m_bFading;			//are we fading from current location?
	
	win32Time		m_delay;			//the delay timer for fading in/out
};

/////////////////////////////////////////////////////////////////////////////
//Invulnerable
/////////////////////////////////////////////////////////////////////////////
class StatusInvulnerable : public CreatureStatus
{
public:
	StatusInvulnerable(const Id & owner, double delay);
	~StatusInvulnerable();

	RETCODE Update();
private:
	bool			m_bFading;			//are we fading?
	
	win32Time		m_fadeDelay;		//the delay timer for fading in/out

	win32Time		m_invulDelay;		//how long it takes before it wears out?
};

/////////////////////////////////////////////////////////////////////////////
//Stun
/////////////////////////////////////////////////////////////////////////////
class StatusStun : public CreatureStatus
{
public:
	StatusStun(const Id & owner, double delay);
	~StatusStun();

	RETCODE Update();
private:	
	hPARFX			m_parFX;			//particle FX

	win32Time		m_stunDelay;		//how long it takes before it wears out?
};

#endif