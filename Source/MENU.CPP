#include "common.h"

#include "menu.h"

#ifdef MENU_ENABLE

main(){
	MenuApp *Tata_Menu = new MenuApp();
	int Menu_Enter_ID;

	// fill in the Menu_Enter_ID
	// and enter the following loop on switch state.


	if(!Tata_Menu->isInit()){

		Tata_Menu->Load_Init();

	}else if(!Tata_Menu->isAct()){

		Tata_Menu->Enter_Menu(Menu_Enter_ID);

	}else{

		Tata_Menu->Update_Menu(); // returns 0 on exit from menu. else 1.

	}

	// when exiting game
	delete(Tata_Menu);

	return true;
}

MenuApp::~MenuApp(){
// MD screen and Loop list
// text in TIMI

}
	
bool MenuApp::Load_Init(){

	this->MD.SetCurScreen(0);
	this->Load_Resourses();
	this->Load_Menu();
	this->Init();

	return true;
}

bool    MenuApp::Load_Resourses(){
	FILE* stream;
//	hTXT  CurTexture; 
//	hSPRITE CurSprite;
	int curID;
	int numItem;
	char* buff = (char*)malloc(sizeof(char)*TEXT_MAX);

	stream = fopen("../textFiles/menuData.txt", "r"); // open the main resourse file

	fscanf(stream, "TOTAL ITEMS=%d\n", &numItem); 
	fscanf(stream, "\n");

	for(int i = 0; i < numItem; i++){

		fscanf(stream, "ID=%d\n", &curID); 
		fscanf(stream, "%s\n", buff);

//		CurTexture = TextureCreate(curID, buff, 1, D3Drgb(255,255,255));

		fscanf(stream, "%s\n", buff);

//		CurSprite = SpriteLoad(CurTexture, 0, 0, buff);

//		this->SpriteList+(&CurSprite);
	}

	free(buff);

	fclose(stream);
	return true;

};

