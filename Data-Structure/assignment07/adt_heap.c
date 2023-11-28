#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free

#include "adt_heap.h"

// 최대힙 (max-heap) 을 구현한다.
// adt_heap.h 에 정의되어 있는 함수 원형들을 구현한다.

/* Reestablishes heap by moving data in child up to correct location heap array
*/
// child node 를 올리면서 힙 내 적절한 위치에 둔다.
static void _reheapUp( HEAP *heap, int index) {

    while(index != 0 && heap->compare(heap->heapArr[index], heap->heapArr[(index-1)/2]) > 0) {
       
        void *temp;
        temp = heap->heapArr[index];
        heap->heapArr[index] = heap->heapArr[(index - 1) / 2];
        heap->heapArr[(index - 1) / 2] = temp;
        index = (index - 1) / 2;
    }
};

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index) {

    void *left;
    void *right;
    int max;

    if (index * 2 + 1 <= heap->last) {
        left = heap->heapArr[index * 2 + 1];
        
        if (index * 2 + 2 <= heap->last) {
            right = heap->heapArr[index * 2 + 2];

            if (heap->compare(left,right) > 0)
                max = index * 2 + 1;
            else
                max = index * 2 + 2;
        }
        else 
            max = index * 2 + 1;
        
        if (heap->compare(heap->heapArr[index],  heap->heapArr[max]) < 0) {
            void *temp;
            temp = heap->heapArr[index];
            heap->heapArr[index] = heap->heapArr[max];
            heap->heapArr[max] = temp;
            _reheapDown(heap, max);
        }
    }
};


/* 이하는 adt_heap.h 에 정의되어 있는 함수 원형*/


/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
The initial capacity of the heap should be 10 = 초기 capacity 는 10 으로 설정한다.
*/
HEAP *heap_Create( int (*compare) (const void *arg1, const void *arg2)) {

    HEAP *newHeap = (HEAP*)malloc(sizeof(HEAP));

    if (!newHeap)
        return NULL;

    newHeap->last = -1;
    newHeap->capacity = 10; // 문제 조건대로 
    newHeap->heapArr = (void**)malloc(sizeof(void*) * 10); // 문제 조건대로
    newHeap->compare = compare;

    return newHeap;
};

/* Free memory for heap
*/
void heap_Destroy( HEAP *heap, void (*remove_data)(void *ptr)) {

    while(!heap_Empty(heap)) { // 일단 작성, 오류 시 수정
        remove_data(heap->heapArr[heap->last]);
        heap->last--;
    }

    free(heap->heapArr); // 11.28 오류 수정
    free(heap);
};

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert( HEAP *heap, void *dataPtr) {

    heap->last++;

    // 문제 조건에 따르면 realloc 시 100 씩 증가하도록 한다.
    // 그렇기에 heap 이 full 될 일이 없는 것 아닌가 ? 
    if (heap->last == heap->capacity){
        heap->heapArr = (void**)realloc(heap->heapArr, sizeof(void*) * (heap->capacity + 100));
        heap->capacity += 100; // 문제 조건대로
        // return 0; // heap full
    }

    heap->heapArr[heap->last] = dataPtr;
    _reheapUp(heap, heap->last); 

    return 1;
};

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete( HEAP *heap, void **dataOutPtr) {

    if (heap->last == -1)
        return 0; // empty
    
    *dataOutPtr = heap->heapArr[0]; // first node

    // 실제로는 마지막 원소와 자리를 바꾸고 last 를 하나 줄인 후 reheapDown 을 수행한다.
    heap->heapArr[0] = heap->heapArr[heap->last];
    heap->last--;
    _reheapDown(heap, 0);
    return 1; // success
};

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(  HEAP *heap) {
    if (heap->last == -1)
        return 1;
    else
        return 0;
};

/* Print heap array */
void heap_Print( HEAP *heap, void (*print_func) (const void *data)) {

    for (int i = 0; i <= heap->last; i++){
        print_func(heap->heapArr[i]);
    }
    printf("\n");
};
