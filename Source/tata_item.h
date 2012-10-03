#ifndef _tata_item_h
#define _tata_item_h

#include "tata_ID.h"
#include "tata_basic.h"
#include "tata_bound.h"

//
// This file includes all items (tender love, care, egg...)
//

//this is the Abstract Base Class for Items

class Item : public EntityCommon
{
public:
	Item(int subType);

	virtual ~Item() {}

	// Load item
	RETCODE Load(const char *dataTypeName);

	RETCODE Update();		//update

	//call this after setting the location on load.
	void SetBoyantMinMax();

private:
	Move	m_boyantSpd;		//some sort of spd for going up and down
	Move	m_rotSpd;			//rotating spd.

	float	m_boyantDir;		//used on boyant to go up or down

	float	m_minBoyant, m_maxBoyant;
};

//tender love and care
class Resource : public Item
{
public:
	Resource();
	~Resource();

	short GetResource(eResourceType type) { return m_rcVal[type]; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	short	m_rcVal[RC_MAX];	//resource value for each resource type

	hTXT	m_particleTxt;		//particle texture
};

//collectibles, for secret stuff and easter eggs
class Collectible : public Item
{
public:
	Collectible();
	~Collectible();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	hTXT	m_particleTxt;		//particle texture
};

#endif