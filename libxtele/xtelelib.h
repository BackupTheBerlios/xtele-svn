#ifndef __XTELELIB_H_
#define __XTELELIB_H_
/** @file 
 * General header file of the libxtele library.
 @ingroup libxtele
 */

/** @defgroup libxtele libxtele library
 * All the files contained in the directory libxtele form the library libxtele.
 */
 
/** @addtogroup xtele_list 
 * @{
 */
/** Basic struct for ::xtele_list, ::xtele_listb and ::xtele_stack  */
struct _xtele_list {
	void* data; /**< Data of the current list element */
	struct _xtele_list* next; /**< Next element of the linked list */
	struct _xtele_list* prev; /**< Previous element of the linked list */
};
typedef struct _xtele_list xtele_list; /**< Linked list */

#define XTELE_LIST(list) ((xtele_list*) list) /**< Convert a c-type to ::xtele_list* */
#define FREE_FUNC(func) ((void (*) (void*)) func) /**< Convert a c-type function pointer to the syntax used by ::xtele_list_remove and ::xtele_list_remove_element. */
#define FOREACH_FUNC(func) ((int (*) (void*, void*)) func) /**< Convert a c-type function pointer to the syntax used by ::xtele_list_foreach. */
#define FIND_FUNC(func) FOREACH_FUNC(func) /**< Convert a c-type function pointer to the syntax used by ::xtele_list_find; */
#define xtele_list_next(list) (list = list->next) /**< switch to the next ::xtele_list element. */
#define xtele_list_prev(list) (list = list->prev) /**< switch to the previous ::xtele_list element. */
#define xtele_list_is_emty(list) !list /**< Return whether a list is empty. */
xtele_list* xtele_list_new(void);
xtele_list* xtele_list_element_new(void* data);
void xtele_list_destroy_one(xtele_list* list, void (*free_func) (void*) );
void xtele_list_destroy(xtele_list* list, void (*free_func) (void*) );
xtele_list* xtele_list_first(xtele_list* list);
xtele_list* xtele_list_last(xtele_list* list);
xtele_list* xtele_list_insert(xtele_list* list, void* data);
xtele_list* xtele_list_add(xtele_list* list, void* data);
xtele_list* xtele_list_append(xtele_list* list, void* data);
//xtele_list* xtele_list_remove_element(xtele_list* list,  void (*free_func) (void*));
xtele_list* xtele_list_remove(xtele_list* list, int (*del_func) (void*, void*), void* data, void (*free_func) (void*));
xtele_list* xtele_list_foreach(xtele_list* debut, int (*foreach_func) (void*, void*), void* data);
void* xtele_list_find(xtele_list* debut, int (*foreach_func) (void*, void*), void* data);
xtele_list* xtele_list_find_string(xtele_list* list, char* name);
/** @} */

/** @addtogroup xtele_listb 
 * @{
 */
typedef struct _xtele_list xtele_listb; /**< Loop linked list */
		
#define XTELE_LISTB(listb) ((xtele_listb*) listb) /**< Convert a c-type to ::xtele_listb* */
#define xtele_listb_next(listb) xtele_list_next(listb) /**< switch to the next ::xtele_listb element. */
#define xtele_listb_prev(listb) xtele_list_prev(listb) /**< switch to the previous ::xtele_listb element. */
xtele_listb* xtele_listb_new(void* data);
xtele_listb* xtele_listb_insert(xtele_listb* list, void* data);
/** @} */

typedef struct _xtele_list xtele_stack; /**< Stack @ingroup xtele_stack */

xtele_stack* xtele_stack_new(void);
void xtele_stack_destroy(xtele_stack* stack);
void xtele_stack_push(xtele_stack* stack, void* data);
void* xtele_stack_pop(xtele_stack* stack);
void* xtele_stack_get_data(xtele_stack* stack);
void xtele_stack_set_data(xtele_stack* stack, void* data);

/** @addtogroup xtele_prop
 * @{
 */
/** Different types to describe kinds of ::xtele_prop */
typedef enum {
	XTELE_TYPE_UNKNOWN, /**< Unknown type. */
	XTELE_TYPE_NULL, /**< NULL type. */
	XTELE_TYPE_INT, /**< integer type. */
	XTELE_TYPE_STRING, /**< string type. */
	XTELE_TYPE_PROP, /**< ::xtele_prop type. */
	XTELE_TYPE_LIST_UNKNOWN = 100, /**< ::xtele_list of unknown type. */
	XTELE_TYPE_LIST_NULL, /**< ::xtele_list of NULL type. */
	XTELE_TYPE_LIST_INT, /**< ::xtele_list of integer type. */ 
	XTELE_TYPE_LIST_STRING, /**< ::xtele_list of string type. */
	XTELE_TYPE_LIST_PROP, /**< ::xtele_list of ::xtele_prop type. */
	XTELE_TYPE_OBJECT = XTELE_TYPE_LIST_PROP /**< ::xtele_object type. */
} xtele_type;

