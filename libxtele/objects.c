#include <string.h>
#include <stdlib.h>
#include "xtelelib.h"

/* Debut xtele_prop */
xtele_prop* xtele_prop_new(char* name, xtele_type type, void* data) {
	xtele_prop* prop = NULL;

	if(name) {
		prop = calloc(1, sizeof(xtele_prop) );
		prop->name = strdup(name);
		prop->type = type;
		prop->data = data;
	}

	return prop;
}

static void xtele_prop_destroy_data(xtele_type type, void* data) {
	if(data) {
		if(type == XTELE_TYPE_STRING || type == XTELE_TYPE_INT)
			free(data);
		else if(type == XTELE_TYPE_LIST_UNKNOWN)
			xtele_list_destroy(data, NULL);
		else if(type == XTELE_TYPE_LIST_STRING || type == XTELE_TYPE_LIST_INT)
			xtele_list_destroy(data, free);
		else if(type == XTELE_TYPE_OBJECT) {
			xtele_list_destroy(data, FREE_FUNC(xtele_prop_destroy));
		}
	}
}

void xtele_prop_destroy(xtele_prop* prop) {
	if(prop) {
		xtele_prop_destroy_data(prop->type, prop->data);
		free(prop->name);
		free(prop);
	}
}

	int xtele_prop_is(xtele_prop* prop, char* name) {
	if(prop)
		return streq(prop->name, name);
	return 0;
}

int xtele_prop_is_type(xtele_prop* prop, int type) {
	if(prop)
		return prop->type == type;
	return 0;
}
/* Fin xtele_prop */

/* Debut xtele_object */
xtele_object* xtele_object_new(char* name) {
	xtele_object* object;

	object = xtele_prop_new(name, XTELE_TYPE_OBJECT, xtele_list_new());
	return object;
}

void xtele_object_prop_add(xtele_object* object, char* name, xtele_type type, void* data) {
	xtele_prop* type_prop;
	
	if(object && name) {
		type_prop = xtele_prop_new(name, type, data);
		object->data = xtele_list_add(object->data, type_prop);
	}
}

void xtele_object_prop_add_int(xtele_object* object, char* name, int integer) {
	int* pint;

	pint = malloc(sizeof(int));
	*pint = integer;

	xtele_object_prop_add(object, name, XTELE_TYPE_INT, pint);
}

void xtele_object_prop_change_data(xtele_object* object, char* name, void* data) {
	xtele_prop* type_prop;

	type_prop = xtele_object_prop_get_prop(object, name);
	if(type_prop) 
		type_prop->data = data;
}

void xtele_object_prop_delete(xtele_object* object, char* name) {
	if(object) {
		object->data = xtele_list_remove(object->data, FOREACH_FUNC(xtele_prop_is), name, xtele_prop_destroy);
	}
}

xtele_prop* xtele_object_prop_get_prop(xtele_object* object, char* name) {
	xtele_prop* prop = NULL;

	if(object) {
		prop = xtele_list_find(object->data, FOREACH_FUNC(xtele_prop_is), name);
	}
	return prop;
}

void* xtele_object_prop_get(xtele_object* object, char* name) {
	xtele_prop* prop;
	void* data = NULL;
	
	prop = xtele_object_prop_get_prop(object, name);
	if(prop) {
		data = prop->data;
	}
	return data;
}

xtele_type xtele_object_prop_get_type(xtele_object* object, char* name) {
	xtele_prop* prop;
	xtele_type type = XTELE_TYPE_UNKNOWN;
	
	prop = xtele_object_prop_get_prop(object, name);
	if(prop) {
		type = prop->type;
	}
	return type;
}

xtele_object* xtele_module_new(char* name, char* sender_name, int file) {
	xtele_object* object;
	int *pfile;

	pfile = calloc(1, sizeof(int));
	*pfile = file;

	object = xtele_object_new(name);
	xtele_object_prop_add(object, "file", XTELE_TYPE_INT, pfile);
	xtele_object_prop_add(object, "sender/name", XTELE_TYPE_STRING, strdup(sender_name));
	xtele_object_prop_add(object, "message", XTELE_TYPE_LIST_UNKNOWN, xtele_stack_new());
	xtele_object_prop_add(object, "message_handler", XTELE_TYPE_UNKNOWN, NULL);
	xtele_object_prop_add(object, "message_handler_data", XTELE_TYPE_UNKNOWN, NULL);

	return object;
}

xtele_object* xtele_message_new(char* message_name, char* sender_name, xtele_type type, void* data) {
	xtele_object* message = NULL;

	if(message_name && sender_name) {
		message = xtele_object_new(message_name);
		xtele_object_prop_add(message, "sender/name", XTELE_TYPE_STRING, strdup(sender_name));
		xtele_object_prop_add(message, "data", type, data);
	}
	return message;
}
/* Fin xtele_object */
