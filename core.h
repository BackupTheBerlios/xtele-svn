#ifndef __CORE_H_
#define __CORE_H_

#include "xtelelib.h"

void xtele_core_broadcast(xtele_prop* message, void* data);
xtele_object* xtele_core_message_listen(void);
void xtele_core_module_add(char* name, int file);
xtele_prop* xtele_core_module_prop_find(char* object_name, char* prop_name);
void xtele_core_module_message_add(xtele_object* module, char* event_type);
int xtele_core_module_message_match(xtele_object* module, char* event_type);
xtele_prop* xtele_core_conf_get(char* module, char* prop_name);
void xtele_core_quit(void);
void xtele_core_module_load( char* name, char* filename);
xtele_object* xtele_core_module_start(char* name, void (*ext_init) (xtele_object* ));
void xtele_core_init(void);

xtele_object* core_app;

#endif
