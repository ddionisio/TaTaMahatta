#include "tata_main.h"

#include "tata_ID.h"

//page stuff
#define	NUMPP			32			//number of obj per page
#define ID_SPOT_ONE	1//(NUMPP/32)		// Flag indicating spot one on page is used

#define ID_PREV_SPOT	1//(NUMPP/32)	// Amount to decrement mask to move to last spot
#define ID_NEXT_SPOT	1//(NUMPP/32)	// Amount to increment mask to move to next spot

#define ID_PAGE_EMPTY	 0	// Flag indicating empty page
#define ID_PAGE_FULL	~0	// Flag indicating full page

typedef struct _IDItem {
	void *data;
	LONGLONG counter;
} IDItem;

typedef struct _IDPage {
	FLAGS status;		//stuff it
	IDItem stuff[NUMPP];
} IDPage;

typedef struct _IDList {
	DWORD nStuff;				// number of stuff
	int nPages, nMaxPages;	// Count of object pages; maximum possible pages
	IDPage **Pages;		// Vector of pages of registered objs.
} IDList;

LONGLONG g_uIDCounter=1;	//unique counter

IDList	 g_IDList={0};

/********************************************************************
*																	*
*					  Game ID list interface						*
*																	*
********************************************************************/

PRIVATE void _IDPageAllocVector()
{
	IDPage **newVec;	// Vector of pages to reallocate

	int index;	// Loop variable

	newVec = (IDPage **)GAME_MALLOC(++g_IDList.nMaxPages * sizeof(IDPage*));

	assert(newVec);

	for (index = 0; index < g_IDList.nPages; ++index)	// Loop through current vector
		newVec[index] = g_IDList.Pages[index];	// Copy pointer data

	if (g_IDList.nPages != 0)	// Check whether pages exist
		GAME_FREE(g_IDList.Pages);

	g_IDList.Pages = newVec;	// Install new page vector
}

PRIVATE IDPage * _IDPageAdd()
{
	IDPage *Page = (IDPage *)GAME_MALLOC(sizeof(IDPage));	// Client page to allocate

	assert(Page);

	if (g_IDList.nPages == g_IDList.nMaxPages)	// Check whether page vector is full
		_IDPageAllocVector();	// Reallocate page vector

	g_IDList.Pages[g_IDList.nPages++] = Page;	// Install page in page vector

	Page->status = ID_PAGE_EMPTY;	// Indicate that all page blocks are available

	return Page;// Return newly allocated page
}

PRIVATE void _IDPageClean(IDPage *page)
{
	IDItem *obj = page->stuff, *nextObj;	// Reference to objs on page

	if (page->status == ID_PAGE_EMPTY) return;	// Trivial success if page is empty

	while(page->status != ID_PAGE_EMPTY)	// Scan through all objects
	{
		nextObj = obj; nextObj++;
		if (GETFLAGS(page->status,ID_SPOT_ONE))	// Check whether block is occupied
		{
			// destroy obj?
			obj->counter=0;
			obj->data=0;
		}

		obj=nextObj;

		page->status >>= ID_PREV_SPOT;	// Update mask for next block
	}
}

PRIVATE void _IDPageRemoveLast()
{
	IDPage *page = g_IDList.Pages[--g_IDList.nPages];	// Obtain final page from manager

	// Perform cleanup on page
	_IDPageClean(page);

	// Release page memory
	GAME_FREE(page);
}

/////////////////////////////////////
// Name:	IDPageClear
// Purpose:	clear all IDs generated,
//			call this after all objects
//			has been destroyed (in game
//			exit)
// Output:	all IDs cleared
// Return:	none
/////////////////////////////////////
PUBLIC void IDPageClear()
{
	while(g_IDList.nPages != 0)		// Iterate until all pages are gone
			_IDPageRemoveLast();	// Remove final page

	if(g_IDList.nMaxPages != 0)	// Check whether client pages exist
		GAME_FREE(g_IDList.Pages);	// Release page vector

	memset(&g_IDList, 0, sizeof(IDList));
}

