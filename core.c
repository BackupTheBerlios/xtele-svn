#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include "core-module.h"
#include "core.h"

void xtele_core_broadcast(xtele_object* message, void* data) {
	xtele_list *module_list;

	module_list = xtele_object_prop_get(core_app, "module list");
	while(module_list) {
		if(xtele_core_module_message_match(module_list->data, message->name)) {
			xtele_message_send(module_list->data, message);
		}
		xtele_list_next(module_list);
	}
}

xtele_object* xtele_core_module_listen(char* name, int file) {
	xtele_object* module;
	xtele_list* module_list;

	module = xtele_module_new(name, "xtele", file);
	xtele_object_prop_add(module, "event_list", XTELE_TYPE_LIST_STRING, xtele_list_new());

	module_list = xtele_object_prop_get(core_app, "module list");
	module_list = xtele_list_add(module_list, module);
	xtele_object_prop_change_data(core_app, "module list", module_list);
	
	xtele_message_listen_register(module, xtele_core_broadcast, NULL);

	return module;
}

void xtele_core_module_message_add(xtele_object* module, char* event_type) {
	xtele_list* event_list;

	event_list = xtele_object_prop_get(module, "event_list");
	event_list = xtele_list_add(event_list, strdup(event_type));
	xtele_object_prop_change_data(module, "event_list", event_list);
}

int xtele_core_module_message_match(xtele_object* module, char* event_type) {
	xtele_list* event_list;

	event_list = xtele_object_prop_get(module, "event_list");
	if(xtele_list_find_string(event_list, event_type))
		return 1;
	return 0;
}

xtele_prop* xtele_core_conf_get(char* module, char* prop_name) {
	xtele_list* mod_list;
	xtele_object* mod;

	mod_list = xtele_object_prop_get(core_app, "configuration");
	mod = xtele_list_find(mod_list, FIND_FUNC(xtele_prop_is), module);
	return xtele_object_prop_get_prop(mod, prop_name);
}

void xtele_core_quit(void) {
	xtele_message_listen_stop();
}

void xtele_core_uninit(void) {
	xtele_list* module_list;


	module_list = xtele_object_prop_get(core_app, "module list");
	while(module_list) {
		void* handle;

		handle = xtele_object_prop_get(module_list->data, "dl handle");
		if(handle)
			dlclose(handle);
		xtele_list_next(module_list);
	}		
	xtele_object_destroy(core_app);
}

void xtele_core_init(void) {
	core_app = xtele_object_new("xtele");

	xtele_object_prop_add(core_app, "module list", XTELE_TYPE_LIST_PROP, xtele_list_new());
	xtele_object_prop_add(core_app, "module list local", XTELE_TYPE_LIST_PROP, xtele_list_new());
	xtele_object_prop_add(core_app, "configuration", XTELE_TYPE_LIST_PROP, xtele_conf_parse());
	xtele_core_module_start("core", xtele_core_run);
	xtele_message_listen();

	xtele_message_uninit();
	xtele_core_uninit();
}

xtele_object* xtele_core_module_start(char* name, void (*ext_init) (xtele_object* )) {
	int files[2];
	xtele_object *xtele, *module;
	xtele_list* module_list_local;

	socketpair(AF_LOCAL, SOCK_STREAM, 0, files);
	xtele_set_nonblock(files[0]);
	xtele_set_nonblock(files[1]);

	module = xtele_core_module_listen(name, files[0]);
	xtele = xtele_module_new("xtele2", name, files[1]);

	module_list_local = xtele_object_prop_get(core_app, "module list local");
	module_list_local = xtele_list_add(module_list_local, xtele);
	xtele_object_prop_change_data(core_app, "module list local", module_list_local);

	ext_init(xtele);
	xtele_print(DEBUG, "xtele", "Module '%s' loaded.\n", name);
	return module;
}

void xtele_core_module_load(char* name, char* filename) {
	void (*ext_init) ( xtele_object*);
	void* handle;
	xtele_object* module;

	handle = dlopen(filename, RTLD_NOW);
	ext_init = dlsym(handle, "xtele_ext_init");
	module = xtele_core_module_start(name, ext_init);
	xtele_object_prop_add(module, "dl handle", XTELE_TYPE_UNKNOWN, handle);
}