/** Representation of a simple property */
typedef struct _xtele_prop xtele_prop;

#define XTELE_INT_MAX_LEN 10 /**< Maximum number of decimal digits of an xtele_type::XTELE_TYPE_INT. */
#define XTELE_PROP(prop) ((xtele_prop*) prop) /**< Convert a c-type to ::xtele_prop* */

xtele_prop* xtele_prop_new(char* name, xtele_type type, void* data);
void xtele_prop_destroy(xtele_prop* prop);
int xtele_prop_is(xtele_prop* prop, char* name);
int xtele_prop_is_type(xtele_prop* prop, int type);
int xtele_prop_print(xtele_prop* prop);
/** @} */

/** @addtogroup xtele_object
 * @{ 
 */
/** Representation of a simple object, which can have mulitple ::xtele_prop 'properties'. */
typedef struct _xtele_prop xtele_object;

#define XTELE_OBJECT(object) ((xtele_object*) object) /**< Convert a c-type to ::xtele_object* */
#define xtele_object_destroy(object) xtele_prop_destroy(object) /**< Destroys a ::xtele_object */
#define xtele_object_prop_add_string(object, name, string) xtele_object_prop_add(object, name, XTELE_TYPE_STRING, string) /**< Adds a new property of type xtele_type::XTELE_TYPE_STRING to an object. */ 
#define xtele_object_prop_add_prop(object, name, prop) xtele_object_prop_add(object, name, XTELE_TYPE_PROP, prop) /**< Adds a new property of type xtele_type::XTELE_TYPE_PROP to an object. */
#define xtele_object_prop_get_data(object, name) xtele_object_prop_get(object, name) /**< Property of an object. */
/** @} */

xtele_object* xtele_object_new(char* name);
void xtele_object_prop_add(xtele_object* object, char* name, xtele_type type, void* data);
void xtele_object_prop_add_int(xtele_object* object, char* name, int integer);
void xtele_object_prop_change_data(xtele_object* object, char* name, void* data);
void xtele_object_prop_delete(xtele_object* object, char* name);
void* xtele_object_prop_remove(xtele_object* object, char* name);
void* xtele_object_prop_get(xtele_object* object, char* name);
xtele_prop* xtele_object_prop_get_prop(xtele_object* object, char* name);
xtele_type xtele_object_prop_get_type(xtele_object* object, char* name);

/** Basic struct for ::xtele_prop and ::xtele_object  @ingroup xtele_object xtele_prop*/
struct _xtele_prop {
	char* name; /**< Name of the property or object. */
	xtele_type type;/**< ::xtele_type of the property, or xtele_type::XTELE_TYPE_OBJECT for an ::xtele_object.*/
	void* data; /**< data of the property, or ::xtele_list of ::xtele_prop for an ::xtele_object. */
};

/* Debut xtele_lib */
/** Level of ::xtele_print.
 * This is used as the 2nd argument of ::xtele_print.
 * @see xtele_print
 * @ingroup libxtele
 */
typedef enum {
	NONE, 
	ALERT, /**< Alert level */
	INFO, /**< Info level */
	DEBUG, /**< Debug level */
	DEBUG_2, /**< Higher debug level */
	DEBUG_ALL /**< Debug everything */
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

/** @addtogroup xtele_message
 * @{
 */
#define XTELE_MESSAGE_LISTEN_FUNC(func) ((void (*) (xtele_prop*, xtele_object*, void*)) func) /**< Convert a c-type function pointer to the syntax used by ::xtele_message_listen_register. */
#define xtele_message_type(message) xtele_object_prop_get_type(message, "data") /**< ::xtele_type of a message */
#define xtele_message_data(message) xtele_object_prop_get(message, "data") /**< Data of a message */
#define xtele_message_sender(message) xtele_object_prop_get(message, "sender/name") /**< Sender of a message */
#define xtele_message_is(message, text) streq(message->name, text) /**< Tells if it is the requested message */
void xtele_message_send(xtele_object* dest, xtele_object* message);
void xtele_message_send_long(xtele_object* dest, char* sender_name, char* message_name, xtele_type type, void* data);
void xtele_message_send_void(xtele_object* dest, char* message);
void xtele_message_send_string(xtele_object* dest, char* message, char* string);
void xtele_message_send_int(xtele_object* dest, char* message, int data);
void xtele_message_send_prop(xtele_object* dest, char* message, xtele_prop* prop);
void xtele_message_listen(void);
void xtele_message_listen_register(xtele_object* module, void (*message_handler) (xtele_object*, void*), void* data);
void xtele_message_listen_unregister(xtele_object* module);
void xtele_message_listen_stop(void);
/* @} */

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
