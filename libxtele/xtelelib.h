#ifndef __XTELELIB_H_
#define __XTELELIB_H_

/* Debut xtele_list */
typedef struct _xtele_list xtele_list;
typedef struct _xtele_list xtele_listb;
typedef struct _xtele_list xtele_stack;
struct _xtele_list {
	void* data;
	xtele_list* next;
	xtele_list* prev;
};

#define XTELE_LIST(list) ((xtele_list*) list)
xtele_list* xtele_list_new(void);
xtele_list* xtele_list_element_new(void* data);
void xtele_list_destroy_one(xtele_list* list, void (*free_func) (void*) );
void xtele_list_destroy(xtele_list* list, void (*free_func) (void*) );
xtele_list* xtele_list_first(xtele_list* list);
xtele_list* xtele_list_last(xtele_list* list);
xtele_list* xtele_list_insert(xtele_list* list, void* data);
xtele_list* xtele_list_add(xtele_list* list, void* data);
xtele_list* xtele_list_append(xtele_list* list, void* data);
xtele_list* xtele_list_remove_element(xtele_list* list,  void (*free_func) (void*));
xtele_list* xtele_list_remove(xtele_list* list, int (*del_func) (void*, void*), void* data, void (*free_func) (void*));
xtele_list* xtele_list_foreach(xtele_list* debut, int (*foreach_func) (void*, void*), void* data);
void* xtele_list_find(xtele_list* debut, int (*foreach_func) (void*, void*), void* data);
xtele_list* xtele_list_find_string(xtele_list* list, char* name);
#define xtele_list_next(list) (list = list->next)
#define xtele_list_prev(list) (list = list->prev)
#define FREE_FUNC(func) ((void (*) (void*)) func)
#define FOREACH_FUNC(func) ((int (*) (void*, void*)) func)
#define FIND_FUNC(func) FOREACH_FUNC(func)


xtele_stack* xtele_stack_new(void);
void xtele_stack_destroy(xtele_stack* stack);
void xtele_stack_push(xtele_stack* stack, void* data);
void* xtele_stack_pop(xtele_stack* stack);
void* xtele_stack_get_data(xtele_stack* stack);
void xtele_stack_set_data(xtele_stack* stack, void* data);

#define XTELE_LISTB(listb) ((xtele_listb*) listb)
xtele_listb* xtele_listb_new(void* data);
xtele_listb* xtele_listb_insert(xtele_listb* list, void* data);
xtele_listb* xtele_listb_remove(xtele_listb* list);
#define xtele_listb_next(listb) xtele_list_next(listb)
#define xtele_listb_prev(listb) xtele_list_prev(listb)
/* Fin xtele_list */


/* xtele_objects */
enum _xtele_type {
	XTELE_TYPE_UNKNOWN,
	XTELE_TYPE_NULL,
	XTELE_TYPE_INT,
	XTELE_TYPE_STRING,
	XTELE_TYPE_PROP,
	XTELE_TYPE_LIST_UNKNOWN = 100,
	XTELE_TYPE_LIST_NULL,
	XTELE_TYPE_LIST_INT,
	XTELE_TYPE_LIST_STRING,
	XTELE_TYPE_LIST_PROP,
	XTELE_TYPE_OBJECT = XTELE_TYPE_LIST_PROP
};
typedef enum _xtele_type xtele_type;

#define XTELE_INT_MAX_LEN 10

typedef struct _xtele_prop xtele_prop;
typedef struct _xtele_prop xtele_object;
struct _xtele_prop {
	char* name;
	xtele_type type;
	void* data;
};

