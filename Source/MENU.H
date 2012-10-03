//#define MENU_ENABLE

#ifdef MENU_ENABLE

#define hSPRITE int*
#define hTXT    int*

#include <stdio.h>

#include "menu_ctrl.h"

#define TEXT_MAX   50
#define TEXT_SIZE  200

#define MAX_SCREEN 20
#define MAX_CONTROL 25
#define MAX_OPTION 3

// A1
#define ACTION_HAS		0
#define ACTION_SLIDE	1
#define ACTION_FADE		2
#define ACTION_ROTATE   3

#define ACTION_FWRD_ROTATE		4
#define ACTION_BACK_ROTATE		5
#define ACTION_TEXT_FADE		6
#define ACTION_TEXT_HIGHLIGHT	7

// A2
#define ANI_TYPE_NONE		0
#define ANI_ONCE_START		1   //HIGHLIGHT TEXT
#define ANI_REPEAT_START	2   //FADE		TEXT
#define ANI_ONCE_SELECT		3    
#define ANI_REPEAT_SELECT	4

// A3
#define BUTTON_NOTACTIVE		0
#define BUTTON_TEXT				1 
#define BUTTON_TEXT_NOTACTIVE	2
#define BUTTON_LIST				3

// MENU SELECT DEFINES
#define UP_DOWN_CAP				1
#define LEFT_RIGHT_CAP			2
#define UP_DOWN_LOOP			3
#define LEFT_RIGHT_LOOP			4

// A5  button action phases
#define FREE_PHASE				0
#define INIT_PHASE				1
#define TEXT_PHASE				2
#define UNIN_PHASE				3
#define DONE_PHASE				4

// A6  item text states
#define TEXT_FREE				0
#define TEXT_INIT				1	
#define TEXT_SELECT				2
#define TEXT_ACTIVATE			3
#define TEXT_UNINIT				4

#define FWRD					1
#define BACK					2


#define STATE_NOTACTIVE			0
#define STATE_IN				1
#define STATE_SEL_FWRD			2
#define STATE_SEL_BACK			3
#define STATE_LOOP_OUT			4
#define STATE_ACTION			5
#define STATE_OUT				6
#define STATE_LEAVE_SCREEN		7
#define STATE_ENTER_SCREEN		8
#define STATE_BUSY				9
#define STATE_DONE				10
#define STATE_TEXT_IN			11
#define STATE_TEXT_OUT			12

typedef struct _TATA_IMPORTANT_MENU_ITEM{

	int  ID;
/////////////////////////////////////////
	bool ActionType[4];  // #define A1
	int Blend[3];
	int SlideX[3];
	int SlideY[3];
	int Angle[3];
/////////////////////////////////////////
	int SpriteID;
/////////////////////////////////////////
	int ButtonType;
	int ListSize;
	daList<char*> TextList;
	int ListAction;
	int Color[3];
///////////////////////////////////////////
	bool StartBusy;				// true if starts busy on every entrance

	int	 StateID;				// indicates the state of an  item.
//////////////////////////////////////////
	int		ActionID;

}TIMI, *pTIMI;


class MenuApp{
private:

	bool    isInitialized;							// true if loaded
	bool    isActive;								// true if in menues
	int		StateID;								// indicates current state of the menus
	int		list_size;								// size of menu obj list					
	daList<pTIMI>   TIMI_List;						// List of menu obj	
	daList<pTIMI>	CurScreenList;					// list of objects on cur screen

	daList<hSPRITE>   SpriteList;					// list of hSPRITE handles
	
	MenuDirector MD;								// controls items per screen and control links

public:

	bool    isInit(){return this->isInitialized;};
	void    Init(){this->isInitialized=true;};
	bool    isAct(){return this->isAct;};
	void    Activate(){this->isActive=true;};

	//////////////////////////////////////
	// ctor & dtor functions
	MenuApp(){isInitialized = false;};
	~MenuApp();

	//////////////////////////////////////
	// loading functions
	bool	Load_Init();
	bool    Load_Resourses();
	bool	Load_Menu();
	//////////////////////////////////////
	// state switch function
	bool    Enter_Menu(int ID);
	//////////////////////////////////////
	// update functions
	bool    Update_Menu();                // processes current menu state

	bool    Update_Items();				  // processes current item states
	bool	Update_Input();				  // processes input and sets states
	bool	Update_Action(int ActionID);  // performs triggered action
	bool    Update_Sprites();			  // does final sprite update

	// return 1 if done
	bool    Item_Move(pTIMI item);
	bool    Item_Blend(pTIMI item);
	bool    Item_Rotate(pTIMI item);
	bool    Item_TextAction(pTIMI item, int typeID);
	
	void    ActivateScreen();
	void    DeActivateScreen();

	void	ActivateChildLoop();
	void	ActivateFirstLoop();
	void	DeActivateLoop(int LoopID);

	void    DeActivateObjects(int* idList, int size);
	void    ActivateObjects(int* idList, int size);
};

#endif //MENU_ENABLE