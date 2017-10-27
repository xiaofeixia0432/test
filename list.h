# ifndef __LIST_H__
# define __LIST_H__

#include <stdlib.h>
#include <stdio.h>

/*
This file define the single list common function,

you should contain the struct list_node in your userdefined struct which you want to make a single list with.
*/


#ifndef offset_of
    #define offset_of(TYPE, MEMBER) ((unsigned long)&((TYPE*)0)->MEMBER)
#endif

# ifndef container_of
# define container_of(ptr, type, member) ({ \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offset_of(type, member)); })
# endif

typedef struct list_node
{
    struct list_node* next;
    struct list_node** pprev;
} LIST_NODE, *PLIST_NODE;

typedef struct list_head
{
    struct list_node* first;
} LIST_HEAD, *PLIST_HEAD;

# define LIST_HEAD_INIT {.first = NULL}
# define LIST_HEAD(name) LIST_HEAD name = LIST_HEAD_INIT
# define INIT_LIST_HEAD(ptr) ((ptr)->first = NULL)

/*

para:
    node: a node need to be initlized;

fun: initlize a node to NULL
*/

static inline void INIT_LIST_NODE(PLIST_NODE node)
{
    if (!node)
    {
        return;
    }

    node->next = NULL;
    node->pprev = NULL;
}

/*
para:
    node: the node need to judge;
fun: return true if the node had been put into a list, else return false
*/

static inline int list_unhashed(const PLIST_NODE node)
{
    return (!node ? 1 : !(node->pprev));
}

/*
para:
    head: the list which need to be judged

return:
    return true if the list is empty, else return false
*/
static inline int list_empty(const PLIST_HEAD head)
{
    return (head ? (!head->first) : 1);
}

/*
para:
    node: the node need to be deleted
*/
static inline void __list_del(PLIST_NODE node)
{
    PLIST_NODE  next;
    PLIST_NODE* pprev;

    if (!node)
    {
        return;
    }

    next = node->next;
    pprev = node->pprev;
    (*pprev) = next;

    if (next)
    {
        next->pprev = pprev;
    }
}

static inline void list_del(PLIST_NODE node)
{
    if (!node || list_unhashed(node))
    {
        return;
    }

    __list_del(node);
    node->next = NULL;
    node->pprev = NULL;
}

/*
para:
    head: the list to be inserted into
    node: the node to be inserted
fun: add a new node into the list head
*/
static inline void list_add_head(PLIST_HEAD head, PLIST_NODE node)
{
    PLIST_NODE      first;

    if (!head || !node)
    {
        return;
    }

    first = head->first;
    node->next = first;

    if (first)
    {
        first->pprev = &node->next;
    }

    head->first = node;
    node->pprev = &head->first;
}

/*
para:
    node: the new node to be inserted
    next: the new node inserted before this node
fun: add a new node before the "next" node
*/
static inline void list_add_before(PLIST_NODE node, PLIST_NODE next)
{

    if (!node || !next)
    {
        return;
    }

    node->next = next;
    node->pprev = next->pprev;
    next->pprev = &node->next;
    *(node->pprev) = node;
}

/*
para:
    node: the node which had been in the list
    next: the new node which will be insert into the list
fun: add the "next" node after the "node" node
*/
static inline void list_add_after(PLIST_NODE node, PLIST_NODE next)
{
    if (!node || !next)
    {
        return;
    }

    next->next = node->next;
    next->pprev = &node->next;

    if (node->next)
    {
        node->next->pprev = &next->next;
    }

    node->next = next;
}

/*
para:
    old: the old list head
    new: the new list head
fun: move the list from the old list header to the new list header
*/
static inline void list_move_list(PLIST_HEAD old, PLIST_HEAD new)
{
    new->first = old->first;

    if (new->first)
    {
        new->first->pprev = &new->first;
    }

    old->first = NULL;
}

static inline PLIST_NODE list_traversal_excute(PLIST_HEAD head,
        int excute(PLIST_NODE, void*), void* data)
{
    PLIST_NODE          node = NULL;

    if (!head || !excute)
    {
        return NULL;
    }

    node = head->first;

    while (node)
    {
        if (0 == (excute(node, data)))
        {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

static inline PLIST_NODE list_find_node(PLIST_HEAD head, int compare(PLIST_NODE,
                                        void*), void* data)
{
    if (!head || !compare)
    {
        return NULL;
    }

    return list_traversal_excute(head, compare, data);
}

static inline void list_add_tail(PLIST_HEAD head, PLIST_NODE node)
{
    PLIST_NODE                  pre = NULL;

    if (!head || !node)
    {
        return;
    }

    if (list_empty(head))
    {
        list_add_head(head, node);
    }
    else
    {
        pre = head->first;

        while (pre->next)
        {
            pre = pre->next;
        }

        list_add_after(pre, node);
    }
}

# endif
