#define BALANCING

#include <stdlib.h> // malloc
#include <stdio.h>

#include "avlt.h"

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// 내부함수 원형 선언
// internal functions (not mandatory)
static NODE *_insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *));
static NODE *_makeNode( void *dataInPtr);
static void _destroy( NODE *root, void (*callback)(void *));
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *));
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *));
static void _traverse( NODE *root, void (*callback)(const void *));
static void _traverseR( NODE *root, void (*callback)(const void *));
static void _inorder_print( NODE *root, int level, void (*callback)(const void *));
static int getHeight( NODE *root);
static NODE *rotateRight( NODE *root);
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////

// 추가
static int height(NODE *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// 추가
static int getBalance(NODE *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// 추가
static NODE *minValueNode(NODE *node) {
    NODE *current = node;

    // leftmost leaf 찾기
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}


// internal functions (not mandatory)
// used in AVLT_Insert
// return 	pointer to root
static NODE *_insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *)) {
    if (root == NULL) {
        return newPtr;
    }

    // BST 삽입과 동일
    if (compare(newPtr->dataPtr, root->dataPtr) < 0) {
        root->left = _insert(root->left, newPtr, compare);
    } else if (compare(newPtr->dataPtr, root->dataPtr) > 0) {
        root->right = _insert(root->right, newPtr, compare);
    } else {
        // 이미 데이터가 존재할 경우, 중복 삽입하지 않는다. 
        return root;
    }

    // height 업데이트
    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    // Unbalanced 케이스
    // LL
    if (balance > 1 && compare(newPtr->dataPtr, root->left->dataPtr) < 0) {
        return rotateRight(root);
    }

    // RR
    if (balance < -1 && compare(newPtr->dataPtr, root->right->dataPtr) > 0) {
        return rotateLeft(root);
    }

    // LR
    if (balance > 1 && compare(newPtr->dataPtr, root->left->dataPtr) > 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // RL
    if (balance < -1 && compare(newPtr->dataPtr, root->right->dataPtr) < 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
};

// used in AVLT_Insert
static NODE *_makeNode( void *dataInPtr) {
    NODE *newNode = (NODE *)malloc(sizeof(NODE));
    if (newNode) {
        newNode->dataPtr = dataInPtr;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
    }
    return newNode;
};

// used in AVLT_Destroy
static void _destroy( NODE *root, void (*callback)(void *)) {
    if (root) {
        _destroy(root->left, callback);
        _destroy(root->right, callback);
        callback(root->dataPtr);
        free(root);
    }
};

// used in AVLT_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)) {
    if (root == NULL) {
        return root; // not found
    }

    // found
    if (compare(keyPtr, root->dataPtr) < 0) {
        root->left = _delete(root->left, keyPtr, dataOutPtr, compare);
    } else if (compare(keyPtr, root->dataPtr) > 0) {
        root->right = _delete(root->right, keyPtr, dataOutPtr, compare);
    } else {
        *dataOutPtr = root->dataPtr;

        // 자식 노드가 1개 이하
        if ((root->left == NULL) || (root->right == NULL)) {
            NODE *temp = root->left ? root->left : root->right;

            // No child
            if (temp == NULL) {
                // temp = root;
                free(root);
                root = NULL;
            } else {
                // One child 
                free(root);
                root = temp;
                // *root = *temp; 
            }
            // free(temp);
        } else {
            // Node with two children
            NODE *temp = minValueNode(root->right);
            NODE *tempParent = root;

            // Find parent of minValueNode
            while (tempParent->right != NULL && tempParent->right != temp) {
                tempParent = tempParent->right;
            }

            // Delete the minValueNode from its current position
            if (tempParent != root) {
                tempParent->right = temp->right;
            } else {
                root->right = temp->right;
            }

            // Replace root with minValueNode
            temp->left = root->left;
            temp->right = root->right;
            free(root);

            root = temp;
            root->height = 1 + max(height(root->left), height(root->right)); // Update height of new root
        
            /*
            // Node with two children
            NODE *temp = minValueNode(root->right);

            root->dataPtr = temp->dataPtr;
            temp->dataPtr = NULL; // 중복 free 방지
            root->right = _delete(root->right, temp->dataPtr, dataOutPtr, compare);
            */
        }
    }

    if (root == NULL) {
        return root;
    }

    // height 업데이트
    root->height = 1 + max(height(root->left), height(root->right));
    return root;

    /*

    문제 조건에 의하면 삭제 연산에서는 rebalancing 작업을 하지 않는다.

    int balance = getBalance(root);

    // LL
    if (balance > 1 && getBalance(root->left) >= 0) {
        return rotateRight(root);
    }

    // LR
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // RR
    if (balance < -1 && getBalance(root->right) <= 0) {
        return rotateLeft(root);
    }

    // RL
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
    */
}

// used in AVLT_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)) {
    // Base case
    if (root == NULL || compare(keyPtr, root->dataPtr) == 0) {
        return root;
    }

    if (compare(keyPtr, root->dataPtr) > 0) {
        return _search(root->right, keyPtr, compare);
    }

    return _search(root->left, keyPtr, compare);
};

