#include <stdlib.h>
#include <string.h>
#include "xtelelib.h"

/* Debut xtele_list */
xtele_list* xtele_list_new(void) {
	return NULL;
}

xtele_list* xtele_list_element_new(void* data) {
	xtele_list* list;

	list = calloc(1, sizeof(xtele_list));
	list->data = data;

	return list;
}

void xtele_list_destroy_one(xtele_list* list, void (*free_func) (void*) ) {
	if(list) {
		if(free_func)
			free_func(list->data);
		free(list);
	}
}

void xtele_list_destroy(xtele_list* list, void (*free_func) (void*) ) {
	if(list) {
		xtele_list_destroy(list->next, free_func);
	}
	xtele_list_destroy_one(list, free_func);
}

xtele_list* xtele_list_last(xtele_list* list) {
	if(list)
		while(list->next)
			list = list->next;

	return list;
}

xtele_list* xtele_list_first(xtele_list* list) {
	if(list)
		while(list->prev)
			list = list->prev;

	return list;
}

xtele_list* xtele_list_insert(xtele_list* list, void* data) {
	xtele_list* new_member;

	new_member = xtele_list_element_new(data);
	if(list) {
		new_member->next = list;
		new_member->prev = list->prev;
		if(list->prev)
			list->prev->next = new_member;
		list->prev = new_member;
	}

	return new_member;
}

xtele_list* xtele_list_add(xtele_list* list, void* data) {
	xtele_list* new_member;

	new_member = xtele_list_element_new(data);
	if(list) {
		new_member->prev = list;
		new_member->next = list->next;
		if(list->next)
			list->next->prev = new_member;
		list->next = new_member;

		return xtele_list_first(list);
	}

	return new_member;
}

xtele_list* xtele_list_append(xtele_list* list, void* data) {
	return xtele_list_add(xtele_list_last(list), data);
}

xtele_list* xtele_list_remove_element(xtele_list* list,  void (*free_func) (void*)) {
	xtele_list* ret = NULL;

	if(list) {
		if(list->next)
			list->next->prev = list->prev;
		if(list->prev) {
			list->prev->next = list->next;
			ret = list->prev;
		} else 
			ret = list->next;			

		if(free_func)
			free_func(list->data);
		free(list);
	}
	return ret;
}

xtele_list* xtele_list_remove(xtele_list* list, int (*del_func) (void*, void*), void* data, void (*free_func) (void*)) {
		return xtele_list_remove_element(xtele_list_foreach(list, del_func, data), free_func);
}

xtele_list* xtele_list_foreach(xtele_list* debut, int (*foreach_func) (void*, void*), void* data) {
	while(debut) {
		if(foreach_func(debut->data, data))
			break;
		xtele_list_next(debut);
	} 
	return debut;
}

void* xtele_list_find(xtele_list* debut, int (*foreach_func) (void*, void*), void* data) {
	xtele_list* member;

	member = xtele_list_foreach(debut, foreach_func, data);
	if(member)
		return member->data;
	else
		return NULL;
}	

xtele_list* xtele_list_find_string(xtele_list* list, char* name) {
	return xtele_list_foreach(list, FOREACH_FUNC(streq), name);
}
/* Fin xtele_list */

/* Debut xtele_listb */
xtele_listb* xtele_listb_new(void* data) {
	xtele_listb* listb;
	
	listb = calloc(1, sizeof(xtele_listb));
	listb->data = data;
	listb->next = listb;
	listb->prev = listb;

	return listb;
}

#if 0
int xtele_listb_is_single(xtele_listb* list) {
	return list->next == list && list->prev == list;
}

void xtele_listb_to_list(xtele_listb* list) {
	list->prev->next = NULL;
	list->prev = NULL;
}

void xtele_listb_from_list(xtele_list* list) {
	xtele_list_first(list)->prev = xtele_list_last(list);
	xtele_list_last(list)->next = xtele_list_first(list);
}
#endif

xtele_listb* xtele_listb_insert(xtele_listb* list, void* data) {
	xtele_listb* new;

	new = xtele_listb_new(data);
	if(list) {
		new->next = list->next;
		new->prev = list;
		list->next->prev = new;
		list->next = new;
		return list;
	} else {
		return new;
	}
}

#if 0
xtele_listb* xtele_listb_remove(xtele_listb* list) {
	if(xtele_listb_is_single(list))
		return NULL;
	else 
		return xtele_list_remove(list);
}	
/*Fin xtele_listb */
#endif

xtele_stack* xtele_stack_new(void) {
	return calloc(1, sizeof(xtele_list));
}

void xtele_stack_destroy(xtele_stack* stack) {
	free(stack);
}

void xtele_stack_push(xtele_stack* stack, void* data) {
	xtele_list_append(stack, data);
}

void* xtele_stack_pop(xtele_stack* stack) {
	xtele_stack* last;
	void* data = NULL;

	last = xtele_list_last(stack);
	if(last) {
		data = last->data;
		xtele_list_remove_element(last, NULL);
	}
	return data;
}

void* xtele_stack_get_data(xtele_stack* stack) {
	xtele_stack* last;

	last = xtele_list_last(stack);
	if(last)
		return last->data;
	else
		return NULL;
}

void xtele_stack_set_data(xtele_stack* stack, void* data) {
	xtele_stack* last;

	last = xtele_list_last(stack);
	if(last)
		last->data = data;
}

