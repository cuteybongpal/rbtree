#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

int insert_case1(rbtree* t, node_t* node);
int insert_case2(rbtree* t, node_t* node);
int insert_case3(rbtree* t, node_t* node);

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
    node_t* cur = t->root;
    while (1)
    {
        if (cur->key >= key)
        {
            if (cur->left == NULL)
                break;
            cur = cur->left;

        }
        else
        {
            if (cur->right == NULL)
                break;
            cur = cur->right;
        }
    }
    
    
    node_t* node = (node_t*)calloc(1, sizeof(node_t));
    //키 설정하고, color를 빨강으로 바꿔줌.
    if (key < cur->key)
        cur->left = node;
    else
        cur->right = node;
    
    node->key = key;
    node->color = RBTREE_RED;
    node->parent = cur;

    if (node->parent == NULL)
        return t->root;
    if (node->parent->parent == NULL)
        return t->root;
    
    int flag = 1;
    node_t* ccur = node;
    while (flag && cur != NULL)
    {
        int flag2 = insert_case2(t, ccur);
        int flag3 = insert_case3(t, ccur);
        int flag1 = insert_case1(t, ccur);
        flag = flag1 | flag2 | flag3;
        ccur = ccur->parent;
    }
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
//삭제는 우선 노드를 삭제하고,
//왼쪽 자식을 루트로 올리면 될듯함.
//만약 왼쪽 자식이 오른쪽 자식을 가지고 있다면,
//새로운 오른쪽 자식의 왼쪽으로 쭉 땡기면 될듯함.
//그리고 새로운 루트가 전 루트의 왼쪽 자식인지 오른쪽 자식인지 몰라서 이건 if문으로 분기 해야 할듯?
int rbtree_erase(rbtree *t, node_t *p) {
    //새로 연결할 오른쪽 자식, 부모, 왼쪽으로 노드를 옮기기때문에 없어질 오른쪽 자식은 temp로 저장.
    
    if (p->left != NULL)
    {
        node_t* temp = p->left->right;
        node_t* newP = p->parent;
        node_t* newR = p->right;
        node_t* cur = p->left;
        //왼쪽 자식을 위로 올려주는 작업
        cur->right = newR;
        cur->parent = newP;
        cur->right->parent = cur;
        
        
        if (newP != NULL)
        {
            if (newP->left == p)
                newP->left = cur;
            else
                newP->right = cur;
        }
        //원래 있던 오른쪽 자식을 새로운 오른쪽 자식의 가장 왼쪽 노드로 바꿔주는 코드
        cur = cur->right;
        while (cur->left != NULL)
        {
            cur = cur->left;
        }
        cur->left = temp;
        temp->parent = cur;
    }
    else if (p->right != NULL)
    {
        node_t* temp = p->right->left;
        node_t* newP = p->parent;
        node_t* newL = p->left;
        node_t* cur = p->left;
        //왼쪽 자식을 위로 올려주는 작업
        cur->left = newL;
        cur->parent = newP;
        cur->left->parent = cur;
        
        if (newP != NULL)
        {
            if (newP->left == p)
                newP->left = cur;
            else
                newP->right = cur;
        }
        //원래 있던 오른쪽 자식을 새로운 오른쪽 자식의 가장 왼쪽 노드로 바꿔주는 코드
        cur = cur->left;
        while (cur->right != NULL)
        {
            cur = cur->right;
        }
        cur->right = temp;
        temp->parent = cur;
    }
    else
    {
        if (p->parent != NULL)
        {
            if (p->parent->left == p)
                p->parent->left = NULL;
            else
                p->parent->right = NULL;
        }
    }
    //메모리 해제
    if (t->root == p)
        t->root = NULL;
    free(p);
    //todo : case 찾아서 whlie문 돌려서 어떻게든 하면 될듯?
    return 0;
}
void rbtree_into_array(node_t* node, key_t* arr, size_t n, int* idx)
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
    //삼촌 노드
    node_t* u = NULL;
    //삼촌 노드 가져오기
    if (p->parent->right == p)
        u = p->parent->left;
    else
        u = p->parent->right;
    //색깔 확인
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

    //왼쪽으로 회전.
    if (gp->left->right == node)
    {
        node_t* temp = node->left;

        node_t* newP = node->parent;
        node_t* newL = node;

        node = node->right;
        node->parent = newP;
        node->left = newL;
        newL->parent = node;
        //이제 temp만 어떻게든 처리하면 되는데 이걸 어떻게 해야 할지 고민이 됨
        //temp를 원래 루트의 가장 오른쪽에 넣으면 될듯함.
        node_t* cur = newL->right;
        while (cur->right == NULL)
        {
            cur = cur->right;
        }
        cur->left = temp;
        temp->parent = cur;
        return 1;
    }

    //오른쪽으로 회전.
    if (gp->right->left == node)
    {
        node_t* temp = node->right;

        node_t* newP = node->parent;
        node_t* newR = node;

        node = node->right;
        node->parent = newP;
        node->right = newR;
        newR->parent = node;
        node_t* cur = newR->left;
        while (cur->right == NULL)
        {
            cur = cur->right;
        }
        cur->right = temp;
        temp->parent = cur;
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
    node_t* gp = node->parent->parent;
    node = node->parent;
    if (gp->left->left == node)
    {
        color_t temp_c = node->parent->color;
        node->parent->color = gp->color;
        gp->color = temp_c;
        //오른쪽 회전

        node_t* temp = node->right;
        node_t* newP = gp->parent;
        node_t* newR = gp;
        node->parent = newP;
        node->right = newR;
        node->right->parent = node;
        node_t* cur = gp;
        while (cur->left == NULL)
        {
            cur = cur->left;
        }
        cur->left = temp;
        temp->parent = cur;
        return 1;
    }

    if (gp->right->right == node)
    {
        color_t temp_c = node->parent->color;
        node->parent->color = gp->color;
        gp->color = temp_c;
        //왼쪽 회전

        node_t* temp = node->left;
        node_t* newP = gp->parent;
        node_t* newL = gp;
        node->parent = newP;
        node->left = newL;
        node->left->parent = node;
        node_t* cur = gp;
        while (cur->right == NULL)
        {
            cur = cur->right;
        }
        cur->right = temp;
        temp->parent = cur;
        return 1;
    }
    return 0;
}