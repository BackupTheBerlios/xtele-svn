/** @file
 * Function to use basic objects and properties.
 * @ingroup libxtele xtele_objects xtele_object xtele_prop
 */
/** @defgroup  xtele_objects xtele objects 
 * Various objects used in xtele.
 * @ingroup libxtele 
 */
#include <string.h>
#include <stdlib.h>
#include "xtelelib.h"

/* Debut xtele_prop */
/** @defgroup  xtele_prop xtele property
 * A basic class defining a 'property'
 * @ingroup xtele_objects
 * @{ 
 */
/** Creates a new ::xtele_prop.
 * The ::xtele_prop created must be freed by ::xtele_prop_destroy after use.
 * @param name Name of the property.
 * @param type Type of the property.
 * @param data data of the property of type 'type'.
 * @return A new ::xtele_prop.
*/
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
		else if(type == XTELE_TYPE_OBJECT) 
			xtele_list_destroy(data, FREE_FUNC(xtele_prop_destroy));
		else if(type == XTELE_TYPE_PROP)
			xtele_prop_destroy(data);
	}
}

/** Destroys a ::xtele_prop.
 * The data are also destroyed according to the type of the property.
 * @param prop The ::xtele_prop to be destroyed.
 */
void xtele_prop_destroy(xtele_prop* prop) {
	if(prop) {
		xtele_prop_destroy_data(prop->type, prop->data);
		free(prop->name);
		free(prop);
	}
}

/** Tells if a ::xtele_prop has the given name.
 * @param name The name we want to compare the property name with.
 * @param prop The property we want to check.
 * @return 0 if the property doesn't have the good name.
 */
int xtele_prop_is(xtele_prop* prop, char* name) {
	if(prop)
		return streq(prop->name, name);
	return 0;
}

/** Tells if a ::xtele_prop has the given ::xtele_type.
 * @param type The type we want to compare the property type with.
 * @param prop The property we want to check.
 * @return 0 if the property doesn't have the good type.
 */
int xtele_prop_is_type(xtele_prop* prop, int type) {
	if(prop)
		return prop->type == type;
	return 0;
}

static void xtele_prop_print_data(xtele_type type, void* data) {
	if(data) {
		if(type == XTELE_TYPE_INT)
			printf("'string' : '%i'\n", *( (int*)data));
		if(type == XTELE_TYPE_NULL)
			printf("'NULL'");
		if(type == XTELE_TYPE_STRING)
			printf("'string' : '%s'\n", data);
		if(type == XTELE_TYPE_PROP)
			xtele_prop_print(data);
		if(type == XTELE_TYPE_LIST_PROP) {
			printf("'list'\n");
			xtele_list_foreach(data, FOREACH_FUNC(xtele_prop_print), NULL);
			printf("'list'\n");
		}
	}
}

/** Print a property 
 * @param prop The property to be printed.
 * @return Always 0.
 * @bug It currently just prints the name of the property. It will also probably print the data of the property according to its ::xtele_type in a near future.
 */
int xtele_prop_print(xtele_prop* prop) {
	if(prop) {
		printf("Property : '%s'\n", prop->name);
		xtele_prop_print_data(prop->type, prop->data);
	}
	return 0; 
}
/** @} Fin xtele_prop */

/* Debut xtele_object */
/** @defgroup  xtele_object xtele object
 * A basic class defining an 'object'
 * @ingroup xtele_objects
 * @see xtele_list xtele_prop
 * @{ 
 */

/** Creates a new ::xtele_object.
 * The ::xtele_object created must be freed by ::xtele_object_destroy after use.
 * @param name Name of the object.
 * @return A new ::xtele_object.
 */
xtele_object* xtele_object_new(char* name) {
	xtele_object* object;

	object = xtele_prop_new(name, XTELE_TYPE_OBJECT, xtele_list_new());
	return object;
}

/** Adds a new property to an object.
 * @param object The object to add the property to.
 * @param name Name of the property.
 * @param type Type of the property.
 * @param data Data of the property.
 */
