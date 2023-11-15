#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
// 이름 구조체
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 'M' or 'F'
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

// 헤더 구조체
typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 구조체 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// qsort를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare_by_name( const void *n1, const void *n2) {

	const tName *name1 = (const tName *)n1;
	const tName *name2 = (const tName *)n2;

	int compare_idx = strcmp(name1->name, name2->name);

	// 1. 이름 기준 정렬
	if (compare_idx != 0) {
		return compare_idx;
	}

	// 2. 이름 동일 - 성별 기준 정렬
	else {
		if (name1->sex < name2->sex) 
            return -1;
        else if (name1->sex > name2->sex)
            return 1;
        else return 0;
    }
}

// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2){

	const tName *name1 = (const tName *)n1;
	const tName *name2 = (const tName *)n2;
	int compare_idx;

	// 1. 빈도 기준 정렬
	if (name1->total_freq != name2->total_freq) {
		compare_idx = name1->total_freq - name2->total_freq;
		if (compare_idx < 0) 
			return 1;
		else 
			return -1;
	}

	// 2. 빈도 동일 - 이름 기준 정렬 
	else {
		compare_idx = strcmp(name1->name, name2->name);
		if (compare_idx != 0)
			return compare_idx;
		
		// 3. 빈도, 이름 동일 - 성별 기준 정렬
		else {
			compare_idx = name1->sex - name2->sex;
			if (compare_idx < 0) 
				return -1;
			else if (compare_idx > 0)
				return 1;
			else return 0;
		}
	}
}

// 이름 순으로 정렬된 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, tNames *names) {
// load_names(fp, 2009, names);

    char line[50];
	memset(names->data, 0, names->capacity * sizeof(tName));

    while (fgets(line, sizeof(line), fp) != NULL) {

        // 라인 파싱(parsing) 
		int year; 
        int year_freq;
        char name[20];
        char sex;

		// 데이터 읽기 (from a line) 
        sscanf(line, "%d\t%s\t%c\t%d", &year, name, &sex, &year_freq);

		// 이진탐색을 위한 target 데이터 저장
		tName searchName;
		strcpy(searchName.name, name);
		searchName.sex = sex;

		// 기존 존재 여부 이진 탐색 (w/ bsearch)
		tName *existingName = bsearch(&searchName, names->data, names->len, sizeof(tName), compare_by_name);

		if (existingName != NULL) {
			// 존재 O
			existingName->total_freq += year_freq;
			existingName->freq[year-2009] = year_freq;	
		}

		else {
			// 존재 X
			if (names->len >= names->capacity) {
                names->capacity += 1000;
                names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
				// 연도별 빈도수 배열 초기화
				for (int i = names->len; i < names->capacity; i++)
            		memset(names->data[i].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
            }
			// 데이터 추가
            strcpy(names->data[names->len].name, name);
            names->data[names->len].sex = sex;
            names->data[names->len].total_freq = year_freq;
			names->data[names->len].freq[year - 2009] = year_freq;
            names->len++;
		}

		/*
		// 기존 존재 여부 선형 탐색 (w/ loop)
		
        // 기존 구조체 내 존재 여부에 다른 데이터 선별적 저장
        int found = 0;

        for (int i = 0; i < names->len; i++) {
			// 존재 O
            if (strcmp(names->data[i].name, name) == 0 && names->data[i].sex == sex) {
                // 해당 연도의 빈도만 저장
                names->data[i].total_freq += year_freq;
                names->data[i].freq[year - 2009] = year_freq; // 수정
                found = 1;
                break;
            }
        }
		
        // 존재 X
        if (!found) {
			// 구조체 용량이 부족한 경우, 메모리 추가 할당 
            if (names->len >= names->capacity) {
                names->capacity += 1000;
                names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
				// 연도별 빈도수 배열 초기화
				for (int i = names->len; i < names->capacity; i++)
            		memset(names->data[i].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
            }
			// 데이터 추가
            strcpy(names->data[names->len].name, name);
            names->data[names->len].sex = sex;
            names->data[names->len].total_freq = year_freq;
			names->data[names->len].freq[year - 2009] = year_freq;
            names->len++;
        }
		*/
    }

	// qsort(names->data, names->len, sizeof(tName), compare_by_name);
}

// 구조체 배열을 화면에 출력 (이름, 성별, 빈도 합, 연도별 빈도)
void print_names( tNames *names, int num_year) {
	
	for (int i = 0; i < names->len; i++) {
		printf("%s\t%c\t%d", names->data[i].name, names->data[i].sex, names->data[i].total_freq);
		for (int j = 0; j < 10; j++){ 
			printf("\t%d", names->data[i].freq[j]);
		}
		printf("\n");
	}
}


////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 이름 구조체를 초기화
// len를 0으로, capacity를 1000으로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1000;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
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
	
	// 이름 구조체 초기화
	names = create_names();

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	// 입력 파일(이름 정보)을 구조체에 저장
	load_names(fp, 2009, names); // 주석 해제

	fclose(fp);

	if (option == SORT_BY_NAME) {
		// 정렬 (이름순 (이름이 같은 경우 성별순))
		qsort( names->data, names->len, sizeof(tName), compare_by_name); // 주석 해제
	}
	else { // SORT_BY_FREQ
		// 정렬 (빈도순 (빈도가 같은 경우 이름순)) 세번째 기준은 성별 순
		qsort( names->data, names->len, sizeof(tName), compare_by_freq); // 주석 해제
	}
		
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION); // 주석 해제

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}
