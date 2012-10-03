#ifdef MENU_ENABLE

#include "list.h"

#define MAX_TIMI_SCREEN 20 // max amount of menu objects on screen
#define MAX_LOOP_SCREEN 5
#define MAX_TIMI_LOOP   5


//if not direction press of cur item, go up to main...
//ex, if up down, exit on menu left right

typedef struct _SCREEN_DSC{

	int ID;							// screen ID
	int StartLoop;					// start loop
	int CurLoop;					// current loop
	int TimiID[MAX_TIMI_SCREEN];	// object ids used by the screen
	int numLoops;					// size of the below list
	int LoopList[MAX_LOOP_SCREEN];	// all loops used by the screen
	int	ParentScreen;				// last screen ID
	
}SCREEN, *pScreen;

typedef struct _CTRL_LOOP{

	int ID;								// loop ID
	int StartItem;						// initial active item ID
	int ParentItem;						// initial active item after exit
	int CurItem;						// current active item ID
	int	ParentLoop;						// last loop id
	int ExpandLoopID[MAX_TIMI_LOOP];    // 0/next setID array list
	int TimiID[MAX_TIMI_LOOP];          // object ids controled by this loop
	int Direction;						// type of control it will have.

}CTRL_LOOP, *pLoop;

class MenuDirector{
private:
	int CurScreen;
public:
	daList<pScreen> ScreenList; // list of different screens to load into director
	daList<pLoop> LoopList;    // list of object control loops

	MenuDirector(){CurScreen=0;};
	~MenuDirector();

	bool	SetScreenList(daList<pScreen> list, int list_size); 
	bool	SetLoopList(daList<pLoop> list, int list_size);

	bool    SetCurScreen(int id){CurScreen = id;};
	bool    SetCurLoop(int id, int StartID);

	int     GetCurScrID(){return this->CurScreen;};
	pScreen GetCurScreen();
	pLoop   GetCurLoop();
	int     GetItemIndex(int* list, int size, int itemID);

	bool    IncrementLoop(pLoop curLoop, bool isCapped);
	bool    DecrementLoop(pLoop curLoop, bool isCapped);
};

#endif //MENU_ENABLE