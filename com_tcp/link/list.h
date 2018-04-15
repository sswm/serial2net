#ifndef __LIST_H
#define __LIST_H

/* This file is from Linux Kernel (include/linux/list.h)
* and modified by simply removing hardware prefetching of list items.
* Here by copyright, credits attributed to wherever they belong.
* Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
*/

/*
* Simple doubly linked list implementation.
*
* Some of the internal functions (“__xxx”) are useful when
* manipulating whole lists rather than single entries, as
* sometimes we already know the next/prev entries and we can
* generate better code by using them directly rather than
* using the generic single-entry routines.
*/

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#if 0
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif
/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200

struct list_head
{
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

//定义一个内核链表并初始化
#define LIST_HEAD(name) \
struct list_head name = LIST_HEAD_INIT(name)

//内核链表的头节点初始化
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


/*
* Insert a newLink entry between two known consecutive entries.
*
* This is only for internal list manipulation where we know
* the prev/next entries already!
*/
//内核链表的插入
static inline void __list_add (struct list_head *newLink, struct list_head *prev, struct list_head *next)
{
    next->prev = newLink;
    newLink->next = next;
    newLink->prev = prev;
    prev->next = newLink;
}

/**
* list_add – add a newLink entry
* @newLink: newLink entry to be added
* @head: list head to add it after
*
* Insert a newLink entry after the specified head.
* This is good for implementing stacks.
*/
//加入到头节点的下一个
static inline void list_add (struct list_head *newLink, struct list_head *head)
{
    __list_add (newLink, head, head->next);
}

/**
* list_add_tail – add a newLink entry
* @newLink: newLink entry to be added
* @head: list head to add it before
*
* Insert a newLink entry before the specified head.
* This is useful for implementing queues.
*/
//加入到 头节点的尾部  或者上一个
static inline void list_add_tail (struct list_head *newLink, struct list_head *head)
{
    __list_add (newLink, head->prev, head);
}

/*
* Delete a list entry by making the prev/next entries
* point to each other.
*
* This is only for internal list manipulation where we know
* the prev/next entries already!
*/
//删除某个节点
static inline void __list_del (struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/**
* list_del – deletes entry from list.
* @entry: the element to delete from the list.
* Note: list_empty on entry does not return true after this, the entry is in an undefined state.
*/
//删除entry节点 并使他的 next和perv 指向NULL 如果需要释放 则自行设置
static inline void list_del (struct list_head *entry)
{
	__list_del (entry->prev, entry->next);
    entry->next = 0;
    entry->prev = 0;
}

/**
* list_del_init – deletes entry from list and reinitialize it.
* @entry: the element to delete from the list.
*/
//删除entry节点  并使entry初始化为头节点 指向自己
static inline void list_del_init (struct list_head *entry)
{
	__list_del (entry->prev, entry->next);
	INIT_LIST_HEAD (entry);
}

/**
* list_move – delete from one list and add as another’s head
* @list: the entry to move
* @head: the head that will precede our entry
*/
//删除list 并将 list 加入到head的头节点之后
static inline void list_move (struct list_head *list, struct list_head *head)
{
	__list_del (list->prev, list->next);
	list_add (list, head);
}

/**
* list_move_tail – delete from one list and add as another’s tail
* @list: the entry to move
* @head: the head that will follow our entry
*/

//删除list 并将 list 加入到head的头节点的尾部
static inline void list_move_tail (struct list_head *list, struct list_head *head)
{
	__list_del (list->prev, list->next);
	list_add_tail (list, head);
}

/**
* list_empty – tests whether a list is empty
* @head: the list to test.
*/
//返回头节点的下一个地址
static inline int list_empty (struct list_head *head)
{
	return head->next == head;
}

//拼接两个互不相关的链表 
static inline void __list_splice (struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
* list_splice – join two lists
* @list: the newLink list to add.
* @head: the place to add it in the first list.
*/
//拼接两个互不相关的链表 
static inline void list_splice (struct list_head *list, struct list_head *head)
{
	if (!list_empty (list))
		__list_splice (list, head);
}

/**
* list_splice_init – join two lists and reinitialise the emptied list.
* @list: the newLink list to add.
* @head: the place to add it in the first list.
*
* The list at @list is reinitialised
*/

//拼接两个互不相关的链表 并初始化list
static inline void list_splice_init (struct list_head *list, struct list_head *head)
{
	if (!list_empty (list)) {
		__list_splice (list, head);
		INIT_LIST_HEAD (list);//重新让list 指回自己避免出错
	}
}

/**
* list_entry – get the struct for this entry
* @ptr:    the &struct list_head pointer.
* @type:    the type of the struct this is embedded in.
* @member:    the name of the list_struct within the struct.
*/
//返回type类型中有链表所在结构体的结构体地址
#define list_entry(ptr, type, member) \
((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
* list_for_each    -    iterate over a list
* @pos:    the &struct list_head to use as a loop counter.
* @head:    the head for your list.
*/
//循环链表
#define list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); \
pos = pos->next)

/**
* list_for_each_prev    -    iterate over a list backwards
* @pos:    the &struct list_head to use as a loop counter.
* @head:    the head for your list.
*/
//从尾部循环
#define list_for_each_prev(pos, head) \
for (pos = (head)->prev; pos != (head); \
pos = pos->prev)

/**
* list_for_each_safe    -    iterate over a list safe against removal of list entry
* @pos:    the &struct list_head to use as a loop counter.
* @n:        another &struct list_head to use as temporary storage
* @head:    the head for your list.
*/
//两个指针来循环pos在n的后面  /可删除链表中的某个
#define list_for_each_safe(pos, n, head) \
for (pos = (head)->next, n = pos->next; pos != (head); \
pos = n, n = pos->next)

/**
* list_for_each_entry    -    iterate over list of given type
* @pos:    the type * to use as a loop counter.
* @head:    the head for your list.
* @member:    the name of the list_struct within the struct.
*/
//将一个大的含有内核链表的结构体 按照内核链表来循环
#define list_for_each_entry(pos, head, member)                \
for (pos = list_entry((head)->next, typeof(*pos), member);    \
&pos->member != (head);                     \
pos = list_entry(pos->member.next, typeof(*pos), member))

/**
* list_for_each_entry_safe – iterate over list of given type safe against removal of list entry
* @pos:    the type * to use as a loop counter.
* @n:        another type * to use as temporary storage
* @head:    the head for your list.
* @member:    the name of the list_struct within the struct.
*/
//将一个大的含有内核链表的结构体 按照内核链表来循环  设置两个指针
#define list_for_each_entry_safe(pos, n, head, member)            \
for (pos = list_entry((head)->next, typeof(*pos), member),    \
n = list_entry(pos->member.next, typeof(*pos), member);    \
&pos->member != (head);                     \
pos = n, n = list_entry(n->member.next, typeof(*n), member))

#endif
