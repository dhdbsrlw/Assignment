#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 이름 순서와 성별 순서, total freq 모두 맞으나, freq 배열 값이 잘못됨.

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언

// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 이름과 성별에 의해 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 1. 이미 등장한 이름인지 검사하기 위해
// 2. 새로운 이름을 삽입할 위치를 찾기 위해 binary_search 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch, qsort를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare_by_name( const void *n1, const void *n2);

// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2);

// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found);


////////////////////////////////////////////////////////////////////////////////
// 함수 정의

void load_names( FILE *fp, int start_year, tNames *names) {

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

		// 존재여부 이진탐색
		int found = 0;
		int target_idx = binary_search(&searchName, names->data, names->len, sizeof(tName), compare_by_name, &found);

		if (found == 1) {
			// 존재 O
			tName* target = &(names->data[target_idx]); 
			target->total_freq += year_freq;
			target->freq[year-2009] = year_freq;	
		}

		else {
			// 존재 X
			// 배열의 용량 (capacity) 이 부족할 경우 용량 추가
			if (names->len >= names->capacity) {
                names->capacity += 1000;
                names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));

				// 연도별 빈도수 배열 초기화
				for (int i = names->len; i < names->capacity; i++)
            		memset(names->data[i].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
            }

			// 데이터 추가를 위한 메모리 이동 (w/ memmove)
			int start_idx = target_idx;
			int end_idx = names->len - 1;
			size_t bytesToMove = (end_idx - start_idx + 1) * sizeof(tName);
			memmove(names->data + start_idx + 1, names->data + start_idx, bytesToMove);

			// 데이터 삽입 전 초기화
			for (int i = 0; i < MAX_YEAR_DURATION; i++) {
    		tName* target = &(names->data[start_idx]);
    		target->freq[i] = 0;
			}

			// 데이터 추가 (삽입)
			tName* target = &(names->data[target_idx]); 
			strcpy(target->name, name);
			target->sex = sex;
			target->total_freq = year_freq;
			target->freq[year - 2009] = year_freq;
			names->len++;
		}
    }
};


// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year) {
	
	for (int i = 0; i < names->len; i++) {
		printf("%s\t%c\t%d", names->data[i].name, names->data[i].sex, names->data[i].total_freq);
		for (int j = 0; j < 10; j++){ 
			printf("\t%d", names->data[i].freq[j]);
		}
		printf("\n");
	}
};


// bsearch, qsort를 위한 비교 함수
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
};


// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2) {

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
};


// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found) {

				int left = 0;
				int right = nmemb - 1;
				// int result = -1;

				while (left <= right) {
					int middle = left + (right - left) / 2;
					int cmp = compare(key, (char*)base + middle * size);

					if (cmp == 0) {
						// result = middle;
						*found = 1;
						return middle;;
					}
					else if (cmp < 0) {
						right = middle - 1;
					}
					else {
						left = middle + 1;
					}
				}

				return left;
			};



// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
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
	
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose( fp);
	
	if (option == SORT_BY_NAME) {
		// 정렬 (이름순 (이름이 같은 경우 성별순))
		qsort( names->data, names->len, sizeof(tName), compare_by_name);
	}
	else { // SORT_BY_FREQ
		// 정렬 (빈도순 (빈도가 같은 경우 이름순))
		qsort( names->data, names->len, sizeof(tName), compare_by_freq);
	}
	
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);
	
	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}

