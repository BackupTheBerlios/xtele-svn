/** @file
 * Functions to manipulate linked lists and stacks
 * @ingroup libxtele xtele_objects xtele_list xtele_listb xtele_stack
 */
#include <stdlib.h>
#include <string.h>
#include "xtelelib.h"

/** @defgroup  xtele_list xtele linked list
 * Linked list in xtele.
 * @ingroup xtele_objects
 * @see  xtele_listb xtele_stack list.c
 * @{ 
 */
/** Creates a new ::xtele_list linked list. */
xtele_list* xtele_list_new(void) {
	return NULL;
}

/** Creates a new ::xtele_list linked list element.
 * It is mainly used internally.
 */
xtele_list* xtele_list_element_new(void* data) {
	xtele_list* list;

	list = calloc(1, sizeof(xtele_list));
	list->data = data;

	return list;
}

/** Destroys one ::xtele_list element.
 * @param list The ::xtele_list element to destroy
 * @param free_func The function called to destroy the data of the element.
 */
void xtele_list_destroy_one(xtele_list* list, void (*free_func) (void*) ) {
	if(list) {
		if(free_func)
			free_func(list->data);
		free(list);
	}
}

/** Destroys the ::xtele_list list.
 * @param list The ::xtele_list to destroy
 * @param free_func The function called to destroy the data of the list.
 */
void xtele_list_destroy(xtele_list* list, void (*free_func) (void*) ) {
	if(list) {
		xtele_list_destroy(list->next, free_func);
		xtele_list_destroy_one(list, free_func);
	}
}

/** Return the last element of a ::xtele_list linked list.
 * @param list The list from which you wand the last element.
 * @return The last ::xtele_list of list.
 */
xtele_list* xtele_list_last(xtele_list* list) {
	if(list)
		while(list->next)
			list = list->next;

	return list;
}

/** Return the first element of a ::xtele_list linked list.
 * @param list The list from which you wand the first element.
 * @return The first ::xtele_list of list.
 */
xtele_list* xtele_list_first(xtele_list* list) {
	if(list)
		while(list->prev)
			list = list->prev;

	return list;
}
/** Insert a ::xtele_list element in an existing ::xtele_list linked list.
 * The data will be added before list.
 * @param list The list in which you want to insert a new element.
 * @param data The data you want to insert in list.
 * @return The new ::xtele_list.
 */
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

/** Add a ::xtele_list element in an existing ::xtele_list linked list.
 * The data will be added after list.
 * @param list The list in which you want to add a new element.
 * @param data The data you want to add in list.
 * @return The new ::xtele_list.
 */
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

/** Append a ::xtele_list element in an existing ::xtele_list linked list.
 * The data will be appended at the end of the list.
 * @param list The list to which you want to append a new element.
 * @param data The data you want to append to list.
 * @return The new ::xtele_list.
 */
xtele_list* xtele_list_append(xtele_list* list, void* data) {
	return xtele_list_add(xtele_list_last(list), data);
}

/** Removes an element from a ::xtele_list.
 * The element is searched by using @p del_func.
 * The element are freed, including the data by using @p free_func. You can pass NULL if you don't want to free the data.
 * @param list The element you want to remove from the list.
 * @param del_func The function used to find the element to remove
 * @param data data passed to del_func.
 * @param free_func The function used to free the data of list.
 * @return The new beginning of the list.
 */
xtele_list* xtele_list_remove(xtele_list* list, int (*del_func) (void*, void*), void* data, void (*free_func)  (void*)) {
	xtele_list* del_element; 
        del_element = xtele_list_foreach(list, del_func, data);
	if(del_element) {
		if(del_element->next) {
			del_element->next->prev = del_element->prev;
		}
		if(del_element->prev) {
			del_element->prev->next = del_element->next;
		} else {
			list = del_element->next;
		}
		if(free_func)
			free_func(del_element->data);
		free(del_element);
	}
	return list;
}

/** Executes a function for each element in the ::xtele_list.
 * @p foreach_func is executed for each element in debut until it return a value different than '0'.
 * @param debut List iterated.
 * @param foreach_func The function executed.
 * @param data The data transmitted as a 2nd argument of foreach_func.
 * @return The ::xtele_list element at which foreach_func return something different than '0'.
 */
xtele_list* xtele_list_foreach(xtele_list* debut, int (*foreach_func) (void*, void*), void* data) {
	while(debut) {
		if(foreach_func(debut->data, data))
			break;
		xtele_list_next(debut);
	} 
	return debut;
}

/** Search for data in a ::xtele_list.
 * It uses a comparison function.
 * @param debut List explored.
 * @param foreach_func The comparison function
 * @param data The data transmitted as a 2nd argument of foreach_func.
 * @return The data found, or NULL if nothing found.
 */
void* xtele_list_find(xtele_list* debut, int (*foreach_func) (void*, void*), void* data) {
	xtele_list* member;

	member = xtele_list_foreach(debut, foreach_func, data);
	if(member)
		return member->data;
	else
		return NULL;
}	

/** Search for a ::xtele_list element containing the given text. */
xtele_list* xtele_list_find_string(xtele_list* list, char* name) {
	return xtele_list_foreach(list, FOREACH_FUNC(streq), name);
}
/* @} */

/* Debut xtele_listb */
/** @defgroup  xtele_listb xtele looping linked list
 * Looping linked list in xtele.
 * @ingroup xtele_list
 * @see xtele_list xtele_listb list.c
 * @{ 
 */
/** Creates a new ::xtele_listb looping linked list. */
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

/** Insert a ::xtele_listb element in an existing ::xtele_listb looping linked list.
 * @param list The list in which you want to insert a new element.
 * @param data The data you want to insert in list.
 * @return The new ::xtele_listb which may have changed.
 */
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
/* @} */
 
/** @defgroup xtele_stack xtele stacks
 * Stacks in xtele.
 * @ingroup xtele_list
 * @{
 */
/** Creates a new ::xtele_stack */
xtele_stack* xtele_stack_new(void) {
	return calloc(1, sizeof(xtele_list));
}

/** Destroys stack */
void xtele_stack_destroy(xtele_stack* stack) {
	free(stack);
}

/** Adds data on the top of the stack */
void xtele_stack_push(xtele_stack* stack, void* data) {
	xtele_list_append(stack, data);
}

/** Remove data on the top of the stack */
void* xtele_stack_pop(xtele_stack* stack) {
	xtele_stack* last;
	void* data = NULL;

	last = xtele_list_last(stack);
	if(last) {
		data = last->data;
		last->prev->next = NULL;
		free(last);
	}
	return data;
}

/** Returns data ontop the stack without removing them */
void* xtele_stack_get_data(xtele_stack* stack) {
	xtele_stack* last;

	last = xtele_list_last(stack);
	if(last)
		return last->data;
	else
		return NULL;
}

/** Sets data ontop the stack without adding them */
void xtele_stack_set_data(xtele_stack* stack, void* data) {
	xtele_stack* last;

	last = xtele_list_last(stack);
	if(last)
		last->data = data;
}
/* @} */