bool MenuApp::Load_Menu(){

	pTIMI tempTIMI;
	FILE* stream;
	int r, g, b;
	char* buff;
	int size1, size2;
	int ct;
	daList<pScreen> Screens(1);
	pScreen tempScr;
	daList<pLoop> Loops(1);
	pLoop tempLoop;
	int tempID;

	stream = fopen("../textFiles/menuData.txt", "r");

/////////////////
/// LOAD SCREENS 

	fscanf(stream, "SCREEN_NUM=%d\n\n", &size1);
	
	Screens.Resize(size1);

	for(int j = 0; j < size1; j++){

		tempScr = (pScreen)malloc(sizeof(SCREEN));

		fscanf(stream, "[%d]:", tempScr->ID);
		fscanf(stream, "%d=[", &size2);

		ct = 0;
		for(int k = ct; k < size2; k++){
			fread(&tempID, sizeof(int), 1, stream);

			ct += 1;

			if(ct != size2)
				fscanf(stream, ",");
			
			tempScr->TimiID[ct] = tempID; 
		}
		while(ct < MAX_TIMI_SCREEN){
			tempScr->TimiID[ct] = -1;
			ct += 1;
		}

		fscanf(stream, "]:%d", tempScr->numLoops);
	
		if(tempScr->numLoops){

			fscanf(stream, "=[");

			ct = 0;
			for(int k = tempScr->numLoops; k > 0; k--){
				fread(&tempID, sizeof(int), 1, stream);
				if((tempScr->numLoops) != 1)
					fscanf(stream, ",");	
				tempScr->LoopList[ct] = tempID; 
				ct += 1;
			}
			while(ct < MAX_LOOP_SCREEN){
				tempScr->LoopList[ct] = 0;
				ct += 1;
			}

			fscanf(stream, "[%d]", tempScr->StartLoop);
		}

		Screens+(tempScr);

	}
	MD.SetScreenList(Screens, size1);
/////////////////
// LOAD LOOPS
	
	fscanf(stream, "LOOP_NUM=%d\n\n", &size1);

	Loops.Resize(size1);
	
	for(j = 0; j < size1; j++){

		tempLoop = (pLoop)malloc(sizeof(CTRL_LOOP));

		fscanf(stream, "[%d]=", tempLoop->ID);
		
		fscanf(stream, "%d[",   &size2);

		ct = 0;
		for(int k = ct; k < size2; k++){
			fread(&tempID, sizeof(int), 1, stream);

			ct += 1;

			if(ct != size2)
				fscanf(stream, ",");
			
			tempLoop->TimiID[ct] = tempID; 
		}
		while(ct < MAX_TIMI_LOOP){
			tempLoop->TimiID[ct] = -1;
			ct += 1;
		}

		fscanf(stream, "]:[",   &size2);
		ct = 0;

		for(int j = ct; j < size2; j++){
			fread(&tempID, sizeof(int), 1, stream);

			ct += 1;

			if(ct != size2)
				fscanf(stream, ",");
			
			tempLoop->ExpandLoopID[ct] = tempID; 
		}
		while(ct < MAX_TIMI_LOOP){
			tempLoop->ExpandLoopID[ct] = -1;
			ct += 1;
		}

		fscanf(stream, "][%d][%d][%d][%d]\n", tempLoop->StartItem, tempLoop->ParentItem, tempLoop->ParentLoop , tempLoop->Direction);
		
		Loops+(tempLoop);
	}

	MD.SetLoopList(Loops, size1);

	fscanf(stream, "\n");
////////////////////
// LOAD MENU OBJECTS
	fscanf(stream, "NUM_TIMI=%d\n\n", &size1);

	this->TIMI_List.Resize(size1);

	for(int i = size1; i > 0; i--){
		tempTIMI = (pTIMI)malloc(sizeof(TIMI));

		// got the id.
		fscanf(stream, "ID=%d\n", &tempTIMI->ID); 

		// get the effects
		fscanf(stream, "EFFECTS=%d ", &tempTIMI->ActionType[0]);

		if(tempTIMI->ActionType[0]){
			fscanf(stream, ":%d %d %d\n", &tempTIMI->ActionType[1]\
										, &tempTIMI->ActionType[2]\
										, &tempTIMI->ActionType[3]);
		
			fscanf(stream, "ALPHA=%d,%d\n", &tempTIMI->Blend[0], &tempTIMI->Blend[1]);
	
			fscanf(stream, "X=%d,%d", &tempTIMI->SlideX[0], &tempTIMI->SlideX[1]);
			fscanf(stream, "Y=%d,%d", &tempTIMI->SlideY[0], &tempTIMI->SlideY[1]);
			fscanf(stream, "DEGREE=%d,%d", &tempTIMI->Angle[0], &tempTIMI->Angle[1]);
		}
		
		fscanf(stream, "BUTTON_TYPE=%d", tempTIMI->ButtonType);
		
		if(tempTIMI->ButtonType){

			fscanf(stream, "COLOR=(%d,%d,%d)",&r, &g, &b);
			tempTIMI->Color[0] = RGB(r, g, b);
			fscanf(stream, "(%d,%d,%d)",&r, &g, &b);
			tempTIMI->Color[1] = RGB(r, g, b);
			fscanf(stream, "(%d,%d,%d)\n",&r, &g, &b);
			tempTIMI->Color[2] = RGB(r, g, b);

			switch(tempTIMI->ButtonType){
				case 1:
				case 2:{
						tempTIMI->ListSize = 1;

						buff = (char*)malloc(sizeof(char)*TEXT_SIZE);

						fread(buff, sizeof(char),TEXT_SIZE, stream);

						tempTIMI->TextList+(buff);
					   }
					break;
				case 3:{
						fscanf(stream, "LISTSIZE=%d\n", tempTIMI->ListSize);

						for(int j = tempTIMI->ListSize; j >= 0; j--){
							buff = (char*)malloc(sizeof(char)*TEXT_SIZE);
							fread(buff, sizeof(char),TEXT_SIZE, stream);

							tempTIMI->TextList+(buff);
							
							fscanf(stream, "\n");
						}
				   }
					break;
			}
		}
		fscanf(stream, "\n");
		fscanf(stream, "ACTIONID=%d", tempTIMI->ActionID);
		fscanf(stream,"\n\n");

		this->TIMI_List+(tempTIMI);
	}

}

	//////////////////////////////////////
	// state switch function