#define XTELE_OBJECT(object) ((xtele_object*) object)
xtele_object* xtele_object_new(char* name);
#define xtele_object_destroy(object) xtele_prop_destroy(object)
void xtele_object_prop_add(xtele_object* object, char* name, xtele_type type, void* data);
void xtele_object_prop_add_int(xtele_object* object, char* name, int integer);
#define xtele_object_prop_add_string(object, name, string) xtele_object_prop_add(object, name, XTELE_TYPE_STRING, string)
#define xtele_object_prop_add_prop(object, name, prop) xtele_object_prop_add(object, name, XTELE_TYPE_PROP, prop)
void xtele_object_prop_change_data(xtele_object* object, char* name, void* data);
void xtele_object_prop_delete(xtele_object* object, char* name);
void* xtele_object_prop_remove(xtele_object* object, char* name);
void* xtele_object_prop_get(xtele_object* object, char* name);
xtele_prop* xtele_object_prop_get_prop(xtele_object* object, char* name);
xtele_type xtele_object_prop_get_type(xtele_object* object, char* name);
#define xtele_object_prop_get_data(object, name) xtele_object_prop_get(object, name)

xtele_object* xtele_module_new(char* name, char* sender_name, int file);
xtele_object* xtele_message_new(char* message_name, char* sender_name, xtele_type type, void* data);
#define xtele_message_type(message) xtele_object_prop_get_type(message, "data")
#define xtele_message_data(message) xtele_object_prop_get(message, "data")
#define xtele_message_sender(message) xtele_object_prop_get(message, "sender/name")
#define xtele_message_is(message, text) streq(message->name, text)

#define XTELE_PROP(prop) ((xtele_prop*) prop)
xtele_prop* xtele_prop_new(char* nom_prop, xtele_type type, void* data);
void xtele_prop_destroy(xtele_prop* prop);
int xtele_prop_is(xtele_prop* prop, char* name);
/* Fin xtele_object */


/* Debut xtele_lib */
typedef enum {
	NONE,
	ALERT,
	INFO,
	DEBUG,
	DEBUG_2,
	DEBUG_ALL
} DebugLevel;

void xtele_set_debuglevel(DebugLevel level);
void xtele_print(DebugLevel level, char* object, char* format, ...);
int streq(const char* chaine1,const char* chaine2);
char* xt_strcat(char* chaine1, char* chaine2);
int memeq(void* p1, void* p2);
void xtele_set_nonblock(int file);
char* xtele_userdir(void);
char* xtele_userconfdir(void);
char* xtele_userconffile(void);
char* xtele_usermoduledir(void);
/* Fin xtele_lib */


/* Debut xtele_message */
#define XTELE_MESSAGE_LISTEN_FUNC(func) ((void (*) (xtele_prop*, xtele_object*, void*)) func)
void xtele_message_send(xtele_object* dest, xtele_object* message);
void xtele_message_send_long(xtele_object* dest, char* sender_name, char* message_name, xtele_type type, void* data);
void xtele_message_send_void(xtele_object* dest, char* message);
void xtele_message_send_string(xtele_object* dest, char* message, char* string);
void xtele_message_send_int(xtele_object* dest, char* message, int data);
void xtele_message_send_prop(xtele_object* dest, char* message, xtele_prop* prop);
void xtele_message_close(xtele_object* dest);
void xtele_message_listen(void);
void xtele_message_listen_register(xtele_object* module, void (*message_handler) (xtele_object*, void*), void* data);
void xtele_message_listen_unregister(xtele_object* module);
void xtele_message_listen_stop(void);
/* Fin xtele_message */


/* Debut xtele_xml_parse */
char* xtele_xml_att_get(char** atts, char* name);
void xtele_xml_parse(
		void (*user_input) (char*, int, int*, void*),
		void (*user_start_element) (char*, char**, void*),
		void (*user_content) (char*, int, void*),
		void (*user_end_element) (char*, void*),
		void* user_input_data,
		void* user_data);
/* Fin xtele_xml_parse */


/* Debut xtele_xml_writer */
xtele_object* xtele_xml_writer_new(void (*xml_writer) (char* , void* ), void* user_data);
void xtele_xml_writer_add_element(xtele_object* writer, char* element_name);
void xtele_xml_writer_add_attribute(xtele_object* writer, char* att_name, char* att_value);
void xtele_xml_writer_add_text(xtele_object* writer, char* text);
void xtele_xml_writer_end_element(xtele_object* writer);
/* Fin xtele_xml_writer */


/* Debut xtele_conf */
xtele_list* xtele_conf_parse(void);
/* Fin xtele_conf */

#endif 
