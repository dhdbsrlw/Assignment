#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr) {

    NODE *newNode = (NODE*)malloc(sizeof(NODE));

	// memory overflow
	if (!newNode) 
		return 0; 

	// 데이터 삽입 및 초기화
	newNode->dataPtr = dataInPtr;
	newNode->rlink = NULL;
	newNode->llink = NULL;

	// 빈 리스트에 삽입되는 경우
	if (emptyList(pList)){
		pList->head = newNode;
		pList->rear = newNode;
	}
	// (빈 리스트가 아닌 상태에서) 처음에 삽입되는 경우
	else if (pPre == NULL) {
		newNode->rlink = pList->head;
		pList->head->llink = newNode;
		pList->head = newNode; 
	}

	// 리스트의 중간 또는 마지막에 삽입되는 경우
	else {

		// 중간 (오른쪽 노드 연결)
		if (pPre->rlink != NULL) {
			newNode->rlink = pPre->rlink;
			pPre->rlink->llink = newNode;
		}

		// 마지막
		else {
			pList->rear = newNode;
		}

		// 왼쪽 노드 연결
		pPre->rlink = newNode;
		newNode->llink = pPre;	
	}

	pList->count++;
	return 1; // successful
};

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr) {

    *dataOutPtr = pLoc->dataPtr; 

	if (pList->count == 1) {
		pList->head = NULL;
		pList->rear = NULL;
	}
	else {
		// 첫 노드 삭제
		if (pLoc->llink == NULL)
			pList->head = pLoc->rlink;
		// 중간 또는 마지막
		else {
			pPre->rlink = pLoc->rlink;
		}
		// 마지막
		if (pLoc->rlink == NULL)
			pList->rear = pPre;
		else
			pLoc->rlink->llink = pPre;
	}

	// free(pLoc->dataPtr); // 데이터 삭제
	free(pLoc); // 노드 삭제
	pList->count--;

};

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu) {

	if (emptyList(pList))
	{
		return 0;
	}

	
		*pLoc = pList->head;
		int cmp = pList->compare((*pLoc)->dataPtr, pArgu);


		// 동일한 이름을 찾거나, 혹은 적절한 위치를 찾았을 때 함수 종료
		// found
		if (cmp == 0)
			return 1;

		// Not found
		else if (cmp > 0)
			return 0;
		
		// 다음 노드와의 비교
		else {
			do {
				*pPre = *pLoc;
				*pLoc = (*pLoc)->rlink;
				if (*pLoc == NULL) return 0;

			} while (pList->compare((*pLoc)->dataPtr, pArgu) < 0);

			// found
			if (pList->compare((*pLoc)->dataPtr, pArgu) == 0)
				return 1;
			
			// not found
			else
				return 0;
		}
	
};

// External functions

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)) {
    
    LIST *newList = (LIST *)malloc(sizeof(LIST));
	if (newList) {
		// initialization
		newList->count = 0;
		newList->head = NULL;
		newList->rear = NULL;
        newList->compare = compare;
		return newList;
	}

	// overflow
	else 
		return NULL;
};

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList, void (*callback)(void *)) {

    NODE *pPre = NULL;
	NODE *pLoc = pList->head;
	
	while (pLoc != NULL) {
		pPre = pLoc;
		pLoc = pLoc->rlink;
		callback(pPre->dataPtr);			
		free(pPre);
	}

	free(pList);
};

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr) {
    NODE *pPre = NULL;
	NODE *pLoc = pList->head;

	// duplicated key detection
	if (_search(pList, &pPre, &pLoc, dataInPtr))
		return 2;
	
	else {
		return _insert(pList, pPre, dataInPtr);
	}
};

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr) {

	NODE *pPre = NULL;
	NODE *pLoc = NULL; 

	if(_search(pList, &pPre, &pLoc, keyPtr)) {
		_delete(pList, pPre, pLoc,dataOutPtr);
		return 1; 
	}
	else return 0; 

};

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr) {

    NODE *pPre = NULL;
	NODE *pLoc = NULL; // pList->head;

	if (_search(pList, &pPre, &pLoc, pArgu)) {

		*dataOutPtr = pLoc->dataPtr;
		return 1; // successful
	}
	else
		return 0; // not found

};

// returns number of nodes in list
int countList( LIST *pList) {
    return pList->count;
};

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList) {
    if (pList->count)
		return 0;
	else return 1;
};

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)) {

    NODE *current = pList->head;

	while(current != NULL) {
		callback(current->dataPtr);
		current = current->rlink;
	}
};

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)) {

    NODE *current = pList->rear;

	while(current != NULL) {
		callback(current->dataPtr);
		current = current->llink;
	}
};