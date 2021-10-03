//자료구조 Quiz 1 - 2020120036 오윤진
#include <stdio.h>
#pragma warning(disable:4996)


//문제_1
int main(void) {

	int num1 = 10;
	int num2 = 20;
	int* ptr1 = &num1;
	int* ptr2 = &num2;
	(*ptr1) += 10;
	(*ptr2) -= 10;

	printf("%d %d\n", num1, num2);
	//printf("%d %d\n", *ptr1, *ptr2);

	return 0;
} 



/* 문제_2
int main(void) {

	int arry[5] = { 1, 2, 3, 4, 5 };
	int* ptr;
	ptr = arry;

	for (int i = 0; i < 5; i++) 
		printf("%d 번째 배열의 값 : %d\n", i, *(ptr + i) += 2);

	return 0;
} */



/* 문제_3
int main(void) {

	int arry[5] = { 1, 2, 3, 4, 5 };
	int* ptr;
	ptr = arry + 4; //*ptr = 5
	int arry_sum = 0;

	for (int i = 0; i < 5; i++)
		arry_sum += *(ptr - i);
	
	printf("배열에 저장된 모든 정수의 합: %d\n", arry_sum);	
	return 0;
} */


/* 문제_4
int main(void) {

	int arry[6] = { 1, 2, 3, 4, 5, 6 };
	int* ptr1 = arry;
	int* ptr2 = arry + 5;

	printf("순서 변환 전 --------\n");
	for (int i = 0; i < 6; i++) 
		printf("%d번째 배열 값 : %d\n", i, *ptr1 + i);
	printf("\n");
	printf("순서 변환 후 --------\n");
	for (int i = 0; i < 6; i++)
		printf("%d번째 배열 값 : %d\n", i, *ptr2 - i);

	return 0;
}*/


/* 문제_5
unsigned int SquareByValue(int num) { //함수_1 (값 복사)
	num *= num;
	return num;
}


unsigned int SquareByReference(int* num) { //함수_2 (값 참조)
	(*num) *= (*num);
	return *num;
}

int main(void) {

	int num;

	printf("양의 정수를 입력하시오 : ");
	scanf("%d", &num);

	printf("Value 함수 제곱 : %d\n", SquareByValue(num));
	printf("num의 값 1 : %d\n", num);
	printf("Reference 함수 제곱 : %d\n", SquareByReference(&num));
	printf("num의 값 2 : %d\n", num);
	return 0;
}*/
