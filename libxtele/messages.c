/** @file
 * Functions to convey messages inside xtele and its modules.
 * @see xtele message
 * @ingroup libxtele xtele_message
 */
/** @defgroup xtele_message xtele message
 * Message conveying inside xtele and its modules.
 * @see xtele_objects xtele_xml
 * @ingroup libxtele
 * @{
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include "xtelelib.h"

static xtele_list* listen_list = NULL;
static int running;

static void xtele_xml_send(char* s, void* file) {
	write(*((int*) file), s, strlen(s));
}

static void xtele_message_data_send(xtele_object* writer, xtele_type type, void* data) {
	char buffer[XTELE_INT_MAX_LEN];
	xtele_list* list;

	switch(type) {
		default:
			break;
		case XTELE_TYPE_INT:
			snprintf(buffer, XTELE_INT_MAX_LEN, "%i", *( (int*) data));
			xtele_xml_writer_add_text(writer, buffer);
			break;
		case XTELE_TYPE_STRING:
			xtele_xml_writer_add_text(writer, data);
			break;
		case XTELE_TYPE_PROP:
			snprintf(buffer, XTELE_INT_MAX_LEN, "%i", XTELE_PROP(data)->type);
			xtele_xml_writer_add_element(writer, "prop");
			xtele_xml_writer_add_attribute(writer, "name", XTELE_PROP(data)->name);
			xtele_xml_writer_add_attribute(writer, "type", buffer);
			xtele_message_data_send(writer, XTELE_PROP(data)->type, XTELE_PROP(data)->data);
			xtele_xml_writer_end_element(writer);
			break;
		case XTELE_TYPE_LIST_INT:
		case XTELE_TYPE_LIST_STRING:
		case XTELE_TYPE_LIST_PROP:
			snprintf(buffer, XTELE_INT_MAX_LEN, "%i", type);
			list = xtele_list_first(data);			
			xtele_xml_writer_add_element(writer, "list");
			xtele_xml_writer_add_attribute(writer, "type", buffer);
			while(list) {
				xtele_xml_writer_add_element(writer, "el");
				xtele_message_data_send(writer, type - XTELE_TYPE_LIST_UNKNOWN, list->data);
				xtele_xml_writer_end_element(writer);
				xtele_list_next(list);
			}
			xtele_xml_writer_end_element(writer);
			break;
	}
}	

#if 0
void xtele_message_close(xtele_object* dest) {

}
#endif

static xtele_object* xtele_message_new(char* message_name, char* sender_name, xtele_type type, void* data) {
	xtele_object* message = NULL;

	if(message_name && sender_name) {
		message = xtele_object_new(message_name);
		xtele_object_prop_add(message, "sender/name", XTELE_TYPE_STRING, strdup(sender_name));
		xtele_object_prop_add(message, "data", type, data);
	}
	return message;
}

static void xtele_message_destroy(xtele_object* message) {
	xtele_object_prop_remove(message, "data");
	xtele_object_destroy(message);
}

/** Sends a message.
 * @see  xtele_message_send_long xtele_message_send_void xtele_message_send_string xtele_message_send_int xtele_message_send_prop
 * @param dest Recipient of the message.
 * @param message Message to be sent.
 */
void xtele_message_send(xtele_object* dest, xtele_object* message) {
	xtele_object* writer;
	char type_buffer[XTELE_INT_MAX_LEN], *sender_name;
	int type, *file;
	void* data;

	file = xtele_object_prop_get(dest, "file");
	writer = xtele_xml_writer_new(xtele_xml_send, file);
	data = xtele_object_prop_get(message, "data");
	type = xtele_object_prop_get_type(message, "data");
	sender_name = xtele_object_prop_get(message, "sender/name");
	snprintf(type_buffer, XTELE_INT_MAX_LEN, "%i", type);

	xtele_xml_writer_add_element(writer, "message");
	xtele_xml_writer_add_attribute(writer, "name", message->name);
	xtele_xml_writer_add_attribute(writer, "type", type_buffer);
	xtele_xml_writer_add_attribute(writer, "sender", sender_name);
	xtele_message_data_send(writer, type, data);
	xtele_xml_writer_end_element(writer);
}

/** Sends a message.
 * @see  xtele_message_send xtele_message_send_void xtele_message_send_string xtele_message_send_int xtele_message_send_prop
 * @param dest Recipient of the message.
 * @param sender_name Name of the sender.
 * @param message_name Name (or title) of the message.
 * @param type ::xtele_type of the message.
 * @param data Data to be sent.
 */
