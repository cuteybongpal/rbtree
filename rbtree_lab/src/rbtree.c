#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

int insert_case1(rbtree* t, node_t* node);
int insert_case2(rbtree* t, node_t* node);
int insert_case3(rbtree* t, node_t* node);
void printTree(node_t* node);
rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    return p;
}
void delete_rbtree_nodes(node_t* node)
{
    if (node == NULL)
        return;
    delete_rbtree_nodes(node->left);
    delete_rbtree_nodes(node->right);
    free(node);
}

//차라리 한쪽으로 계속 회전하면서 삭제를 하면 될 거 같긴 한데
//재귀가 더 쉬울 거 같음
void delete_rbtree(rbtree *t) {
    delete_rbtree_nodes(t->root);
    free(t);
}
void check_insert_case(rbtree* tree, node_t* node)
{
    if (node == NULL)
        return;
    check_insert_case(tree, node->left);
    check_insert_case(tree, node->right);

    if (insert_case1(tree, node))
        return;
    if (!insert_case3(tree, node))
    {
        insert_case2(tree, node);
        insert_case3(tree, node);
    }

}
node_t *rbtree_insert(rbtree *t, const key_t key) {
    if (t->root == NULL)
    {
        node_t* n = (node_t*)malloc(sizeof(node_t));
        t->root = n;
        n->color = RBTREE_BLACK;
        n->key = key;
        return t->root;
    }
    node_t* node = (node_t*)calloc(1, sizeof(node_t));
    node->key = key;
    node->color = RBTREE_RED;
    node_t* cur = t->root;
    
    while (1)
    {
        if (cur->key >= key)
        {
            if (cur->left == NULL)
            {
                cur->left = node;
                node->parent = cur;
                break;
            }
            cur = cur->left;

        }
        else
        {
            if (cur->right == NULL)
            {
                cur->right = node;
                node->parent = cur;
                break;
            }
            cur = cur->right;
        }
    }
    
    printTree(t->root);
    check_insert_case(t, t->root);
    printf("\n");
    printTree(t->root);
    return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t* cur = t->root;

    while (cur != NULL)
    {
        if (cur->key == key)
            break;
        if (cur->key < key)
            cur = cur->right;
        else
            cur = cur->left;
    }
    return cur;
}


node_t *rbtree_min(const rbtree *t) {
    node_t* cur = t->root;
    while (cur->left != NULL)
    {
        cur = cur->left;
    }
    return cur;
}

node_t *rbtree_max(const rbtree *t) {
    node_t* cur = t->root;
    while (cur->right != NULL)
    {
        cur = cur->right;
    }
    return cur;
}
node_t* rbtree_node_erase(node_t** root, key_t key)
{
    if (*root == NULL)
        return NULL;
    if ((*root)->key > key)
        (*root)->left = rbtree_node_erase(&((*root)->left), key);
    else if ((*root)->key < key)
        (*root)->right = rbtree_node_erase(&((*root)->right), key);
    else
    {
        //case1 : 자식이 없을때
        if ((*root)->left == NULL && (*root)->right == NULL)
        {
            free(*root);
            *root = NULL;
            return NULL;
        }
        //case2 : 자식이 한쪽에만 있을 때
        else if ((*root)->left == NULL && (*root)->right != NULL)
        {
            node_t* right = (*root)->right;
            right->parent = (*root)->parent;
            free(*root);
            *root = NULL;
            return right;
        }
        else if ((*root)->left != NULL && (*root)->right == NULL)
        {
            node_t* left = (*root)->left;
            left->parent = (*root)->parent;
            free(*root);
            *root = NULL;
            return left;
        }
        //case3 : 자식이 두개가 있을 때
        else
        {
            // 후계자
            node_t* successor = (*root)->right;
            while (successor->left != NULL)
            {
                successor = successor->left;
            }
            if (successor != (*root)->right)
            {
                successor->parent->left = successor->right;
                if (successor->right != NULL)
                    successor->right->parent = successor->parent;

            }
            successor->parent = (*root)->parent;
            if ((*root)->parent != NULL)
            {
                if ((*root)->parent->left == (*root))
                {
                    (*root)->parent->left = successor;
                }
                else
                {
                    (*root)->parent->right = successor;
                }
            }
            successor->left = (*root)->left;
            successor->left->parent = successor;
            if ((*root)->right != successor)
                successor->right = (*root)->right;
            if (successor->right != NULL)
                successor->right->parent = successor;
            free(*root);
            *root = NULL;
            return successor;
        }
    }
    return *root;
}
//삭제는 우선 노드를 삭제하고,
//왼쪽 자식을 루트로 올리면 될듯함.
//만약 왼쪽 자식이 오른쪽 자식을 가지고 있다면,
//새로운 오른쪽 자식의 왼쪽으로 쭉 땡기면 될듯함.
//그리고 새로운 루트가 전 루트의 왼쪽 자식인지 오른쪽 자식인지 몰라서 이건 if문으로 분기 해야 할듯?
int rbtree_erase(rbtree *t, node_t *p) {
    //일단 재귀 함수 만들기
    rbtree_node_erase(&(t->root), p->key);
    return 0;
}
void rbtree_into_array(node_t* node, key_t* arr, size_t n, size_t* idx)
{
    if (node == NULL)
        return;
    if (*idx >= n)
        return;
    rbtree_into_array(node->left, arr, n, idx);
    if (*idx >= n)
        return;
    arr[*idx] = node->key;
    *idx = *idx + 1;
    rbtree_into_array(node->right, arr, n, idx);
    
}
//이것 또한 재귀로 풀어야 할듯? 아니면 아예 회전을 계속 시켜서 가장 편향된 트리로 만든 후,
//순서대로 배열에 담던가 해야하는데 재귁가 더 편해서 재귀로 하는게 나을듯 함.
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    rbtree_into_array(t->root, arr, n, 0);
    return 0;
}

