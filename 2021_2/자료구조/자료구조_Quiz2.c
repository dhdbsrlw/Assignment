#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

// 주소록 구조체 (최대글자수 임의지정)
typedef struct list {
	char name[10];
	char phone[20];
	char adr[10];
	char birth[10];
} list;

// 연결리스트 구현을 위한 자기 참조 구조체
typedef struct node {
	list student;
	struct node* next;
} node;


int main(void) {

	// 초기 화면
	printf("=================================================\n");
	printf("--------------------주소록 메뉴------------------\n");
	printf("1)목록보기     2)추가  3)검색  4)삭제      5)종료\n");
	printf("=================================================\n");


	// 주소록 기본 노드 및 변수
	node* head = NULL;
	node* tail = NULL;
	node* curNode = NULL;
	node* newNode = NULL;
	node* delNode = NULL;
	node* delNodeNext = NULL;
	char searchName[10];
	int option;

	
	// 메뉴 입력에 따른 동작
	while (scanf("%d", &option)) {

		// 1) 목록보기
		if (option == 1) {
			curNode = head;

			if (head == NULL) 
				printf("주소록에 데이터가 존재하지 않습니다.\n\n\n");
			else {
				while (curNode != NULL) {
					printf("%s   %s   %s   %s\n", curNode->student.name, curNode->student.phone, curNode->student.adr, curNode->student.birth);
					curNode = curNode->next;
				}
			}
			printf("\n\n");
		}


		// 2) 추가
		else if (option == 2) {
			newNode = (node*)malloc(sizeof(node)); //동적 할당
			newNode->next = NULL;

			printf("추가할 이름: ");
			scanf("%s", newNode->student.name);
			printf("추가할 전화번호: ");
			scanf("%s", newNode->student.phone);
			printf("추가할 주소: ");
			scanf("%s", newNode->student.adr);
			printf("추가할 생일: ");
			scanf("%s", newNode->student.birth);
			printf("----------추가 완료----------\n\n\n");

			if (head == NULL)
				head = newNode;
			else
				tail->next = newNode;
			tail = newNode;
		}


		// 3) 검색
		else if (option == 3) {
			curNode = head;

			printf("검색할 이름: ");
			scanf("%s", searchName);

			while (curNode != NULL) { //탐색

				if (strcmp(curNode->student.name, searchName) == 0) {
					printf("%s   %s   %s   %s\n\n\n", curNode->student.name, curNode->student.phone, curNode->student.adr, curNode->student.birth);
					break;
				}
				
				curNode = curNode->next;
			}

			if (curNode == NULL)
				printf("찾는 이름이 주소록에 존재하지 않습니다.\n\n\n");
		}


		// 4)삭제
		else if (option == 4) {

			// 주소록에 데이터가 없는 경우
			if (head == NULL)
				printf("주소록에 데이터가 존재하지 않습니다.\n\n\n");

			// 일반적인 경우
			else {
				delNode = head;
				delNodeNext = head->next;

				printf("삭제할 이름: ");
				scanf("%s", searchName);

				// 4-1)첫번째 노드(head) 삭제
				if (strcmp(head->student.name, searchName) == 0) {
					head = head->next;
					free(delNode);
					delNode = delNodeNext;
					delNodeNext = delNodeNext->next;
					printf("----------삭제 완료----------\n\n\n");
				}

				else {
					// 4-2)두번째 이후 노드 삭제 (문제의 구간)
					while (delNodeNext != NULL) {

						if (strcmp(delNodeNext->student.name, searchName) == 0) {
							delNode->next = delNodeNext->next;
							free(delNodeNext);
							printf("----------삭제 완료----------\n\n\n");
							break;
						}

						delNode = delNodeNext;
						delNodeNext = delNodeNext->next;
					}
				}
			}
		}

		// 5)종료
		else if (option == 5) {
			printf("\n\n");
			return 0;
		}

		// 주소록 메뉴
		printf("=================================================\n");
		printf("--------------------주소록 메뉴------------------\n");
		printf("1)목록보기     2)추가  3)검색  4)삭제      5)종료\n");
		printf("=================================================\n");
	}
}