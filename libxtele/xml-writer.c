#include <stdlib.h>
#include <string.h>
#include "xtelelib.h"

xtele_object* xtele_xml_writer_new(void (*xml_writer) (char* , void* ), void* user_data) {
	xtele_object* writer = NULL;

	if(xml_writer) {
		writer = xtele_object_new("writer");
		xtele_object_prop_add(writer, "xml/writer/func", XTELE_TYPE_UNKNOWN, xml_writer);
		xtele_object_prop_add(writer, "xml/writer/func/userdata", XTELE_TYPE_UNKNOWN, user_data);
		xtele_object_prop_add(writer, "xml/writer/document", XTELE_TYPE_UNKNOWN, xtele_stack_new());
	}
	
	return writer;
}

void xtele_xml_writer_add_element(xtele_object* writer, char* element_name) {
	xtele_object* element;
	xtele_stack* doc;
	xtele_list* att_list;

	if(writer && element_name) {
		doc = xtele_object_prop_get(writer, "xml/writer/document");

		element = xtele_object_new(element_name);
		att_list = xtele_list_new();
		xtele_object_prop_add_int(element, "written", 0);
		xtele_object_prop_add(element, "attributes", XTELE_TYPE_LIST_PROP, att_list);

		xtele_stack_push(doc, element);
	}
}

static void xtele_xml_writer_output_element(xtele_object* writer) {
	xtele_stack* doc;
	xtele_object* element;
	int *written;

	doc = xtele_object_prop_get(writer, "xml/writer/document");
	element = xtele_stack_get_data(doc);

	if(element) {
		written = xtele_object_prop_get(element, "written");
		if(! *written ) {
			void (*xml_writer) (char* , void* );
			void* data;
			xtele_list* att_list;
			
			xtele_stack_pop(doc);
			xtele_xml_writer_output_element(writer);
			xtele_stack_push(doc, element);

			xml_writer = xtele_object_prop_get(writer, "xml/writer/func");
			data = xtele_object_prop_get(writer, "xml/writer/func/userdata");
			att_list = xtele_object_prop_get(element, "attributes");

			xml_writer("<", data);
			xml_writer(element->name, data);
			while(att_list) {
				xtele_prop* att;
				
				att = att_list->data;
				xml_writer(" ", data);
				xml_writer(att->name, data);
				xml_writer("=\"", data);
				xml_writer(att->data, data);
				xml_writer("\"", data);
				
				xtele_list_next(att_list);
			}

			xtele_object_prop_delete(element, "attributes");
			*written = 1;
			xml_writer(">", data);
		}
	}
}

void xtele_xml_writer_add_attribute(xtele_object* writer, char* att_name, char* att_value) {
	xtele_stack* doc;
	xtele_object* element;
	xtele_object* att;
	xtele_list* att_list;

	if(writer && att_name && att_value) {
		doc = xtele_object_prop_get(writer, "xml/writer/document");
		element = xtele_stack_get_data(doc);
		att_list = xtele_object_prop_get(element, "attributes");

		att = xtele_prop_new(att_name, XTELE_TYPE_STRING, strdup(att_value));
		att_list = xtele_list_append(att_list, att);
		xtele_object_prop_change_data(element, "attributes", att_list);
	}
}

void xtele_xml_writer_add_text(xtele_object* writer, char* text) {
	void (*xml_writer) (char* , void* );
	void* data;

	if(writer && text) {
		xml_writer = xtele_object_prop_get(writer, "xml/writer/func");
		data = xtele_object_prop_get(writer, "xml/writer/func/userdata");

		xtele_xml_writer_output_element(writer);
		xml_writer(text, data);
	}
}

void xtele_xml_writer_end_element(xtele_object* writer) {
	void (*xml_writer) (char* , void* );
	void* data;
	xtele_object* element;
	xtele_stack* doc;

	if(writer) {
		xml_writer = xtele_object_prop_get(writer, "xml/writer/func");
		data = xtele_object_prop_get(writer, "xml/writer/func/userdata");
		doc = xtele_object_prop_get(writer, "xml/writer/document");
		element = xtele_stack_get_data(doc);

		xtele_xml_writer_output_element(writer);
		xml_writer("</", data);
		xml_writer(element->name, data);
		xml_writer(">", data);

		xtele_stack_pop(doc);
		xtele_object_destroy(element);

		if(!xtele_stack_get_data(doc)) {
			xtele_stack_destroy(doc);
			xtele_object_destroy(writer);
		}
	}
}
