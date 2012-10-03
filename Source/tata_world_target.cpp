#include "tata_main.h"
#include "tata_world.h"

/////////////////////////////////////
// Purpose:	add a target with given name(ID)
// Output:	new target appended
// Return:	none
/////////////////////////////////////
void World::TargetAdd(const char *name, const D3DXVECTOR3 & loc)
{
	Target newTarget;
	newTarget.ID = name;
	newTarget.loc = loc;
	
	m_targets.push_back(newTarget);
}

/////////////////////////////////////
// Purpose:	remove a target with given name(ID)
// Output:	target removed
// Return:	none
/////////////////////////////////////
void World::TargetRemove(const char *name)
{
	for(list<Target>::iterator i = m_targets.begin(); i != m_targets.end(); i++)
	{
		if(strcmp((*i).ID.c_str(), name) == 0)
		{
			m_targets.erase(i);
			break;
		}
	}
}

/////////////////////////////////////
// Purpose:	get the location of target
// Output:	none
// Return:	the target location
/////////////////////////////////////
bool World::TargetGet(const char *name, D3DXVECTOR3 & locOut)
{
	for(list<Target>::iterator i = m_targets.begin(); i != m_targets.end(); i++)
	{
		if(strcmp((*i).ID.c_str(), name) == 0)
		{
			locOut = (*i).loc;
			return true;
		}
	}

	return false;
}

/////////////////////////////////////
// Purpose:	set the location of target
// Output:	target set if found
// Return:	true if target found and set
/////////////////////////////////////
bool World::TargetSet(const char *name, D3DXVECTOR3 & newLoc)
{
	for(list<Target>::iterator i = m_targets.begin(); i != m_targets.end(); i++)
	{
		if(strcmp((*i).ID.c_str(), name) == 0)
		{
			(*i).loc = newLoc;
			return true;
		}
	}

	return false;
}