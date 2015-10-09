#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

typedef struct list_node list_node;
struct list_node
{
    list_node *prev;
    list_node *next;
};
typedef list_node list_head;

#define LIST_HEAD(name) list_head name = {&name, &name}

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr)->prev = (ptr); \
} while (0)

/**
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

static __inline void __list_link(list_node *prev, list_node *next)
{
    next->prev = prev;
    prev->next = next;
}

static __inline void __list_add(list_head *node,
                                list_head *prev,
                                list_head *next)
{
    __list_link(node, next);
    __list_link(prev, node);
}

static __inline void list_add(list_head *head, list_node *node)
{
    __list_add(node, head, head->next);
}

static __inline void list_add_tail(list_head *head, list_head *node)
{
    __list_add(node, head->prev, head);
}

static __inline void list_del(list_node *node)
{
    __list_link(node->prev, node->next);
}

static __inline int list_empty(const list_head *head)
{
    return head->next == head;
}

/**
 * list_for_each    -   iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

 /**
 * list_for_each_prev   -   iterate over a list backwards
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe   -   iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

static __inline int count_list(list_head *l)
{
    int count = 0;
    list_node *pos;

    list_for_each(pos, l) {
        count++;
    }
    return count;
}

static __inline void rotate_list(list_head *head, int count)
{
    list_node *n = head->next;

    list_del(head);
    while(count--) {
        n = n->next;
    }
    list_add(head, n);
}

#define list_find(head, item, cmpfn, cmparg, pos) \
{ \
    list_for_each(pos, (head)) \
        if (cmpfn(pos, item, cmparg)) { break; } \
    if (pos == head) pos = NULL; \
}

#endif // __LIST_H__