void xtele_object_prop_add(xtele_object* object, char* name, xtele_type type, void* data) {
	xtele_prop* type_prop;
	
	if(object && name) {
		type_prop = xtele_prop_new(name, type, data);
		object->data = xtele_list_add(object->data, type_prop);
	}
}

/** Adds a new property of type xtele_type::XTELE_TYPE_INT to an object.
 * @param object The object to add the property to.
 * @param name Name of the property.
 * @param integer The int to add.
 */
void xtele_object_prop_add_int(xtele_object* object, char* name, int integer) {
	int* pint;

	pint = malloc(sizeof(int));
	*pint = integer;

	xtele_object_prop_add(object, name, XTELE_TYPE_INT, pint);
}

/** Change an existing property of an object.
 * @param object The object to add the property to.
 * @param name Name of the property.
 * @param data New data of the property.
 */
void xtele_object_prop_change_data(xtele_object* object, char* name, void* data) {
	xtele_prop* type_prop;

	type_prop = xtele_object_prop_get_prop(object, name);
	if(type_prop) 
		type_prop->data = data;
}

/** Deletes a property from an object.
 * The property is freed using its ::xtele_type.
 * @param object The object from which the property is deleted.
 * @param name The name of the property to delete.
 */
void xtele_object_prop_delete(xtele_object* object, char* name) {
	if(object) {
		object->data = xtele_list_remove(object->data, FOREACH_FUNC(xtele_prop_is), name, FREE_FUNC(xtele_prop_destroy));
	}
}

/** Removes a property from an object.
 * Contrary to ::xtele_object_prop_delete, the property data is not freed and is returned.
 * @param object The object from which the property is deleted.
 * @param name The name of the property to delete.
 * @return The data of the removed property.
 */
void* xtele_object_prop_remove(xtele_object* object, char* name) {
	if(object) {
		xtele_prop* prop = xtele_object_prop_get_prop(object, name);
		if(prop) {
			void* data = prop->data;
			prop->data = NULL;
			xtele_object_prop_delete(object, name);
			return data;
		}
	}
	return NULL;
}

/** ::xtele_prop of an object.
 * @param object The object which contains the property.
 * @param name The property we want.
 * @return The correct :xtele_prop or NULL if the object doesn't contain this property.
 */
xtele_prop* xtele_object_prop_get_prop(xtele_object* object, char* name) {
	xtele_prop* prop = NULL;

	if(object) {
		prop = xtele_list_find(object->data, FOREACH_FUNC(xtele_prop_is), name);
	}
	return prop;
}

/** Property of an object.
 * @param object The object which contains the property.
 * @param name The property we want.
 * @return The correct property data or NULL if the object doesn't contain this property.
 */
void* xtele_object_prop_get(xtele_object* object, char* name) {
	xtele_prop* prop;
	void* data = NULL;
	
	prop = xtele_object_prop_get_prop(object, name);
	if(prop) {
		data = prop->data;
	}
	return data;
}

/** Property type of an object.
 * @param object The object which contains the property.
 * @param name The property from which we want to know the type.
 * @return The correct property type or xtele_type::XTELE_TYPE_UNKNOWN if the object doesn't contain this property.
 */
xtele_type xtele_object_prop_get_type(xtele_object* object, char* name) {
	xtele_prop* prop;
	xtele_type type = XTELE_TYPE_UNKNOWN;
	
	prop = xtele_object_prop_get_prop(object, name);
	if(prop) {
		type = prop->type;
	}
	return type;
}

/** Print an object 
 * @param object The object to be printed.
 * @return Always 0.
 */
int xtele_object_print(xtele_object* object) {
	if(object) {
		printf("Object : '%s'\n", object->name);
		xtele_list_foreach(object->data, FOREACH_FUNC(xtele_prop_print), NULL);
	}
	return 0;
}
/** @} Fin xtele_object */
