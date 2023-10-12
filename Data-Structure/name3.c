#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 이름 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*link; // 이름순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 이름순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

static int compare_by_name(const tName *pName1, const tName *pName2);
static int compare_by_freq(const tName *pName1, const tName *pName2);

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void) {
	LIST *newList = (LIST *)malloc(sizeof(LIST));

	if (newList){
		newList->count = 0;
		newList->head = NULL; // 초기화
		newList->head2 = NULL; // 초기화
		return newList;
	}

	else 
		return NULL;
	
};

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode) {

	if (pNode) {
		free(pNode);
	}
};

// 이름 리스트에 할당된 메모리를 해제 (head node, data node, name data) 
// 데이터(tName), 노드(Node), 리스트(List) 모두 해제 필요
void destroyList( LIST *pList) {

	if (pList) {

		// 이름순 링크
		NODE *current = pList->head;
		NODE *next;

		while (current != NULL) {
			next = current->link; // 이름순 기준 포인터
			if (current->dataPtr)
				destroyName(current->dataPtr);
			free(current);
			current = next;
		}

		// 이름순 리스트 완전 제거
		pList->head = NULL;

		/*
		// 빈도순 링크
		current = pList->head2;

		while (current != NULL) {
			next = current->link2; // 빈도순 기준 포인터
			if (current->dataPtr)
				destroyName(current->dataPtr);
			free(current);
			current = next;
		}

		*/
	
		// 빈도순 리스트 완전 제거
		pList->head2 = NULL;
		

		free(pList);
	}
};

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr) {

	NODE *newNode = (NODE*)malloc(sizeof(NODE));

	
	// memory overflow
	if (!newNode)
		return 0;

	// newNode 에 데이터 저장
	newNode->dataPtr = dataInPtr;
	newNode->link = NULL; // 초기화
    newNode->link2 = NULL; // 초기화

	// 리스트의 가장 첫 노드로 삽입될 경우
	if (!pPre) {
		newNode->link = pList->head;
		pList->head = newNode;
	}

	// 리스트 중간에 삽입될 경우
	else {
		newNode->link = pPre->link;
		pPre->link = newNode;
	}

	pList->count++;
	// successful
	return 1;
};

// internal function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc) {

	if (!pPre) {
		pLoc->link2 = pList->head2;
		pList->head2 = pLoc;
	}

	else {
		pLoc->link2 = pPre->link2;
		pPre->link2 = pLoc;
	}
};

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) {
// _search(list, &preName, &locName, namePtr)

	/*코드 점검 필요*/
	*pPre = NULL;
	*pLoc = pList->head;

	while (*pLoc) {
		int cmp = compare_by_name(pArgu, (*pLoc)->dataPtr);

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
			*pLoc = (*pLoc)->link;
		}
	}

	// Not found
	return 0;
};

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) {

	/*코드 점검 필요*/
	*pPre = NULL;
    *pLoc = pList->head2;

    while (*pLoc) {

		// found
		int cmp = compare_by_freq(pArgu, (*pLoc)->dataPtr);

		if (cmp == 0)
			return 1;

		// Not found (양수 반환 - pArgu 의 빈도가 pLoc 의 빈도보다 큰 경우)
		else if (cmp > 0) {
			*pPre = *pLoc;
			*pLoc = (*pLoc)->link2;
		}
		
		// 음수 반환 - 다음 노드와의 비교
		else 
			return 0;
	}

	// Not found
	return 0;

};

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex) {
	tName* newName = (tName*)malloc(sizeof(tName));

	if (newName){
		strcpy(newName->name, name);
		newName->sex = sex;
		newName->total_freq = 0;                         // valgrind 메모리 초기화 에러 해결
		memset(newName->freq, 0, sizeof(newName->freq)); // valgrind 메모리 초기화 에러 해결
	}

	else {
        free(newName);
        newName = NULL;
    }

	return newName;
}; 


////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, LIST *list) {

	char line[50]; 
    tName *namePtr = NULL;
	
	while (fgets(line, sizeof(line), fp)) {
        
		// 라인 파싱(parsing) 
		int year; 
        int year_freq;
        char name[20];
        char sex;

		// 데이터 읽기 (from a line) 
        sscanf(line, "%d\t%s\t%c\t%d", &year, name, &sex, &year_freq);
		
		NODE *preName = NULL;
        NODE *locName = NULL;
            
		// 읽은 데이터를 바탕으로 데이터 저장 구조체 (tName) 생성
		namePtr = createName(name, sex);
					
		// 존재 X
		if (_search(list, &preName, &locName, namePtr) == 0) {
			namePtr->freq[year - start_year] = year_freq;
        	namePtr->total_freq += year_freq;
			// 삽입
			_insert(list, preName, namePtr);
		}

		// 존재 O
		else {
			free(namePtr); // 메모리 누수 해결 - 이미 존재하는 이름-성별 인 경우에는 새롭게 데이터 tName 을 생성할 필요가 없다.
			locName->dataPtr->freq[year - start_year] = year_freq;
			locName->dataPtr->total_freq += year_freq;
		}
	}
};

// 이름순 리스트를 순회하며 빈도 순 리스트로 연결
void connect_by_frequency(LIST *list) {

	NODE *current = list->head; // 10.12 수정

    while (current != NULL) {
        NODE *preFreq = NULL;
        NODE *locFreq = NULL;
        _search_by_freq(list, &preFreq, &locFreq, current->dataPtr);
        _link_by_freq(list, preFreq, current);
        current = current->link; // 다음 노드로 이동
    }

};

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year) {

	NODE *current = pList->head;

    while (current != NULL) {
		printf("%s\t%c\t%d", current->dataPtr->name, current->dataPtr->sex, current->dataPtr->total_freq);
        
		for (int i = 0; i < num_year; i++) {
            printf("\t%d", current->dataPtr->freq[i]);
        }

        printf("\n");
        current = current->link; 
    }

}; // 이름순

void print_names_by_freq( LIST *pList, int num_year) {

	NODE *current = pList->head2;

    while (current != NULL) {
        printf("%s\t%c\t%d", current->dataPtr->name, current->dataPtr->sex, current->dataPtr->total_freq);

        for (int i = 0; i < num_year; i++) {
            printf("\t%d", current->dataPtr->freq[i]);
        }

		printf("\n");
        current = current->link2;
    }
}; // 빈도순


////////////////////////////////////////////////////////////////////////////////
// compares two names in name structures
// for _search function
// 정렬 기준 : 이름(1순위), 성별(2순위)
static int compare_by_name(const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
static int compare_by_freq(const tName *pName1, const tName *pName2)
{
	int ret = pName2->total_freq - pName1->total_freq;
	if (ret != 0) return ret;
	
	return compare_by_name(pName1, pName2);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by name\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_NAME;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
	printf( "Cannot create list\n");
	return 100;
	}
	
	// 입력 파일로부터 이름 정보를 리스트에 저장 (이름순 리스트 구축)
	load_names( fp, 2009, list);
	
	fclose( fp);

	if (option == SORT_BY_NAME) {
		
		// 이름순 리스트를 화면에 출력
		print_names( list, MAX_YEAR_DURATION);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_names_by_freq( list, MAX_YEAR_DURATION);
	}
	
	// 이름 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}