void xtele_message_send_long(xtele_object* dest, char* sender_name, char* message_name, xtele_type type, void* data) {
	xtele_object* message;

	if(message_name) {
		if(!sender_name)
			sender_name = xtele_object_prop_get(dest, "sender/name");

		message = xtele_message_new(message_name, sender_name, type, data);
		xtele_message_send(dest, message);
		xtele_message_destroy(message);
	}
}	

/** Sends a void message.
 * @see xtele_message_send xtele_message_send_long xtele_message_send_string xtele_message_send_int xtele_message_send_prop
 * @param dest Recipient of the message.
 * @param message Name (or title) of the message.
 */
void xtele_message_send_void(xtele_object* dest, char* message) {
	xtele_message_send_long(dest, NULL, message, XTELE_TYPE_NULL, NULL);
}

/** Sends a message of type xtele_type::XTELE_TYPE_STRING.
 * @see xtele_message_send xtele_message_send_long xtele_message_send_void xtele_message_send_int xtele_message_send_prop
 * @param dest Recipient of the message.
 * @param message Name (or title) of the message.
 * @param string String to be sent.
 */
void xtele_message_send_string(xtele_object* dest, char* message, char* string) {
	xtele_message_send_long(dest, NULL, message, XTELE_TYPE_STRING, string);
}

/** Sends a message of type xtele_type::XTELE_TYPE_INT.
 * @see xtele_message_send xtele_message_send_long xtele_message_send_void xtele_message_send_string  xtele_message_send_prop
 * @param dest Recipient of the message.
 * @param message Name (or title) of the message.
 * @param data Integer to be sent.
 */
void xtele_message_send_int(xtele_object* dest, char* message, int data) {
	int* pint;

	pint = malloc(sizeof(int));
	*pint = data;
	xtele_message_send_long(dest, NULL, message, XTELE_TYPE_INT, pint);
}

/** Sends a message of type xtele_type::XTELE_TYPE_PROP.
 * @see xtele_message_send xtele_message_send_long xtele_message_send_void xtele_message_send_string xtele_message_send_int
 * @param dest Recipient of the message.
 * @param message Name (or title) of the message.
 * @param prop ::xtele_prop to be sent.
 */
void xtele_message_send_prop(xtele_object* dest, char* message, xtele_prop* prop) {
	xtele_message_send_long(dest, NULL, message, XTELE_TYPE_PROP, prop);
}

static void xtele_xml_get_data(char* buf, int cBytes, int *cBytesActual, void *file) {
	int filedes = *((int*) file);

	*cBytesActual = read(filedes, buf, cBytes);
	return;
	if(*cBytesActual > 0) {
		write(STDOUT_FILENO, buf, *cBytesActual);
	}
}

static void xtele_xml_element_start(char* qName, char** atts, void* sender) {
	char *name, *type, *sender_name;
	xtele_stack *message_stack;

	message_stack = xtele_object_prop_get(sender, "message");

	name = xtele_xml_att_get(atts, "name");
	type = xtele_xml_att_get(atts, "type");
	sender_name = xtele_xml_att_get(atts, "sender");

	if(streq(qName, "message")) {
		xtele_object* message;

		message = xtele_message_new(name, sender_name, atoi(type), NULL);
		xtele_stack_push(message_stack, message);
		xtele_stack_push(message_stack, strdup(type));
		xtele_stack_push(message_stack, NULL);
	}
	else if(streq(qName, "prop")) {
		xtele_prop* prop;

		prop = xtele_prop_new(name, atoi(type), NULL);
		xtele_stack_set_data(message_stack, prop);
		xtele_stack_push(message_stack, strdup(type));
		xtele_stack_push(message_stack, NULL);
	}
	else if(streq(qName, "list")) {
		xtele_stack_set_data(message_stack, xtele_list_new());
		xtele_stack_push(message_stack, strdup(type));
	}
	else if(streq(qName, "el")) {
		xtele_stack_push(message_stack, NULL);
	}
}

