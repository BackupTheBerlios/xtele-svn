#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "core.h"

static void xtele_core_manage_message(xtele_object* message, void* user_data) {
	char* sender_name;
	xtele_object* sender;
	xtele_type type;
	void* data;
	xtele_list* module_list;

	data = xtele_message_data(message);
	sender_name = xtele_message_sender(message);
	type = xtele_message_type(message);

	module_list = xtele_object_prop_get(core_app, "module list");
	sender = xtele_list_find(module_list, FIND_FUNC(xtele_prop_is), sender_name);
	if(!sender) {
		xtele_print(ALERT, "xtele", "Received message '%s' from unknown sender '%s' of type '%i'\n", message->name, sender_name, type);
		return;
	}

	xtele_print(DEBUG_2, "xtele", "Received message '%s' from sender '%s' of type '%i'\n", message->name, sender->name, type);
	if(xtele_message_is(message, "xtele/action/quit")) {
//		xtele_message_send_void(sender, "");
		xtele_core_quit();
	} else if(xtele_message_is(message, "xtele/name/set")) {
		xtele_print(DEBUG, "xtele", "Module '%s' wants to change his name to '%s'.\n",sender->name, data);
		free(sender->name);
		sender->name = strdup(data);
	} else if(xtele_message_is(message,  "xtele/message/receive")) {
		xtele_print(DEBUG, "xtele", "Module '%s' wants to receive message of type '%s'.\n", sender->name, data);
		xtele_core_module_message_add(sender, data);
	} else if(xtele_message_is(message, "xtele/conf/module/get")) {
		xtele_prop* conf;

		xtele_print(DEBUG, "xtele", "Module '%s' wants his configuration parameter '%s'.\n", sender->name, data);
		conf = xtele_core_conf_get(sender->name, data);
		if(conf)
			xtele_message_send_prop(sender, "xtele/conf/module", conf);
	}
}

void xtele_core_module_find(void) {
	DIR* moduledir;
	struct dirent *file;
	char* moduledir_char;

	moduledir_char = xtele_usermoduledir();
	moduledir = opendir(moduledir_char);
	while((file = readdir(moduledir))) {
		if(file->d_name[0] != '.') {
			char* file_name;

			file_name = xt_strcat(moduledir_char, file->d_name);
			xtele_core_module_load(file->d_name, file_name);
			free(file_name);
		}
	}
	closedir(moduledir);
	free(moduledir_char);
}

void xtele_core_run( xtele_object* xtele) {
	xtele_list* module_list;
	xtele_object* core;

	module_list = xtele_object_prop_get(core_app, "module list");
	core = xtele_list_find(module_list, FIND_FUNC(xtele_prop_is), "core");
	xtele_core_module_message_add(core, "xtele/name/set");
	xtele_core_module_message_add(core, "xtele/message/receive");
	xtele_core_module_message_add(core, "xtele/action/quit");
	xtele_core_module_message_add(core, "xtele/conf/module/get");

	xtele_core_module_find();
	xtele_message_listen_register(xtele, xtele_core_manage_message, NULL);
}