bool    MenuApp::Enter_Menu(int ID){
	int loopid;

	switch(ID){
		case 0:{ // Logo
			// set right screen, set right loopID
			// transfer screen items to cur list  // CurScreen;
			
			this->MD.SetCurScreen(1);

			this->StateID = STATE_ENTER_SCREEN;

			   }break;
		case 1:{ // Main
			   }break;
		case 2:{ // New Game
			   }break;
		case 3:{ // Continue
			   }break;
		case 4:{ // Options
			   }break;
		case 5:{ // Controls
			   }break;
		case 6:{ // Credits
			   }break;
		case 7:{ // Pause
			   }break;
		case 8:{ // Hatch
			   }break;
		case 9:{ // Select
			   }break;
		case 10:{ // Saved
			   }break;
		case 11:{ // GameOver
			   }break;
		case 12:{ // GameEnd
			   }break;
		case 13:{ // leave
			   }break;
		case 14:{ // victory
			   }break;
	}

	this->Activate();

}



bool	MenuApp::Update_Input(){

	pScreen curScreen = this->MD.GetCurScreen();
	pLoop	curLoop   = this->MD.GetCurLoop();

	if(curLoop->Direction%2){ // up down
		if(InputIsDown(INP_UP)){// if up, switch select
			this->StateID = STATE_SEL_BACK;
		}
		if(InputIsDown(INP_DOWN)){// if down, switch select

			this->StateID = STATE_SEL_FWRD;

		}
		if((InputIsDown(INP_LEFT))||(InputIsDown(INP_RIGHT))){// if !up down, exit loop
			this->StateID = STATE_LOOP_OUT;
		}
	}else{ // left right
		if(InputIsDown(INP_LEFT)){// if down, switch select
		 
			this->StateID = STATE_SEL_BACK;
		}
		if(InputIsDown(INP_RIGHT)){// if down, switch select
			
			this->StateID = STATE_SEL_FWRD;

		}
		if((InputIsDown(INP_UP))||(InputIsDown(INP_DOWN))){// if !up down, exit loop
			this->StateID = STATE_LOOP_OUT;
		}
	}
	
	// if button press
	// set activate item

	if((InputIsDown(INP_A))||(InputIsDown(INP_C))||(InputIsDown(INP_START))){
			// set active
			this->StateID = STATE_ACTION;
	}
	if((InputIsDown(INP_B))||(InputIsDown(INP_D))||(INPXKbIsPressed(DIK_ESCAPE))){
			// exit loop or screen
			this->StateID = STATE_OUT;
	}


	return true;
}