int insert_case1(rbtree* t, node_t* node)
{
    if (node->parent == NULL)
        return 0;
    if (node->parent->parent == NULL)
        return 0;
    //부모 노드
    node_t* p = node->parent;
    node_t* gp = node->parent->parent;
    //삼촌 노드gt
    node_t* u = NULL;
    //삼촌 노드 가져오기
    if (gp->right == p)
        u = gp->left;
    else
        u = gp->right;
    
    //색깔 확인
    if (u == NULL)
        return 0;
    color_t pc = p->color;
    color_t uc = RBTREE_BLACK;
    if (u != NULL)
        uc = u->color;
    
    if (pc == RBTREE_BLACK)
        return 0;
    if (uc == RBTREE_BLACK)
        return 0;
    //만약 둘다 red라면, 삼촌과 부모를 black으로 바꿔주고, 조부모를  red를 바꿈
    if (pc == RBTREE_RED && uc == RBTREE_RED)
    {
        p->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        p->parent = RBTREE_RED;
    }
    //루트를 black으로 만듦. 조부모가 루트일 수도 있기 때문임.
    t->root->color = RBTREE_BLACK;
    return 1;
}
int insert_case2(rbtree* t, node_t* node)
{
    if (node->parent == NULL)
        return 0;
    if (node->parent->parent == NULL)
        return 0;
    node_t* gp = node->parent->parent;
    if (gp->left != NULL && gp->right != NULL)
        return 0;
    if (node->parent->left != NULL && node->parent->right != NULL)
        return 0;
    
    //왼쪽으로 회전.
    if (gp->left != NULL && gp->left->right == node)
    {
        node_t* temp = node->left;
        node_t* newL = node->parent;
        node_t* newP = gp;

        node->parent = newP;
        node->left = newL;
        newL->parent = node;
        newP->left = node;
        
        newL->right = temp;
        if (temp != NULL)
            temp->parent = newL;
        
        return 1;
    }

    //오른쪽으로 회전.
    if (gp->right != NULL && gp->right->left == node)
    {
        node_t* temp = node->right;

        node_t* newP = gp;
        node_t* newR = node->parent;

        node->parent = newP;
        node->right = newR;
        newR->parent = node;
        newP->right = node;
        newR->left = temp;
        if (temp != NULL)
            temp->parent = newR;

        return 1;
    }
    return 0;
}
int insert_case3(rbtree* t, node_t* node)
{
    if (node->parent == NULL)
        return 0;
    if (node->parent->parent == NULL)
        return 0;
    node_t* p = node->parent;
    node_t* gp = node->parent->parent;

    if (gp->left != NULL && gp->right != NULL)
        return 0;
    if (p->left != NULL && p->right != NULL)
        return 0;
    //todo :  왼쪽 회전과 오른쪽 회전 구현하기
    //우선 p랑 gp의 색깔을 서로 바꿔야함.
    color_t color_temp = p->color;
    p->color = gp->color;
    gp->color = color_temp;
    //오른쪽 회전
    if (gp->left != NULL && gp->left->left == node)
    {
        node_t* newR = gp;
        node_t* newP = gp->parent;
        node_t* temp = p->right;
        //회전
        p->parent = newP;
        p->right = newR;
        gp->parent = p;
        //temp를 회전 당한 얘의 left로 붙여줌
        gp->left = temp;
        if (temp != NULL)
            temp->parent = gp;
        if (newP == NULL)
            t->root = p;
        else
            if (newP->left == gp)
                newP->left = p;
            else
                newP->right = p;
        return 1;
    }
    //왼쪽 회전
    else if (gp->right != NULL && gp->right->right == node)
    {
        node_t* newL = gp;
        node_t* newP = gp->parent;
        node_t* temp = p->left;

        p->parent = newP;
        p->left = newL;
        gp->parent = p;

        gp->right = temp;
        if (temp != NULL)
            temp->parent = gp;
        if (newP == NULL)
            t->root = p;
        else
            if (newP->left == gp)
                newP->left = p;
            else
                newP->right = p;
        return 1;
    }
    return 0;
}

void printTree(node_t* node)
{
    if (node == NULL)
    {
        printf("print NULL\n");
        return;
    }
    printf("print %d \n", node->key);
    printTree(node->left);
    printTree(node->right);
}