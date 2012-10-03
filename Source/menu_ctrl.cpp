#include "menu_ctrl.h"

#ifdef MENU_ENABLE

pScreen MenuDirector::GetCurScreen(){
	return &this->ScreenList[this->CurScreen];
}

pLoop MenuDirector::GetCurLoop(){
	pScreen scr = this->GetCurScreen();

	return &this->LoopList[scr->CurLoop];
}

pLoop MenuDirector::GetLoop(int LoopId){

	return &this->LoopList[LoopId];
}
int MenuDirector::GetItemIndex(int* list, int size, int itemID){
	
	for(int i = 0; i< size; i++){

		if(list[i] == itemID)
			return i;
	}
	return -1;

}

bool    MenuDirector::SetCurLoop(int id, int StartID){

	pScreen scr = this->GetCurScreen();
	pLoop nLoop;

	if(id >= 0){
		scr->CurLoop = id;
		nLoop = this->GetCurLoop();
		if(StartID >=0){
			nLoop->CurItem  = StartID;
		}else{
			nLoop->CurItem = nLoop->StartItem;
		}
		// set scrn loop to parent loop, set loop to start item;
	}
}

// return the id of the old item to deactivate
int    MenuDirector::IncrementLoop(pLoop curLoop, bool isCapped){
	
	int temploop;

	if(isCapped){
		if(curLoop->CurItem+1 < MAX_TIMI_LOOP){
			if(curLoop->TimiID[curLoop->CurItem+1] != -1){
				curLoop->CurItem += 1;

				if(curLoop->ExpandLoopID[curLoop->CurItem-1] >= 0)
				{
					return curLoop->ExpandLoopID[curLoop->CurItem-1];
				}
				
			}
		}
	}else{
		if((curLoop->CurItem+1 == MAX_TIMI_LOOP)||(curLoop->TimiID[curLoop->CurItem+1] == -1)){ // loop over
			temploop = curLoop->CurItem; 
			curLoop->CurItem = 0;

			if(curLoop->ExpandLoopID[curLoop->CurItem] >= 0){
				return curLoop->ExpandLoopID[temploop]);
			}	
			
		}else if(curLoop->CurItem+1 < MAX_TIMI_LOOP){
			if(curLoop->TimiID[curLoop->CurItem+1] != -1){
				curLoop->CurItem += 1;

				if(curLoop->ExpandLoopID[curLoop->CurItem] >= 0){
					return curLoop->ExpandLoopID[curLoop->CurItem-1]);
				}
				
			}
		}
	}
	
	return -1;

}

int    MenuDirector::DecrementLoop(pLoop curLoop, bool isCapped){

	int temploop;

	if(isCapped){
		if(curLoop->CurItem-1 >= 0 ){
			if(curLoop->TimiID[curLoop->CurItem-1] != -1){
				curLoop->CurItem -= 1;

				if(curLoop->ExpandLoopID[curLoop->CurItem] >= 0)
				{
					return curLoop->ExpandLoopID[curLoop->CurItem+1]);
				}
				
			}
		}
	}else{
/////////////////////////////////////////////////////////////////////////
		if(curLoop->CurItem-1 >= 0 ){
			if(curLoop->TimiID[curLoop->CurItem-1] != -1)){
				curLoop->CurItem -= 1;

				if(curLoop->ExpandLoopID[curLoop->CurItem] >= 0)
				{
					return curLoop->ExpandLoopID[curLoop->CurItem+1]);
				}
				
			}
		}else{ // got to the end of the loop
			for(int u = MAX_TIMI_LOOP; u >= 0; u--){
				if(curLoop->TimiID[u] != -1){
					temploop = curLoop->CurItem;
					curLoop->CurItem = u;

					if(curLoop->ExpandLoopID[curLoop->CurItem] >= 0){
						 return curLoop->ExpandLoopID[temploop]);
					}	
					
				}
			}
		}
	}
}

#endif //MENU_ENABLE