bool MenuApp::Update_Menu(){
	int loopid;
	int retflag;
	int oldItemID;
	int newLoopID;
	pTIMI tempItem;

	switch(this->StateID){
	case STATE_NOTACTIVE:{
			return false; // return 0 if exiting menu
						 }break;

	case STATE_IN:{
		// mark all cur screen cur loop items with state_in
		// set this as state_busy

		
		//ActivateScreenItems, process cur loop
		this->ActivateScreen();
		this->ActivateChildLoop();

		this->StateID = STATE_BUSY;

				  }break;

	case STATE_SEL_FWRD:{// input sent
		// if not last, switch state_select, otherwise nothing
		// if has child loop, set to loop in
		// set this as state_done
		

		pLoop	curLoop   = this->MD.GetCurLoop();


		if(curLoop->Direction < 3){
			oldItemID = this->MD.IncrementLoop(curLoop, true);
		}else{
			oldItemID = this->MD.IncrementLoop(curLoop, false);
		}

		newLoopID = curLoop->ExpandLoopID[curLoop->CurItem];


		this->StateID = STATE_DONE;
		
		if(oldItemID > -1){
		
			DeActivateLoop(oldItemID);
			
			this->StateID = STATE_BUSY;
		
		}
		
		// check if new item has loop, activate loop in
	
		if(newLoopID >= 0){

			ActivateChildLoop();

			this->StateID = STATE_BUSY;
		}

				  }break;

	case STATE_SEL_BACK:{// input sent
		// if not first, switch state_select, otherwise nothing
		// set this as state_done
		pTIMI tempItem;
		pLoop	curLoop   = this->MD.GetCurLoop();


		if(curLoop->Direction < 3){
			oldItemID = this->MD.DecrementLoop(curLoop, true);
		}else{
			oldItemID = this->MD.DecrementLoop(curLoop, false);
		}

		newLoopID = curLoop->ExpandLoopID[curLoop->CurItem];

		this->StateID = STATE_DONE;
		
		if(oldItemID > -1){
		
			DeActivateLoop(oldItemID);
			
			this->StateID = STATE_BUSY;
		
		}
		
		// check if new item has loop, activate loop in
	
		if(newLoopID >= 0){

			ActivateChildLoop();

			this->StateID = STATE_BUSY;
		}


				  }break;
	
	case STATE_LOOP_OUT:{// input sent
		// if not main loop, switch select to parent
		// mark children as slide_text_out
		// set this as state_busy
		pLoop	curLoop   = this->MD.GetCurLoop();

		if(curLoop->ParentItem>=0){

			DeActivateLoop(curLoop->ID);
			
			this->MD.SetCurLoop(curLoop->ParentLoop, curLoop->ParentItem);

			this->StateID = STATE_BUSY;
		}else{
			this->StateID = STATE_DONE;	
		}

				  }break;

	case STATE_ACTION:{// input sent
		pLoop curLoop = this->MD.GetCurLoop();
		pTIMI tempItem;

		tempItem = *(this->TIMI_List^curLoop->CurItem);

		tempItem->StateID = STATE_ACTION;

		Update_Action(tempItem->ActionID);

		this->StateID = STATE_BUSY;

				  }break;

	case STATE_OUT:{// input sent
		// if screen id is not 1;, else dont do anything

		// if buttontype mark state_text_out
		// esle state_out if visible
		// this to state_swtch
		pScreen curScreen = this->MD.GetCurScreen();
		pLoop curLoop = this->MD.GetCurLoop();

		while(curLoop->ParentLoop != -1){
			this->MD.SetCurLoop(curLoop->ParentLoop,0);
			DeActivateLoop(curLoop->ID);
			pLoop curLoop = this->MD.GetCurLoop();
		}
		
		if(curScreen->ParentScreen != -1)
			this->StateID = STATE_LEAVE_SCREEN;

				  }break;
	case STATE_LEAVE_SCREEN:{
		DeActivateScreen();
		this->StateID = STATE_BUSY;

							}break;

	case STATE_ENTER_SCREEN:{
		pScreen tempScr;
		pLoop   tempLoop;

		if(Update_Items()){
			// activate parent screen items.
			switch(this->MD.GetCurScrID()){
			case 1:
			case 2:{
				this->MD.SetCurScreen(1);

				// init screen and loop // CurItem CurLoop;
				tempScr = *(this->MD.ScreenList^1);

				loopid = tempScr->StartLoop;

				tempScr->CurLoop = loopid;

				tempLoop = *(this->MD.LoopList^loopid);
				tempLoop->CurItem = tempLoop->StartItem;

				this->StateID = STATE_IN;
				   }break;

		}
							 }break;
	case STATE_BUSY:{
		retflag = UpdateItems();

		if(retflag > 0)
			this->StateID = STATE_DONE;
		if(retflag < 0)
			this->StateID = STATE_ENTER_SCREEN;

					}break;
	case STATE_DONE:
	default:{
		// update input
			this->Update_Input();

			}break;
	}
		return true;
}

bool MenuApp::Update_Items(){

	pTIMI tempItem;
	int size;
	int finishct = 0;
	int donect = 0;
	int notActiveCt = 0;

	size = this->CurScreenList.GetSize();
	
	tempItem = this->CurScreenList^1;

	// go through items perform action as on item StateID
	for(int i = 0; i < size; i++){

		finishct = 0;

		switch(tempItem->StateID){
		case STATE_IN:{
			if(tempMenu->ActionType[ACTION_HAS]){
				if(tempMenu->ActionType[ACTION_SLIDE]){
					finishct += this->Item_Move(tempMenu, FWRD)
					
				}
				if(tempMenu->ActionType[ACTION_FADE]){
					finishct += this->Item_Blend(tempMenu, FWRD);

				}
				if(tempMenu->ActionType[ACTION_ROTATE]){
					finishct += this->Item_Rotate(tempMenu, FWRD);

				}

				if(finishct == 3){ 
					if(tempMenu->ButtonType)	
						tempMenu->StateID = STATE_TEXT_IN;
					else
						tempMenu->StateID = STATE_DONE;
				}
				
			}else{
					if(tempMenu->ButtonType)	
						tempMenu->StateID = STATE_TEXT_IN;
					else
						tempMenu->StateID = STATE_DONE;
			}

					  }break;
		case STATE_TEXT_IN:{
				if(tempMenu->ButtonType){
					finishct = this->Item_TextAction(TEXT_INIT);	
				} 

				if(finishct)
					tempMenu->StateID = STATE_DONE;

			}
		case STATE_TEXT_OUT:{
				if(tempMenu->ButtonType){
					finishct = this->Item_TextAction(TEXT_UNINIT);	
				} 

				if(finishct)
					tempMenu->StateID = STATE_OUT;

			}
						   }break;
		case STATE_SELECT:{
					this->Item_TextAction(TEXT_SELECT);	
						  }break;
		case STATE_ACTION:{
				if(tempMenu->ActionType[ACTION_SLIDE]){
					finishct += this->Item_Move(tempMenu, ACT)
					
				}
				if(tempMenu->ActionType[ACTION_FADE]){
					finishct += this->Item_Blend(tempMenu, ACT);

				}
				if(tempMenu->ActionType[ACTION_ROTATE]){
					finishct += this->Item_Rotate(tempMenu, ACT);

				}

				this->Item_TextAction(TEXT_ACTION);	


				if(finishct == 3) 
					tempMenu->StateID = STATE_DONE;

						  }break;
		case STATE_OUT:{
			if(tempMenu->ActionType[ACTION_HAS]){
				if(tempMenu->ActionType[ACTION_SLIDE]){
					finishct += this->Item_Move(tempMenu, BACK)
					
				}
				if(tempMenu->ActionType[ACTION_FADE]){
					finishct += this->Item_Blend(tempMenu, BACK);

				}
				if(tempMenu->ActionType[ACTION_ROTATE]){
					finishct += this->Item_Rotate(tempMenu, BACK);

				}

				if(finishct == 3) 
					tempMenu->StateID = STATE_NOTACTIVE;
			}else{
					tempMenu->StateID = STATE_NOTACTIVE;
			}

					   }break;
		
		case STATE_NOTACTIVE:{
				this->CurScreen-(tempMenu);
							 }break;
		
		case STATE_DONE:
		default:
			break;
		}
		if(tempItem->StateID == STATE_DONE)		donect += 1;
		if(tempItem->StateID == STATE_NOTACTIVE) notActiveCt += 1;

		tempItem = ++this->CurScreenList;
		if(!tempItem)				return -99;// error
	}
	if(notActiveCt == size)	return -1;

	if(donect == size) return true;
	else			   return false;
	return false;
}

