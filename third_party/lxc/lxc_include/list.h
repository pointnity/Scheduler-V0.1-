#ifndef _list_h
#define _list_h

struct lxc_list {
	void *elem;
	struct lxc_list *next;
	struct lxc_list *prev;

#define lxc_init_list(l) { .next = l, .prev = l }

#define lxc_list_for_each(__iterator, __list)		
	for (__iterator = (__list)->next;				\
	     __iterator != __list;					\
	     __iterator = __iterator->next)

static inline void lxc_list_init(struct lxc_list *list)
{
	list->elem = NULL;
	list->next = list->prev = list;
}

static inline void lxc_list_add_elem(struct lxc_list *list, void *elem)
{
	list->elem = elem;
}

static inline void *lxc_list_first_elem(struct lxc_list *list)
{
	return list->next->elem;
