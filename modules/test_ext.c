#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include "xtelelib.h"

void test_ext_manage_event(xtele_object* message, void* data) {
	xtele_print(DEBUG_2, "test_ext", "Received message '%s' from sender '%s' of type : '%i'\n", message->name, xtele_message_sender(message), xtele_message_type(message));
	if(streq(message->name, "xtele/conf/module")) {
//		printf("%s\n", xtele_message_data(message));
	}
}

void xtele_ext_init(xtele_object* xtele) {
//	xtele_prop *prop;

/*	
	prop = xtele_prop_new("prop", "prop_data", strlen("prop_data") + 1);
	xtele_event_send_prop(xtele, "xtele/prop/add", prop);
	xtele_prop_destroy(prop);*/

	/*
	prop = xtele_prop_new("test_ext2", "prop", strlen("prop") + 1);
	xtele_event_send_prop(xtele, "xtele/prop/get", prop);
	xtele_prop_destroy(prop);
	*/

//	xtele_event_send_void(xtele, "xtele/action/quit");
	xtele_message_send_string(xtele, "xtele/conf/module/get", "bof");
	xtele_message_listen_register(xtele, test_ext_manage_event, xtele);
}