static void xtele_xml_content(char* chars, int cbSize, void* sender) {
	char *nchars, *ptype;
	int* data_int;
	xtele_stack* message_stack;
	xtele_type type;
	
	message_stack = xtele_object_prop_get(sender, "message");
	
	xtele_stack_pop(message_stack);
	ptype = xtele_stack_get_data(message_stack);
	if(ptype)
		type = atoi(ptype);

	nchars = calloc(cbSize + 1, sizeof(char));
	memcpy(nchars, chars, cbSize);

	switch(type) {
		default:
			free(nchars);
			break;
		case XTELE_TYPE_LIST_INT:
		case XTELE_TYPE_INT:
			data_int = malloc(sizeof(int));
			*data_int = atoi(nchars);
			xtele_stack_push(message_stack, data_int);
			free(nchars);
			break;
		case XTELE_TYPE_LIST_STRING:
		case XTELE_TYPE_STRING:
			xtele_stack_push(message_stack, nchars);
			break;
	}
}

static void xtele_xml_element_end(char* qName, void *sender) {
	xtele_stack *message_stack;
	
	message_stack = xtele_object_prop_get(sender, "message");
	if(streq( qName, "xtele")) {
		xtele_message_listen_unregister(sender);
	} else if(streq( qName, "el")) {
		void* data;
		xtele_list* list;
		xtele_type* ptype;

		data = xtele_stack_pop(message_stack);
		ptype = xtele_stack_pop(message_stack);
		list = xtele_stack_get_data(message_stack);

		xtele_stack_set_data(message_stack, xtele_list_append(list, data));
		xtele_stack_push(message_stack, ptype);
	} else if(streq(qName, "list")) {
		free(xtele_stack_pop(message_stack));
	} else if(streq( qName, "prop")) {
		void* data;
		xtele_prop* prop;

		data = xtele_stack_pop(message_stack);
		free(xtele_stack_pop(message_stack));
		prop = xtele_stack_get_data(message_stack);

		prop->data = data;
	} else if(streq( qName, "message")) {
		xtele_object* message;
		int (*message_handler) (xtele_object*, void*);
		void* message_handler_data;
		void* data;

		message_handler = xtele_object_prop_get(sender, "message_handler");
		message_handler_data = xtele_object_prop_get(sender, "message_handler_data");

		data = xtele_stack_pop(message_stack);
		free(xtele_stack_pop(message_stack));
		message = xtele_stack_pop(message_stack);

		xtele_object_prop_change_data(message, "data", data);
		if(message && message_handler) {
			message_handler(message, message_handler_data);
			xtele_prop_destroy(message);
		}
	}
}

static void xtele_message_check_modules(fd_set* set) {
	xtele_list* module_list = listen_list;
	xtele_object* module;
	int* file;

	while(module_list) {
		module = module_list->data;
		file = xtele_object_prop_get(module, "file");
		xtele_list_next(module_list);
		if(FD_ISSET(*file, set)) {
				xtele_xml_parse(
						xtele_xml_get_data,
						xtele_xml_element_start,
						xtele_xml_content,
						xtele_xml_element_end,
						file, module);
		}
		FD_SET(*file, set);
	}
}

/** Enter the main 'listening loop' */
void xtele_message_listen(void) {
	fd_set set;

	running = 1;
	FD_ZERO(&set);
	while(running) {
		xtele_message_check_modules(&set);
		if(running)
			select(FD_SETSIZE, &set, NULL, NULL, NULL);
	}
}

/** Register a new module which will be able to send and receive messages 
 * @param module The new module to register.
 * @param message_handler The function which will be called when a message is received.
 * @param data This will be passed to @p message_handler each time it is called.
 */
void xtele_message_listen_register(xtele_object* module, void (*message_handler) (xtele_object*, void*), void* data) {
	xtele_object_prop_change_data(module, "message_handler", message_handler);
	xtele_object_prop_change_data(module, "message_handler_data", data);
	
	listen_list = xtele_list_append(listen_list, module);
}

/** Removes a module from the listening main loop.
 * It won't be able to receive or send messages anymore.
 * @param module The module to unregister.
 */
void xtele_message_listen_unregister(xtele_object* module) {
	printf("%s\n", module->name);
	listen_list = xtele_list_remove(listen_list, memeq, module, NULL);
}	

/** Stop the main loop initiated by ::xtele_message_listen */
void xtele_message_listen_stop(void) {
	running = 0;
}

/** Cleaning function */
void xtele_message_uninit(void) {
	xtele_list_destroy(listen_list, NULL);
}
/** @} */
