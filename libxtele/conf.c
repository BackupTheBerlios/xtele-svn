/** @file
 * Configuration functions.
 * @ingroup libxtele xtele_conf
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtelelib.h"

static void xtele_conf_get_data(char* buf, int cBytes, int *cBytesActual, void* file) {
	*cBytesActual = fread(buf, 1, cBytes, file);
}

static void xtele_conf_element_start(char* qName, char** atts, void* state) {
	int *level = xtele_object_prop_get(state, "level");
	if(level) {
		(*level)++;
	}
	
	if(streq(qName, "xtele")) {
		xtele_object* level_data;
		xtele_stack* stack;
		if(*level > 1)
			xtele_print(ALERT, "xtele/conf", "Configuration file syntax incorrect\n");
		
		xtele_object_prop_add(state, "conf", XTELE_TYPE_LIST_PROP, xtele_list_new());
		stack = xtele_stack_new();
		level_data = xtele_object_new("level");
		xtele_object_prop_add(level_data, "list", XTELE_TYPE_LIST_PROP, xtele_list_new());
		xtele_object_prop_add_string(level_data, "module", strdup("xml xtele root"));
		xtele_stack_push(stack, level_data);
		xtele_object_prop_add(state, "stack", XTELE_TYPE_LIST_PROP, stack);
	}  else if(streq(qName, "prop") || streq(qName, "module")) {
		char* name;
		xtele_object *parent_level_data, *level_data;
		xtele_stack* stack;
		xtele_list *parent_list;
		void* data;
		
		stack = xtele_object_prop_get(state, "stack");
		parent_level_data = xtele_stack_get_data(stack);
		data = xtele_object_prop_get(parent_level_data, "content");
		
		if(!data) {
			
			level_data = xtele_object_new("level");
			name = xtele_xml_att_get(atts, "name");
			xtele_object_prop_add_string(level_data, qName, strdup(name));
			xtele_object_prop_add(level_data, "list", XTELE_TYPE_LIST_PROP, xtele_list_new());
			xtele_stack_push(stack, level_data);
		} 
	}
}

static void xtele_conf_element_content(char* chars, int cbSize, void* state) {
	char *nchars;
	xtele_stack* stack;
	xtele_object* level_data;
	
	nchars = calloc(cbSize + 1, sizeof(char));
	memcpy(nchars, chars, cbSize);
	
	stack = xtele_object_prop_get(state, "stack");
	level_data = xtele_stack_get_data(stack);
	xtele_object_prop_add(level_data, "content", XTELE_TYPE_LIST_PROP, nchars);
}

static void xtele_conf_element_end(char* qName, void* state) {
	int *level = xtele_object_prop_get(state, "level");
	if(level) {
		(*level)--;
	}
	
	if(streq(qName, "xtele") ) {
		xtele_stack* stack;
		xtele_object *level_data;
		xtele_list* list;
		
		stack = xtele_object_prop_remove(state, "stack");
		level_data = xtele_stack_pop(stack);
		list = xtele_object_prop_remove(level_data, "list");
		xtele_object_prop_add(state, "conf", XTELE_TYPE_LIST_PROP, list);
		
		xtele_object_destroy(level_data);
		xtele_stack_destroy(stack);
		
	} else if(streq(qName, "prop") || streq(qName, "module")) {
		xtele_stack* stack;
		xtele_prop* prop;
		xtele_object *level_data;
		xtele_list* list;
		void* data;
		char* name;
		
		stack = xtele_object_prop_get(state, "stack");
		level_data = xtele_stack_pop(stack);
		name = xtele_object_prop_get(level_data, qName);
		data = xtele_object_prop_remove(level_data, "content");
		list = xtele_object_prop_remove(level_data, "list");
		
		if(data) {
			prop = xtele_prop_new(name, XTELE_TYPE_STRING, data);
		} else if(list) {
			prop = xtele_prop_new(name, XTELE_TYPE_LIST_PROP, list);
		} else {
			prop = xtele_prop_new(name, XTELE_TYPE_NULL, NULL);
		}
		xtele_object_destroy(level_data);
		
		level_data = xtele_stack_get_data(stack);
		list = xtele_object_prop_get(level_data, "list");
		list = xtele_list_insert(list, prop);
		xtele_object_prop_change_data(level_data, "list", list);
	}
}

/** @defgroup xtele_conf Configuration
 * Configuration part of xtele (It will probably move in core_module in a near future).
 * The configuration file is in xml format. You can use ::xtele_userconfdir and ::xtele_userconffile to retrieve the emplacements of the configuration directory and file (On Unix platorm, it is usually @e $HOME/.xtele/ and @e $HOME/.xtele/xtele.conf) .
 * @par Example configuration file : 
 * @verbinclude conf/xtele.conf.sample
 * @see xtele_xml
 * @ingroup libxtele
 */
/** Parse the configuration file
 * @return list of the configuration parameter as a list of xtele_type::XTELE_TYPE_PROP.
 * @ingroup xtele_conf
 */
xtele_list* xtele_conf_parse(void) {
	xtele_list* conf;
	xtele_object* state;
	char* conffile = xtele_userconffile();
	FILE* file = fopen(conffile, "r");
	
	if(!file) {
		xtele_print(ALERT, "xtele conf", "Cannot open the configuration file.\n");
		free(conffile);
		return NULL;
	}
	state = xtele_object_new("state");
	xtele_object_prop_add_int(state, "level", 0);
	
	xtele_xml_parse(xtele_conf_get_data, xtele_conf_element_start, xtele_conf_element_content, xtele_conf_element_end, file, state);
	
	conf = xtele_object_prop_remove(state, "conf");
	xtele_object_destroy(state);
	fclose(file);
	free(conffile);
	return conf;
}
