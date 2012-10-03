#ifndef _tata_ID_h
#define _tata_ID_h

//This is the ID generator for all objects used in the game
//Also used to keep track of which ones are allocated and freed

//whenever you create game ID, game ID will add itself to a global list to
//keep track of itself.  Removes itself when destroyed
class GameID
{
public:
	GameID(void *thisPtr); //this will add an identifier in the ID list for given 'thisPtr'
	virtual ~GameID();	   //this will remove the identifier in the ID list

	Id GetID() { return m_ID; } //grab identifier
private:
	Id m_ID;	//the identifier
};

//
// ID functions
//

/////////////////////////////////////
// Name:	IDPageQuery
// Purpose:	call this to get the object
//			based on it's Id
// Output:	none
// Return:	the object if exists
/////////////////////////////////////
PUBLIC void * IDPageQuery(const Id & key);

/////////////////////////////////////
// Name:	IDPageClear
// Purpose:	clear all IDs generated,
//			call this after all objects
//			has been destroyed (in game
//			exit)
// Output:	all IDs cleared
// Return:	none
/////////////////////////////////////
PUBLIC void IDPageClear();

#endif