/////////////////////////////////////
// Name:	IDPageAdd
// Purpose:	add a new object identifier
//			NOTE: DON'T CALL THIS!!!
//			THIS IS ONLY CALLED BY GAMEID
// Output:	pIDOut filled
// Return:	none
/////////////////////////////////////
PROTECTED void IDPageAdd(void *item, Id *pIDOut)
{
	assert(item);	// Verify that Data points to valid memory

	DWORD pIndex, bIndex;	// Index of client page; index of client block

	IDPage *page;

	FLAGS Mask = ID_SPOT_ONE;	// Mask used to test page availability

	if (g_IDList.nPages * NUMPP == g_IDList.nStuff)	// Check whether all pages are full
	{
		pIndex = g_IDList.nPages;	// Get index of new page

		page = _IDPageAdd();	// Append a new page to the manager
	}

	else	// Pages are available
	{
		for (pIndex = 0; pIndex < g_IDList.nPages; ++pIndex)	// Loop through all pages
			if (g_IDList.Pages [pIndex]->status != ID_PAGE_FULL) break;	// Break if page has openings

		page = g_IDList.Pages [pIndex];	// Obtain refence to page
	}

	for (bIndex = 0; bIndex < NUMPP; ++bIndex)	// Scan block occupancy flags
	{
		if (!GETFLAGS(page->status,Mask)) break;	// Break if block is open

		Mask <<= ID_NEXT_SPOT;	// Update mask for next block
	}

	SETFLAG(page->status,Mask);	// Indicate block is in use

	page->stuff[bIndex].data = item;	// set obj ptr

	pIDOut->ID = pIndex * NUMPP + bIndex;	// Obtain obj ID
	page->stuff[bIndex].counter = pIDOut->counter = ++g_uIDCounter;

	++g_IDList.nStuff;	// Increment item count
}

/////////////////////////////////////
// Name:	IDPageRemove
// Purpose:	removes given id
//			NOTE: DON'T CALL THIS!!!
//			THIS IS ONLY CALLED BY GAMEID
// Return:	none
/////////////////////////////////////
PROTECTED void IDPageRemove(Id *pID)
{
	IDPage *page;	//page where obj resides

	int pIndex = pID->ID / NUMPP;	// Page index

	// Ascertain that ID is within the messager's pages
	ASSERT_MSG(pIndex < g_IDList.nPages,"Invalid ID","IDPageRemove");

	page = g_IDList.Pages [pIndex];	// Obtain client page

	pID->ID %= NUMPP;	// Convert client ID to index

	CLEARFLAG(page->status,ID_SPOT_ONE << pID->ID);	// Mark block as available

	//if(page->status == ID_PAGE_EMPTY && pIndex + 1 == g_IDList.nPages)	// Check whether last page is empty
	//	_IDPageRemoveLast();

	--g_IDList.nStuff;	// Decrement obj count
}

/////////////////////////////////////
// Name:	IDPageGet
// Purpose:	use query instead...
// Output:	pCounter filled
// Return:	the object
/////////////////////////////////////
PROTECTED void * IDPageGet(DWORD ID, LONGLONG *pCounter)
{
	IDPage *page;	//page where obj resides

	int pIndex = ID / NUMPP;	// Page index
	int bIndex = ID % NUMPP;	// Block index

	if (pIndex >= g_IDList.nPages) return NULL;	// Trivially fail if client cannot exist

	page = g_IDList.Pages [pIndex];	// Refer to client page

	if (GETFLAGS(page->status,ID_SPOT_ONE << bIndex))	// Check whether block is in use
	{
		if(pCounter)
			*pCounter=page->stuff [bIndex].counter;
		return page->stuff [bIndex].data;	// If so, return client data
	}

	return NULL;// Client is unavailable
}

/////////////////////////////////////
// Name:	IDPageQuery
// Purpose:	call this to get the object
//			based on it's Id
// Output:	none
// Return:	the object if exists
/////////////////////////////////////
PUBLIC void * IDPageQuery(const Id & key)
{
	void *data;
	LONGLONG counter;

	data = IDPageGet(key.ID, &counter);	// Attempt to obtain client

	if (data != NULL && counter == key.counter)	// Check whether key matches
		return data;	// If so, return client

	return NULL;// Client is unavailable
}

/********************************************************************
*																	*
*						Game ID interface							*
*																	*
********************************************************************/
GameID::GameID(void *thisPtr)
{
	IDPageAdd(thisPtr, &m_ID);
}

GameID::~GameID()
{
	IDPageRemove(&m_ID);
}