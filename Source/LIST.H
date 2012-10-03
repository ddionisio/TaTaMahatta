//***************************************************************
// Dimitry Kudlay
// 10-05-02
//
// This is my finest list as of the date.
// Special thanks to Nathan Ukrainetz for 
// My knowledge of C++, Thanks Nathan(:
//
// This Code was developed for "Tata-Mahatta" GAME
//***************************************************************
//********************************************************************
// DA LIST USAGE: Because it's 'da List.
//
// Official Name "daList"
// 
// Declaration:  daList<DataType> daListName(initial size);
// Resizing:     Resize(new list size);
//				 daList+(NewItem) adds and increments the list as well.
// Size:         GetSize();
// Adding, Removing:			List+(Item) and List-(Item)
// 
// Getting Values by Index
//
//	   List[index]
//
// Getting Values: By Itterator.
//
//     List^(index) returns item at index and sets itterator
//     ++List       increments itterator, returns item at new itt
//     --List       decriments itterator, returns item at new itt
//********************************************************************
#ifndef _DA_LIST_
#define _DA_LIST_
 
#include <windows.h>
#include <memory.h>
#include <iostream>


//***************************************************************
// Templatized Class List. Dimitry's Template List: dtList
//
// Sub-List, holds most of the functions of the list it self.
//***************************************************************
template <class T>
class dtList{
	private:
		template <class T>
			class dtNode{
				public:
					T* data;
					dtNode():data(0), Next(0), Prev(0){};
					dtNode(T* n):data(n), Next(0), Prev(0){};
					dtNode<T>* Next;
					dtNode<T>* Prev;
			};
	protected:
		dtNode<T>* Head;
		dtNode<T>* Tail;
		dtNode<T>* Cur;
	public:
													//
		dtList(){Head = NULL; Tail = NULL;};		// constructor
		~dtList();									// destructor
		dtList(dtList<T> & n);						// copy ctor
		dtList & operator = (const dtList<T> & n);	// equal operator, same as copy.
													//
		void DestroyList();							// destroys entire list.
													//
		bool AddAtHead(T* n);						// adds object to head of list
		bool AddAtNode(T* n1, T* n2);				// adds n2 after n1
													//
		//////////////////////////////////////////////
		// all of the following functions return FALSE if:
		// a: the object refered to doesnt exist,
		// b: the function doesnt find result
		// c: the function fails.
		//////////////////////////////////////////////
													//
		T* RemoveAtHead();							// removes head of the list
		bool RemoveAtNode(T* n);					// removes object from list if it exists
													//
		T* GetAtHead();								// returns item at the head of the list
		T* GetAtNode(T* n);							// returns the item after ob in the list
													//
		T* & ListIndex(int index);					// returns index item from list
													//
		//////////////////////////////////////////////
		// The following functions all relly on the in-list
		// itterator.                               note: 
		// The itterator is reset on call to daList::Resize
		//////////////////////////////////////////////
													//
		bool ResetCur(){ Cur = Head; return 1;};	// resets the itt to Head
													//
		T* GetFirst();								// resets the itt, returns Head
		T* GetNext();								// advances itt, returns itt.
		T* GetLast();								// decrements itt, returns itt
};													//
//////////////////////////////////////////////////////
// Templatized Class List Functions
//////////////////////////////////////////////////////
template <class T>
dtList<T>::~dtList(){								// dtor

	while(RemoveAtHead());							//undoes the list
	Head = NULL; Tail = NULL;
}

template <class T>
dtList<T>::dtList(dtList<T> & n){					// copy ctor

	dtNode<T> TempNode = n.Tail;					//get the tail

	if(Head)								
		while(RemoveAtHead());						//delete "this" list

		while(TempNode){
			this->AddAtHead(TempNode->data);		//add to the head all or copy
			TempNode = TempNode->Prev;
		}

	Head = n.Head;
	Tail = n.Tail;

	return *this;
}

template <class T>
dtList<T> & dtList<T>::operator = (const dtList<T> & n){

	dtNode<T>* TempNode = n.Head;

	if(Head)
		while(RemoveAtHead());

		while(TempNode){
			this->AddAtHead(TempNode->data);
			TempNode = TempNode->Next;
		}

	return *this;
}

template <class T>
void dtList<T>::DestroyList(){
	while(RemoveAtHead());								//undoes the list
	Head = NULL; Tail = NULL;
}

template <class T>
bool dtList<T>::AddAtHead(T * n){

	dtNode<T>* TempNode = new dtNode<T>(n);				//make new node

	if(!TempNode) return false;
	else{
		if(!Head){Head = TempNode; Tail = TempNode;}	//if first
		else{
			TempNode->Next = Head;
			Head->Prev = TempNode;
			Head = TempNode;
		}
		return true;
	}
}

template <class T>
bool dtList<T>::AddAtNode(T * n1, T * n2){
	if(!n1)
		AddAtHead(n2);

	dtNode<T>* TempNode = Head;
	dtNode<T>* NewNode = new dtNode<T>(n2);				//make new node

	while(TempNode){
		if(!memcmp(TempNode->data, n1, sizeof(T))){		//if data is the same
			NewNode->Next = TempNode->Next;				//fit in new node
			NewNode->Prev = TempNode;
			if(TempNode->Next)
				TempNode->Next->Prev = NewNode;

			TempNode->Next = NewNode;
			if(!NewNode->Next)
				this->Tail = NewNode;

			return true;
		}
		TempNode = TempNode->Next;
	}
	return false;
}

