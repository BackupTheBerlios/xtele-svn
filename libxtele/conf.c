#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtelelib.h"

static void xtele_conf_get_data(char* buf, int cBytes, int *cBytesActual, void* file) {
	*cBytesActual = fread(buf, 1, cBytes, file);
}

static void xtele_conf_element_start(char* qName, char** atts, void* stack) {
	if(streq(qName, "xtele")) {
		xtele_stack_push(stack, NULL);
	} else if(streq(qName, "list")) {
		xtele_stack_push(stack, NULL);
	} else if(streq(qName, "module") || streq(qName, "prop")) {
		char* name;

		name = xtele_xml_att_get(atts, "name");
		xtele_stack_push(stack, strdup(name));
		xtele_stack_push(stack, NULL);
	}
}

static void xtele_conf_element_content(char* chars, int cbSize, void* stack) {
	char *nchars;
	
	nchars = calloc(cbSize + 1, sizeof(char));
	memcpy(nchars, chars, cbSize);

	xtele_stack_push(stack, nchars);
}

static void xtele_conf_element_end(char* qName, void* stack) {
	void *prop_data;
	if(streq(qName, "xtele") ) {
		xtele_list* conf;
		void* data;
		
		conf = xtele_list_new();
		while((data = xtele_stack_pop(stack)))
			conf = xtele_list_insert(conf, data);
		xtele_stack_push(stack, conf);	
	} else {
		char *name;
		xtele_object *prop;
		xtele_type type = XTELE_TYPE_STRING; 

		prop_data = xtele_stack_pop(stack);
		if(prop_data) {
			if(xtele_stack_get_data(stack) || streq(qName, "module")) {
				xtele_list* prop_list;
				void* data;

				type = XTELE_TYPE_LIST_PROP;
				prop_list = xtele_list_new();
				prop_list = xtele_list_insert(prop_list, prop_data);
				while((data = xtele_stack_pop(stack))) 
					prop_list = xtele_list_insert(prop_list, data);
				prop_data = prop_list;
			} else 
				xtele_stack_pop(stack);
		}
		name = xtele_stack_pop(stack);
		prop = xtele_prop_new(name, type, prop_data);
		xtele_stack_push(stack, prop);
		free(name);
	}
}

xtele_list* xtele_conf_parse(void) {
	xtele_list* conf;
	xtele_stack* stack;
	char* conffile = xtele_userconffile();
	FILE* file;

	stack = xtele_stack_new();
	file = fopen(conffile, "r");
	
	xtele_xml_parse(xtele_conf_get_data, xtele_conf_element_start, xtele_conf_element_content, xtele_conf_element_end, file, stack);
	conf = xtele_stack_pop(stack);
	xtele_stack_destroy(stack);
	fclose(file);
	free(conffile);

	return conf;
}
