#ifndef _tata_object_h
#define _tata_object_h

#include "tata_ID.h"
#include "tata_basic.h"
#include "tata_waypoint.h"

#include "tata_collision.h"

//this is the Abstract Base Class for Level Objects

class Object : public EntityCommon
{
public:
	Object(int subType);
	virtual ~Object();

	RETCODE Update();

	// if section is NULL, then
	// the entire file is parsed.
	// format of section: [<section>_begin]
	// 				      ... // code
	//					  [<section>_end]
	RETCODE LoadScript(const char *filename, const char *section);

	//destroys current script
	void    RemoveScript();

	//Collide Entity, Object style
	void CollideEntity(EntityCommon *pEntity, const gfxTrace & trace);

	//Collide World, Object style
	void CollideWorld(hQBSP qbsp, const gfxTrace & trace);

protected:

private:
	eObjectType	m_objType;			//type of object (eObjectType)
	hSCRIPT		m_script;			//the script, updated if exists
};

#endif