template <class T>
T * dtList<T>::RemoveAtHead(){

	dtNode<T>* TempNode;
	T* data;

	if(Head){
		TempNode = Head;								//get head ptr
		Head = Head->Next;
		if(Head) Head->Prev = NULL;
		data = TempNode->data;							//save data
		delete TempNode;								//delete head
		return data;
	}else	
		return 0;
}

template <class T>
bool dtList<T>::RemoveAtNode(T * n){

	dtNode<T>* TempNode =  Head;
	while(TempNode){
		if(!memcmp(TempNode->data, n, sizeof(T))){
			
			if(TempNode->Prev){
				TempNode->Prev->Next = TempNode->Next;						//fix the link with prev
				
				if(TempNode->Next) TempNode->Next->Prev = TempNode->Prev;	//fix the link with next
				else {Tail = TempNode->Prev; Tail->Next = 0;}				//else at tail
			}
			else{Head = TempNode->Next; Head->Prev = NULL;}					//else at head

			delete TempNode;

			return true;
		}
		TempNode = TempNode->Next;
	}
	return false;
}

template <class T>
T* dtList<T>::GetAtHead(){
	if(Head) return Head->data;
	else	 return false;
}

template <class T>
T* dtList<T>::GetAtNode(T * n){

	dtNode<T>* TempNode = Head;

	while(TempNode){
		if(!memcmp(TempNode->data, n, sizeof(T))){					//compares the data
			if(TempNode->Next){return TempNode->Next->data;}}		//returns the data
		TempNode = TempNode->Next;
	}
	return false;
}

template <class T>
T* & dtList<T>::ListIndex(int index){
	if(!index) return Head->data;

	dtNode<T>* TempNode = Head;
	for(int i = 0; i < index; i++){
		TempNode = TempNode->Next;
	}
	return TempNode->data;
}

template <class T>
T* dtList<T>::GetFirst(){

	if(Head){
		Cur = Head;
		return Head->data;
	}else
		return false;

}

template <class T>
T* dtList<T>::GetNext(){
	dtNode<T>* TempNode;

	if(Cur->Next){
		TempNode = Cur;
		Cur = TempNode->Next;
		return Cur->data;
	}
	return false;
}

template <class T>
T* dtList<T>::GetLast(){
	dtNode<T>* TempNode;

	if(Cur->Prev){
		TempNode = Cur;
		Cur = TempNode->Prev;
		return Cur->data;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//********************************************************************
// Array-Based Templatized Class List. Dimitry's Awesome List: daList
//
// Main List. Contains plethora of overloaded operatiors 
//********************************************************************
template <class T>
class daList{
	private:
		dtList<T> list;
		unsigned int size;
	public:

		daList():size(0){};									// ctor
		~daList(){};										// dtor
		daList(int size);									// copy ctor
															//
		bool Destroy(){list.DestroyList(); return true;};	// destroys the entire list.	
														//////
		void Resize(unsigned int newsize);				// resizes the array to the new ammount
													    //
		unsigned int GetSize(){return size;};			// returns the current size of the list;
														//
		daList<T> & operator = (const daList<T> & n);	// same as cpy
														//////////////
		T* & operator [] (int index){return list.ListIndex(index);} // overloaded to be used as array
																	//////////////////////
		bool operator + (T* n){ Resize(size+1); list.AddAtHead(n); return true;};		//
		bool operator - (T* n){ size -= 1; list.RemoveAtNode(n); return list.ResetCur;};//
																//////////////////////////
		T* operator ^ ( bool fill ){ return list.GetFirst();};	// itterator get first
		T* operator ^ ( int index );					        // itt set, and get index
																//
		T* operator ++ (){return list.GetNext();};				// increment itt, ret itt
		T* operator -- (){return list.GetLast();};				// decriment itt, ret itt
};																//
//////////////////////////////////////////////////////////////////
// Array-Based Templatized Class List Functions
//////////////////////////////////////////////////////////////////
template <class T>
T* daList<T>::operator^( int index ){
	T* tempT;

	tempT = list.GetFirst();
	for(int i = 1; i < index; i++){
		tempT = list.GetNext();
	}
	return tempT;

}

template <class T>
daList<T>::daList(int size){
	this->size = 0;
	Resize(size);
}

template <class T>
daList<T> &daList<T>::operator=(const daList<T> & n){
	Resize(n.size);
	list = n.list;
	size = n.size;
	return *this;
}

template <class T>
void daList<T>::Resize(unsigned int newsize){

	int dif = newsize - size;

	if(dif){
		for(int i = 0; i < dif; i++){
			if(size)											//if not empty
				list.AddAtNode(list.ListIndex(size - 1), 0);	//add after last node
			else
				list.AddAtHead(0);								//else add at head
		}
	}
	size = newsize;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#endif