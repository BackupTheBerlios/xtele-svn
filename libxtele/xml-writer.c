#include <stdlib.h>
#include <string.h>
#include "xtelelib.h"

/** @defgroup xtele_xml_write xml writing
 * xml functions for writing xml files.
 * @see xtele_xml_parsing xtele_object
 * @ingroup xtele_xml
 * @{
 */

/** Creates a new xml_writer object.
 * The xml_writer object created can then be used by all the xml_writer_* functions. A unique xml_writer object may be used by several functions. It must be destroyed by ::xtele_object_destroy after use.
 * @param xml_writer The function used to output xml data.
 * @param user_data Data passed to xml_writer each time it is called.
 * @return a xml_writer object used by xtele_xml_* functions.
 */
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

/** Begins an element in the xml stream.
 * @param writer an xml_writer object given by ::xtele_xml_writer_new.
 * @param element_name name of the element to begin.
 */
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

/** Adds an attribute to an element.
 * @param writer an xml_writer object given by ::xtele_xml_writer_new.
 * @param att_name name of the attribute to add.
 * @param att_value value of the attribute.
 */
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

/** Adds content to an element. 
 * @param writer an xml_writer object given by ::xtele_xml_writer_new.
 * @param text The content to add.
 */
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

/** Ends an element in the xml stream.
 * The data are actually written.
 * It must follow an ::xtele_xml_writer_add_element.
 * @param writer an xml_writer object given by ::xtele_xml_writer_new.
 */
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
/* @} */
