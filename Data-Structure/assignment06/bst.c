#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"

// internal functions (not mandatory)
// used in BST_Insert
// NODE *root 에서 NODE **root 로 인자 변경
static void _insert( NODE **root, NODE *newPtr, int (*compare)(const void *, const void *)) {

    if (*root == NULL)
        *root = newPtr;

    // 삽입할 위치 찾기
    else {
        int cmp = compare (newPtr->dataPtr, (*root)->dataPtr);
        
        if (cmp < 0) 
            _insert(&((*root)->left), newPtr, compare);

        else 
            _insert(&((*root)->right), newPtr, compare);
    }    
};


// used in BST_Insert
// 완료
static NODE *_makeNode( void *dataInPtr) {

    NODE* newNode = (NODE*) malloc(sizeof(NODE));
    
    newNode->dataPtr = dataInPtr;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
};

// used in BST_Destroy
// 완료
static void _destroy( NODE *root, void (*callback)(void *)) {
    
    if (root == NULL)
        return;

    _destroy(root->left, callback);
    _destroy(root->right, callback);

    if (callback != NULL)
        callback(root->dataPtr);
    
    free(root);
};

// used in BST_Delete
// return pointer to root
// 완료 (오류 시 수정 필요)
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)) {

    if (root == NULL) 
        return NULL; // matching key not found

    int cmp = compare(keyPtr, root->dataPtr);

    if (cmp < 0)
        root->left = _delete(root->left, keyPtr, dataOutPtr, compare);
    else if (cmp > 0)
        root->right = _delete(root->right, keyPtr, dataOutPtr, compare);

    // 삭제되어야 할 노드를 찾은 경우
    else {
        *dataOutPtr = root->dataPtr;

        // 해당 노드에게 자식 노드가 1개 이하인 경우
        if (root->left == NULL) {
            NODE*temp = root->right;
            free(root);
            return temp;
            }
        else if (root->right == NULL) {
            NODE*temp = root->left;
            free(root);
            return temp;
        } 

        // 해당 노드에게 자식 노드가 2개인 경우    
        // 오른쪽 서브트리에서 가장 하단의 작은 값을 삭제 
        NODE *temp = root->right;
        while (temp && temp->left != NULL)
            temp = temp->left;
        
        root->dataPtr = temp->dataPtr;
        root->right = _delete(root->right, temp->dataPtr, dataOutPtr, compare);
    }

    return root;
};


// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
// 완료
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)) {

    if (!root)
        return NULL;

    int cmp = compare(keyPtr, root->dataPtr);

    if (cmp == 0) 
        return root;
    else if (cmp < 0)
        return _search(root->left, keyPtr, compare); // 왼쪽 서브트리 탐색
    else 
        return _search(root->right, keyPtr, compare); // 오른쪽 서브트리 탐색
};


// used in BST_Traverse
// 완료
static void _traverse( NODE *root, void (*callback)(const void *)) {

    if (root != NULL) {
        _traverse(root->left, callback);
        callback(root->dataPtr);
        _traverse(root->right, callback);
    }
};

// used in BST_TraverseR
// 완료
static void _traverseR( NODE *root, void (*callback)(const void *)) {

    if (root != NULL) {
        _traverse(root->right, callback);
        callback(root->dataPtr);
        _traverse(root->left, callback);
    }
};

// used in printTree
// 완료
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)) {
    
    if (root != NULL) {
        _inorder_print(root->right, level + 1, callback);

        for (int i = 0; i < level; i++)
            printf("\t");

        callback(root->dataPtr);
        _inorder_print(root->left, level + 1, callback);
    }
};


// externel functions (custom)
/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
// 완료
TREE *BST_Create( int (*compare)(const void *, const void *)) {

    TREE *newTree = (TREE*)malloc(sizeof(TREE));

    if (newTree == NULL) 
        return NULL; // overflow

    else {
    // 초기화
    newTree->count = 0;
    newTree->root = NULL;
    newTree->compare = compare;
    }

    return newTree;
};

/* Deletes all data in tree and recycles memory
*/
// 완료
void BST_Destroy( TREE *pTree, void (*callback)(void *)) {

    if (pTree == NULL) return;

    _destroy(pTree->root, callback);
    free(pTree);
};

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
// 완료
int BST_Insert( TREE *pTree, void *dataInPtr) {

    // dataInPtr 이 담긴 새로운 노드 생성
    NODE *newNode = _makeNode(dataInPtr);

    if (newNode == NULL)
        return 0; // overflow (동적 메모리 할당 과정에서 실패)

    // 트리가 비어있는 경우
    if (pTree->root == NULL)
        pTree->root = newNode;

    else {
        _insert(&(pTree->root), newNode, pTree->compare);
    }

    pTree->count++;
    return 1; // success
};

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
// 완료 (오류 시 수정 필요)
void *BST_Delete( TREE *pTree, void *keyPtr) {
    
    if (pTree == NULL || pTree->root == NULL) 
        return NULL;
    
    void *dataOutPtr = NULL;
    pTree->root = _delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);

    if (dataOutPtr != NULL)
        pTree->count--;
    
    return dataOutPtr;
};


/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
    발견된 노드의 주소를 포인터로 넘겨받는다.
*/
// 완료
void *BST_Search( TREE *pTree, void *keyPtr) {
    
    // 트리가 비었을 경우
    if (pTree == NULL)
        return NULL;

    NODE *foundNode = _search(pTree->root, keyPtr, pTree->compare);
    if (foundNode == NULL) return NULL; // not found
    else return foundNode->dataPtr; // found
};


/* prints tree using inorder traversal (중위순회, 왼쪽 - 자기자신 - 오른쪽)
*/
// 완료
void BST_Traverse( TREE *pTree, void (*callback)(const void *)) {

    NODE* current = pTree->root;
    _traverse(current, callback);
};

/* prints tree using right-to-left inorder traversal (중위순회, 오른쪽 - 자기자신 - 왼쪽)
*/
// 완료
void BST_TraverseR( TREE *pTree, void (*callback)(const void *)) {

    NODE* current = pTree->root;
    _traverseR(current, callback);
};

/* Print tree using right-to-left inorder traversal with level
*/
// 완료
void printTree( TREE *pTree, void (*callback)(const void *)) {
    
  _inorder_print(pTree->root, 0, callback);  
};

/*
	returns number of nodes in tree
*/
// 완료
int BST_Count( TREE *pTree) { 

    return pTree->count;
}; 
