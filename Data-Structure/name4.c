/*
TO DO LEFT (Last Update: 2023.10.28 토)

0. Segmentation Error 해결하기
1. 데이터 출력값과 정답 컴파일 파일의 출력값 비교 (cmp 함수)
2. VALGRIND 체크
*/ 

#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;	// 이름
	char	sex;	// 성별 M or F
	int		freq;	// 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*llink; // 왼쪽 링크
	struct node	*rlink; // 오른쪽 링크
} NODE;

typedef struct
{
	int		count;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

LIST *createList(void);
void destroyList( LIST *pList);

int addNode( LIST *pList, tName *dataInPtr);
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr);
int searchNode( LIST *pList, tName *pArgu, tName **dataOutPtr);

int countList( LIST *pList);
int emptyList( LIST *pList);

void traverseList( LIST *pList, void (*callback)(const tName *));
void traverseListR( LIST *pList, void (*callback)(const tName *));

static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr);
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

tName *createName( char *name, char sex, int freq);
void destroyName( tName *pNode);

int get_action();
int cmpName( const tName *pName1, const tName *pName2);
void print_name(const tName *dataPtr);
void input_name(char *name, char *sex);

////////////////////////////////////////////////////////////////////////////////
// (완료v) Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void) {

	LIST *newList = (LIST *)malloc(sizeof(LIST));

	if (newList) {
		// initialization
		newList->count = 0;
		newList->head = NULL;
		newList->rear = NULL;
		return newList;
	}

	// overflow
	else 
		return NULL;
};

// (완성v) 이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList) {

	NODE *pPre = NULL;
	NODE *pLoc = pList->head;
	
	while (pLoc != NULL) {
		pPre = pLoc;
		pLoc = pLoc->rlink;
		destroyName(pPre->dataPtr);			
		free(pPre);
	}

	// pList->head = NULL; // 오류 시, pList-> rear 에 대한 고려 필요
	// pList->rear = NULL;
	
	free(pList);
};
 
// (보류) Inserts data into list 
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
// 이 함수에서 아래 정의된 _insert, _search 함수를 사용한다.
// _insert 함수 원형: static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr) 
// _search 함수 원형: static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu)
int addNode( LIST *pList, tName *dataInPtr) {
	
	NODE *pPre = NULL;
	NODE *pLoc = pList->head;

	// duplicated key detection
	if (_search(pList, &pPre, &pLoc, dataInPtr))
		return 2;
	
	else {
		return _insert(pList, pPre, dataInPtr);
	}
};

// (완료v) Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr) {

	NODE *pPre = NULL;
	NODE *pLoc = NULL; // pList->head;

	if(_search(pList, &pPre, &pLoc, keyPtr)) {
		_delete(pList, pPre, pLoc,dataOutPtr);
		return 1; // deleted
	}
	else return 0; // not found
};

// (완료v) interface to search function
//	pArgu key being sought (=finding key)
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, tName *pArgu, tName **dataOutPtr) {

	NODE *pPre = NULL;
	NODE *pLoc = NULL; // pList->head;

	if (_search(pList, &pPre, &pLoc, pArgu)) {

		*dataOutPtr = pLoc->dataPtr;
		return 1; // successful
	}
	else
		return 0; // not found
};

// (완료v) returns number of nodes in list
int countList( LIST *pList) {
	return pList->count;
};

// (완료v) returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
	if (pList->count)
		return 0;
	else return 1;
};

// traverses data from list (forward)
// *callback 은 함수 포인터
// (완료v) The callback function will be called for each element in the list.
void traverseList( LIST *pList, void (*callback)(const tName *)) {

	NODE *current = pList->head;

	while(current != NULL) {
		callback(current->dataPtr);
		current = current->rlink;
	}
};

// (완료v) traverses data from list (backward) - reverse
void traverseListR( LIST *pList, void (*callback)(const tName *)) {

	NODE *current = pList->rear;

	while(current != NULL) {
		callback(current->dataPtr);
		current = current->llink;
	}
};

// (완료v) internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr) {

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

	/*
	if (!pPre) {
		if (pList->count == 0) 
			pList->rear = newNode; // 리어 노드 변경 (빈 리스트에 삽입되는 경우)

		newNode->rlink = pList->head;
		newNode->llink = pList->head->llink; // v
		pList->head->llink->rlink = newNode; // v
		pList->head->llink = newNode;
		pList->head = newNode; 
		
	}
	*/

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