// not finished
bool	MenuApp::Update_Action(int ActionID){
	
	pTIMI tempItem;

	 // if done with action anime
		switch(tempItem->ActionID){			 // and has action code, do action
		case 1:{
			   }break;


		}
}

// not done
bool	MenuApp::Update_Sprites(){
	SIZE screenRect = GFXGetScrnSize();
	int size = this->CurScreen.ListSize();
}

// not done
bool    MenuApp::Item_Move(pTIMI item, int typeID){

}

bool    MenuApp::Item_Blend(pTIMI item, int typeID){

}

bool    MenuApp::Item_Rotate(pTIMI item, int typeID){

}

bool    MenuApp::Item_TextAction(pTIMI item, int typeID){
	
}
//////////////////////////////////////////////////
// Activation and Deactivation of entire screens
//
//////////////////////////////////////////////////

void 	MenuApp::ActivateScreen(){
	pScreen curScr = this->MD.GetCurScreen();

	// go through object list , activate all items with ID == int* ids
	// activate isVisible.
	ActivateObjects(curScr->TimiID, MAX_TIMI_SCREEN);

	ActivateFirstLoop();
}

void MenuApp::DeActivateScreen(){
	pTIMI littleTimi;
	int sizeMain = this->CurScreen.GetSize();

	// go through obj list make visible all on list.

	littleTimi = this->CurScreen^1;

	for(int j = 0; j < sizeMain; j++){

		if(littleTimi->ButtonType)
			littleTimi->StateID = STATE_TEXT_OUT;
		else
			littleTimi->StateID = STATE_OUT;

		littleTimi = ++this->CurScreen;
	}
}