// used in AVLT_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)) {
    if (root) {
        _traverse(root->left, callback);
        callback(root->dataPtr);
        _traverse(root->right, callback);
    }
};

// used in AVLT_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *)) {
    if (root) {
        _traverseR(root->right, callback);
        callback(root->dataPtr);
        _traverseR(root->left, callback);
    }
};

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)) {
    if (root != NULL) {
        _inorder_print(root->right, level + 1, callback);

        for (int i = 0; i < level; i++)
            printf("\t");

        callback(root->dataPtr);
        _inorder_print(root->left, level + 1, callback);
    }
};

// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight( NODE *root) {
    if (!root) return 0;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
};

// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes
// return	new root
static NODE *rotateRight( NODE *root) {
    NODE *x = root->left;
    NODE *T2 = x->right;

    // 회전
    x->right = root;
    root->left = T2;

    // height 업데이트
    root->height = max(height(root->left), height(root->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
};

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft( NODE *root) {
    NODE *y = root->right;
    NODE *T2 = y->left;

    // 회전
    y->left = root;
    root->right = T2;

    // height 업데이트
    root->height = max(height(root->left), height(root->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
};


////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *AVLT_Create( int (*compare)(const void *, const void *)) {
    TREE *tree = (TREE *)malloc(sizeof(TREE));
    if (tree) {
        tree->count = 0;
        tree->root = NULL;
        tree->compare = compare;
    }
    return tree;
};

/* Deletes all data in tree and recycles memory
*/
void AVLT_Destroy( TREE *pTree, void (*callback)(void *)) {
    if (pTree) {
        _destroy(pTree->root, callback);
        free(pTree);
    }
};

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVLT_Insert( TREE *pTree, void *dataInPtr) {
    if (!pTree) return 0; 

    NODE *newNode = _makeNode(dataInPtr);
    if (!newNode) return 0; // overflow

    pTree->root = _insert(pTree->root, newNode, pTree->compare);
    
    pTree->count++;
    return 1; // success
};

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Delete( TREE *pTree, void *keyPtr) {
    if (!pTree || !pTree->root) return NULL;

    void *dataOutPtr = NULL;
    pTree->root = _delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);

    if (dataOutPtr != NULL) {
        pTree->count--;
    }
    return dataOutPtr;
};

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Search( TREE *pTree, void *keyPtr) {
    if (pTree == NULL || pTree->root == NULL) {
        return NULL; 
    }

    NODE *foundNode = _search(pTree->root, keyPtr, pTree->compare);
    if (foundNode == NULL) {
        return NULL; // not found
    }

    return foundNode->dataPtr; 
};

/* prints tree using inorder traversal
*/
void AVLT_Traverse( TREE *pTree, void (*callback)(const void *)) {
    if (pTree) {
        _traverse(pTree->root, callback);
    }
};

/* prints tree using right-to-left inorder traversal
*/
void AVLT_TraverseR( TREE *pTree, void (*callback)(const void *)) {
    if (pTree) {
        _traverseR(pTree->root, callback);
    }
};

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)) {
    if (pTree == NULL || pTree->root == NULL) {
        return; 
    }
    _inorder_print(pTree->root, 0, callback); 
};

/* returns number of nodes in tree
*/
int AVLT_Count( TREE *pTree) {
    return pTree ? pTree->count : 0;
};

/* returns height of the tree
*/
int AVLT_Height( TREE *pTree) {
    return getHeight(pTree->root);
};
