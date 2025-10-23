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
    //삽입한 노드부터  rb트리의 규칙을 만족하지 못한 노드를 수정함.
    //케이스3을 만족해, 회전을 하게 되면 무조건 규칙에 맞게 조정 되기 때문에, 바로 break를 걸어줌.
    while (node->parent != NULL && node->parent->color == RBTREE_RED) {
        if (insert_case1(t, node))
        {
            node = node->parent->parent;
            continue;
        }
        node_t* p = node->parent;
        if (insert_case2(t, node))
        {
            node = p;
            p = node->parent;
        }
        if (insert_case3(t, node))
            break;
    }
    
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
//다시 봤는데 그냥 후계자를 삭제할 노드로 올려야 하더라
//재귀로 삭제 하려고 했는데 규칙에 위반하지 않는지 확인하려면 재귀가 아니라,
//while문으로 검사를 해야함.
int rbtree_erase(rbtree *t, node_t *p) {
    color_t c = p->color;
    if (p->left == NULL && p->right)
    {
        node_t* parent =p->parent;
        if (p->parent->left == p)
            p->parent->left = NULL;
        else
            p->parent->right = NULL;
        free(p);
    }
    else if (p->left == NULL && p->right != NULL)
    {
        node_t* newP = p->parent;
        node_t* newR = p->right;
        newR->parent = newP;
        if (t->root != p)
        {
            if (newP->left == p)
                newP->left = newR;
            else
                newP->right = newR;
        }
        else
        {
            t->root = p->right;
        }
        free(p);
    }
    else if (p->left != NULL && p->right == NULL)
    {
        node_t* newP = p->parent;
        node_t* newL = p->left;
        newL->parent = newP;
        if (newP->left == p)
            newP->left = newL;
        else
            newP->right = newL;
        free(p);
    }
    else
    {
        node_t* successor = p->right;
        while (successor->left != NULL)
        {
            successor = successor->left;
        }
        //후계자의 부모와, 오른쪽 자식, 왼쪽 자식 연결 시켜주기
        node_t* newL = p->left;
        node_t* newR = p->right;
        node_t* newP = p->parent;
        color_t color = p->color;

        
        //후계자가 오른쪽 자식일때, 왼쪽 자식만 연결해주면 됨.
        if (successor == newR)
        {
            successor->parent = newP;
            if (newP->left == p)
                newP->left = successor;
            else
                newP->right = successor; 
            successor->left = newL;
            newL->parent = successor;
        }
        else
        {
            successor->parent->left = NULL;
            successor->parent = newP;
            if (newP->left == p)
                newP->left = successor;
            else
                newP->right = successor; 
            successor->left = newL;
            successor->right = newR;

            newL->parent = successor;
             newR->parent = successor;
        }
        c = successor->color;
        successor->color = color;
        free(p);
    }
    //삭제되는 색깔이 빨강일 경우, 아무런 어떠한 속정도 위반하지 않기 때문에 바로 리턴해줌.
    if (c == RBTREE_RED)
        return 0; 
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
        p->parent->color = RBTREE_RED;
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
    if (gp->left == node->parent)
    {
        if (gp->right != NULL && gp->right->color == RBTREE_RED)
            return 0;
    }
    else
    {
        if (gp->left != NULL && gp->left->color == RBTREE_RED)
            return 0;
    }
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
    //삼촌 노드가 빨강일때 return함
    if (gp->left == p)
    {
        if (gp->right != NULL && gp->right->color == RBTREE_RED)
            return 0;
    }
    else
    {
        if (gp->left != NULL && gp->left->color == RBTREE_RED)
            return 0;
    }
    //todo :  왼쪽 회전과 오른쪽 회전 구현하기
    
    //오른쪽 회전
    if (gp->left != NULL && gp->left->left == node)
    {
        //우선 p랑 gp의 색깔을 서로 바꿔야함.
        color_t color_temp = p->color;
        p->color = gp->color;
        gp->color = color_temp;

        node_t* newR = gp;
        node_t* newP = gp->parent;
        node_t* temp = p->right;
        //회전
        p->parent = newP;
        p->right = newR;
        newR->parent = p;
        //temp를 회전 당한 얘의 left로 붙여줌
        newR->left = temp;
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
        //우선 p랑 gp의 색깔을 서로 바꿔야함.
        color_t color_temp = p->color;
        p->color = gp->color;
        gp->color = color_temp;
        node_t* newL = gp;
        node_t* newP = gp->parent;
        node_t* temp = p->left;

        p->parent = newP;
        p->left = newL;
        newL->parent = p;

        newL ->right = temp;
        if (temp != NULL)
            temp->parent = gp;
        if (newP == NULL)
            t->root = p;
        else
            if (p->parent->left == gp)
                newP->left = p;
            else
                newP->right = p;
        return 1;
    }
    return 0;
}

int del_case1()
{

}

int del_case2()
{
    
}

int del_case3()
{
    
}

int del_case4()
{
    
}
void printTree(node_t* node)
{
    if (node == NULL)
    {
        printf("print NULL\n");
        return;
    }
    if (node->color == RBTREE_BLACK)
        printf("print %d Black\n", node->key);
    else
        printf("print %d Red\n", node->key);
    printTree(node->left);
    printTree(node->right);
}
//회전의 기준이 되는 노드 루트 노드임
void rotateLeft(rbtree* t, node_t* node)
{
    node_t* replaceNode = node->right;
    node_t* newP = node->parent;
    node_t* newL = node;
    node_t* temp = node->right->left;

    if (newP == NULL)
        t->root = replaceNode;
    else
    {
        if (newP->left == node)
            newP->left = replaceNode;
        else
            newP->right = replaceNode;
    }
    
    replaceNode->left = newL;
    newL->parent = replaceNode;
    newL->right = temp;

    if (temp != NULL)
        temp->parent = newL;


}

void rotateRight(rbtree* t, node_t* node)
{
    node_t* replaceNode = node->right;
    node_t* newP = node->parent;
    node_t* newR = node;
    node_t* temp = node->right->left;

    if (newP == NULL)
        t->root = replaceNode;
    else
    {
        if (newP->left == node)
            newP->left = replaceNode;
        else
            newP->right = replaceNode;
    }
    replaceNode->right = newR;
    newR->parent = replaceNode;
    newR->left = temp;

    if (temp != NULL)
        temp->parent = newR;
}