///////////////////////////////////////////////
// Activate Loop
// Date: 10-03-02
// Attach any expansion menu items to curent screen
///////////////////////////////////////////////
void 	MenuApp::ActivateChildLoop(){
	pLoop  curLoop = this->MD.GetCurloop();
	pScreen curScr = this->MD.GetCurScreen();

	if(curLoop->ExpandLoopID[curLoop->CurItem] >= 0){ // activate newloop, newloop items
		curScr->CurLoop = curLoop->ExpandLoopID[curLoop->CurItem];
		curLoop = this->MD.GetCurloop();
		curLoop->CurItem = curLoop->StartItem;
		ActivateObjects(curLoop->TimiID, MAX_TIMI_LOOP);
	}
}

void MenuApp::ActivateFirstLoop(){
	pScreen curScr = this->MD.GetCurScreen();
	pLoop curLoop;
	pTIMI tempItem;

	this->MD.SetCurLoop(curScr->StartLoop,0);
	curLoop = this->MD.GetCurLoop();

	ActivateObjects(curLoop->TimiID, MAX_TIMI_LOOP);
}


void MenuApp::DeActivateLoop(int LoopID){
	// set parent loop, and parent item active
	// reset old loop, mark all objects of old loop as UNINIT_PHASE

	pLoop  deactLoop = this->MD.GetLoop(LoopID);

	DeActivateObjects(deactLoop->TimiID[i],MAX_TIMI_LOOP);

}

//////////////////////////////////////////////
// Activate Objects
// Date: 10-03-02
// Set objects to Visible
// Form Screen object list
/////////////////////////////////////////////
void   MenuApp::ActivateObjects(int* idList, int size){
	pTIMI littleTimi;
	int sizeMain = this->TIMI_List.GetSize();

	// go through obj list make visible all on list.

	littleTimi = this->TIMI_List^1;

	for(int j = 0; j < sizeMain; j++){

		for(int i = 0; i < size; i++){

			if(littleTimi->ID == idList[i]){

				littleTimi->StateID = STATE_IN;

				this->CurScreen+(littleTimi);
			}
			if(idList[i] < 0)	i = size;		// if done with processing
		}
		littleTimi = ++this->TIMI_List;
	}
}

void MenuApp::DeActivateObjects(int* idList, int size){
	pTIMI littleTimi;
	int sizeMain = this->CurScreen.GetSize();

	// go through obj list make visible all on list.

	littleTimi = this->CurScreen^1;

	for(int j = 0; j < sizeMain; j++){

		for(int i = 0; i < size; i++){

			if(littleTimi->ID == idList[i]){

				if(littleTimi->ButtonType)
					littleTimi->StateID = STATE_TEXT_OUT;
				else
					littleTimi->StateID = STATE_OUT;
				

			}
			if(idList[i] < 0)	i = size;		// if done with processing
		}
		littleTimi = ++this->CurScreen;
	}
}

#endif //MENU_ENABLE