// (완료v) internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr) {

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

	/*
	// 리스트 처음 노드를 삭제하는 경우 
	if(!pPre) {
		pList->head = pLoc->rlink;

		// 삭제 대상 노드 뒤에 노드가 더 없는 경우
		if (pList->head == NULL) 
			pList->rear = NULL;
		else 
			pList->head->llink = NULL;
	}

	// 리스트의 중간 또는 마지막 노드를 삭제하는 경우
	else {
		pPre->rlink = pLoc->rlink;
		// 마지막
		if (pLoc->rlink == NULL)
			pList->rear = pPre;
		// 중간
		else
			pLoc->rlink->llink = pPre;
	}
	*/

	// free(pLoc->dataPtr); // 데이터 삭제
	free(pLoc); // 노드 삭제
	pList->count--;
};

// (보류) internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) {
	
	*pPre = NULL;
	*pLoc = pList->head;

	while (*pLoc) {
		int cmp = cmpName(pArgu, (*pLoc)->dataPtr);

		// 동일한 이름을 찾거나, 혹은 적절한 위치를 찾았을 때 함수 종료
		// found
		if (cmp == 0)
			return 1;

		// Not found (음수 반환 - pArgu 의 이름이 pLoc 의 이름보다 앞의 알파벳)
		else if (cmp < 0)
			return 0;
		
		// 양수 반환 - 다음 노드와의 비교
		else {
			*pPre = *pLoc;
			*pLoc = (*pLoc)->rlink;
		}
	}

	// Not found
	return 0;
}; 


////////////////////////////////////////////////////////////////////////////////
// (완료v) Allocates dynamic memory for a name structure, initialize fields(name, sex, freq) and returns its address to caller
// return	name structure pointer
//			NULL if overflow
// +) 이번 과제의 입력 파일에는 중복되는 이름은 없다.
tName *createName( char *name, char sex, int freq) {

	tName* newName = (tName*)malloc(sizeof(tName));

	if (newName){
		newName->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
		strcpy(newName->name, name);
		newName->sex = sex;
		newName->freq = freq;      
		return newName;                  
	}

	// overflow
	else {
		free(newName); //v - I think it's optional. 
        return NULL;
	}
}; 

// (완료v) Deletes all data in name structure and recycles memory
void destroyName( tName *pNode) {
	
	free(pNode->name);
	free(pNode);
};

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for _search function
int cmpName( const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName *dataPtr)
{
	printf( "%s\t%c\t%d\n", dataPtr->name, dataPtr->sex, dataPtr->freq);
}

// gets user's input (name/sex)
void input_name(char *name, char *sex)
{
	char *p;

	while (1)
	{
		fprintf( stderr, "Input a name/sex to find: ");
		fscanf( stdin, "%s", name);
				
		if ((p = strchr(name, '/')) == NULL)
		{
			fprintf( stderr, "invalid name/sex!\n");
			continue;
		}
		*p++ = 0;
		if (*p && (*p == 'M' || *p == 'F')) *sex = *p;
		else
		{
			fprintf( stderr, "invalid sex! (M | F)\n");
			continue;
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char name[100];
	char sex;
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s\t%c\t%d", name, &sex, &freq) != EOF)
	{

		// 읽어들인 데이터를 바탕으로, tName 생성 - 해당 데이터를 담은 NODE 생성 
		pName = createName( name, sex, freq); 
		
		ret = addNode( list, pName);
		
		if (ret == 0 || ret == 2) // failure or duplicated (노드 생성 실패 / 노드 중복)
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		tName *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				input_name(name, &sex);
				
				pName = createName( name, sex, 0);

				if (searchNode( list, pName, &ptr)) print_name( ptr);
				else fprintf( stdout, "%s/%c not found\n", name, sex);
				
				destroyName( pName);
				break;
				
			case DELETE:
				input_name(name, &sex);
				
				pName = createName( name, sex, 0);

				if (removeNode( list, pName, &ptr))
				{
					fprintf( stdout, "(%s/%c, %d) deleted\n", ptr->name, ptr->sex, ptr->freq);
					destroyName( ptr);
				}
				else fprintf( stdout, "%s/%c not found\n", name, sex);
				
				destroyName( pName);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
