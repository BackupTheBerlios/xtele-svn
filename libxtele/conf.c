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
		if(*level > 1)
			xtele_print(ALERT, "xtele/conf", "Configuration file syntax incorrect\n");
		
		xtele_object_prop_add(state, "conf", XTELE_TYPE_LIST_PROP, xtele_list_new());
	} else if(streq(qName, "module")) {
		char* name;
		xtele_object* module;

		if(*level > 2 || *level < 1)
			xtele_print(ALERT, "xtele/conf", "Configuration file syntax incorrect\n");
		
		name = xtele_xml_att_get(atts, "name");
		module = xtele_object_new(name);
		xtele_object_prop_add_prop(state, "module", module);
	} else if(streq(qName, "prop")) {
		char* name;
		xtele_object* prop;
		
		name = xtele_xml_att_get(atts, "name");
	//	prop = xtele_object_new(name);
	}
}

static void xtele_conf_element_content(char* chars, int cbSize, void* state) {
	char *nchars;
	
	nchars = calloc(cbSize + 1, sizeof(char));
	memcpy(nchars, chars, cbSize);

	xtele_object_prop_add(state, "content", XTELE_TYPE_STRING, nchars);
}

static void xtele_conf_element_end(char* qName, void* state) {
	int *level = xtele_object_prop_get(state, "level");
	if(level) {
		(*level)--;
	}
	
	if(streq(qName, "xtele") ) {
		
	} else if(streq(qName, "prop")) {
		xtele_object_prop_delete(state, "content");
	} else if(streq(qName, "module")) {
		xtele_object* module = xtele_object_prop_remove(state, "module");
		xtele_list* conf = xtele_object_prop_get(state, "conf");
		conf = xtele_list_insert(conf, module);
		xtele_object_prop_change_data(state, "conf", conf);
	}
}

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
