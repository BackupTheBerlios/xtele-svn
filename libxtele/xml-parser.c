#include <stdlib.h>
#include <string.h>

/** @defgroup xtele_xml xtele xml
 * general xml functions for parsing and outputting xml files in xtele. 
 * @ingroup libxtele
 */

static int xml_find_char(char* buf, char find, int start, int end) {
	int i;
	
	for(i = start; i < end ; i++)
		if(buf[i] == find)
			break;

	return i;
}

static int xml_skip_blank(char* buf, int start, int end) {
	int i;
	
	for(i = start; i < end ; i++)
		if(buf[i] != ' ' && buf[i] != '\n'  && buf[i] != '\r' && buf[i] != '\t')
			break;

	return i;
}

static char** xml_parse_attributes(char* buf, int start, int end) {
	char* attributes_content;
	char** attribute_list = NULL;
	int att_len, cursor = 0, nb_att = 0;

	att_len = end - start;
	attributes_content = malloc(att_len * sizeof(char));
	memcpy(attributes_content, buf + start, att_len * sizeof(char));

	while(cursor < att_len) {
		nb_att++;
		attribute_list = realloc(attribute_list, (nb_att * 2 + 1) * sizeof(char*));
		attribute_list[2 * (nb_att - 1)] = attributes_content + cursor;
		cursor = xml_find_char(attributes_content, '=', cursor, att_len);
		attributes_content[cursor] = '\0';
		cursor = xml_find_char(attributes_content, '"', cursor, att_len);
		attributes_content[cursor] = '\0';
		attribute_list[2 * (nb_att - 1) + 1] = attributes_content + cursor + 1;
		cursor = xml_find_char(attributes_content, '"', cursor, att_len);
		attributes_content[cursor] = '\0';
		cursor++;
		cursor = xml_skip_blank(attributes_content, cursor, att_len);
	}
	attribute_list[nb_att * 2] = NULL;

	return attribute_list;
}

static void xml_parse_start_element(char* buf, int start, int end,
		void (*user_start_element) (char*, char**, void*), void* user_data) {
	
	int local_start, local_end;
	char *name, **attribute = NULL;

	local_end = xml_find_char(buf, ' ', start, end);
	name = calloc(local_end - start + 1, sizeof(char));
	memcpy(name, buf + start, (local_end - start)* sizeof(char));

	local_start = xml_skip_blank(buf, local_end, end);
	if(local_start < end) {
		attribute = xml_parse_attributes(buf, local_start, end);
	}

	if(user_start_element)
		user_start_element(name, attribute, user_data);
	
	if(attribute) {
		free(*attribute);
		free(attribute);
	}
	free(name);
}

static void xml_parse_end_element(char* buf, int start, int end,
		void (*user_end_element) (char*, void*), void* user_data) {

	char* name;
	int name_end;

	name_end = xml_find_char(buf, ' ', start, end);
	name = calloc(name_end - start + 1, sizeof(char));
	memcpy(name, buf + start, (name_end - start) * sizeof(char));

	if(user_end_element)
		user_end_element(name, user_data);

	free(name);
}

/** @defgroup xtele_xml_parsing xml parsing
 * xml functions for parsing xml files.
 * @ingroup xtele_xml
 * @see xtele_xml_write 
 * @{
 */

/** Retrieves the content of an attribute given its name.
 * It is designed to be used inside of the 'user_start_element' given to ::xtele_xml_parse. By passing the 2nd argument as the 'atts' argument.
 * @warning The returned string must not be freed !
 * @param atts 2nd argument of 'user_start_element'.
 * @param name The xml attribute we want to retrieve.
 * @return The xml attribute.
 */
char* xtele_xml_att_get(char** atts, char* name) {
	if(atts)
		while(*atts) {
			if(!strcmp(atts[0], name))
				return atts[1];
			
			atts += 2;
		}
	return NULL;
}

/** xml parsing.
 * Parses an xml stream. It is 'event oriented'.
 * It gets input by a user function and call a handler each time an xml node is opened, closed or content is encountered.
 * @param user_input function by which it gets xml data.
 * @param user_start_element function called at the beginning of a node.
 * @param user_content function called when content is encountered
 * @param user_end_element function called at the end of a node.
 * @param user_input_data data passed as the last argument of user_input.
 * @param user_data data passed as the last argument of each parsing function (user_start_element, user_content and user_end_element.
 */
void xtele_xml_parse(
		void (*user_input) (char*, int, int*, void*),
		void (*user_start_element) (char*, char**, void*),
		void (*user_content) (char*, int, void*),
		void (*user_end_element) (char*, void*),
		void* user_input_data,
		void* user_data) {

	char *buf;
	int element_start, element_end = 0, offset = 0;
	int r = 1, taille_buf = 512, buf_read;
	
	buf = malloc(taille_buf);
	while(r > 0) {
		user_input(buf + offset, taille_buf - offset, &r, user_input_data);
		if(r <= 0)
			break;
		element_end = 0;
		buf_read = r + offset;
		while(element_end <= buf_read) {
			element_start = xml_find_char(buf, '<', element_end, buf_read);

			if(element_start >= buf_read) {
				offset = buf_read - element_end;
				if(offset >= taille_buf / 2) {
					taille_buf *= 2;
					buf = realloc(buf, taille_buf);
				}					
				memmove(buf, buf + element_end, offset);
				break;
			}

			if(element_start - element_end > 0) {
				int blank;

				blank = xml_skip_blank(buf, element_end, element_start);
				if(element_start - blank > 0 && user_content)
					user_content(buf + element_end, element_start - element_end, user_data);
			}
			element_end = xml_find_char(buf, '>', element_start, buf_read);

			if(element_end >= buf_read) {
				offset = buf_read - element_start;
				if(offset >= taille_buf / 2) {
					taille_buf *= 2;
					buf = realloc(buf, taille_buf);
				}					
				memmove(buf, buf + element_start, offset);
				break;
			}

			if(buf[element_start + 1] == '/') {
				xml_parse_end_element(buf, element_start + 2, element_end, user_end_element, user_data);
			} else if(buf[element_end - 1] == '/') {
				xml_parse_start_element(buf, element_start + 1, element_end -1 , user_start_element, user_data);
				xml_parse_end_element(buf, element_start + 1, element_end -1 , user_end_element, user_data);
			} else {
				xml_parse_start_element(buf, element_start + 1, element_end, user_start_element, user_data);
			}
			element_end++;
		}
	}
	free(buf);
}
/* @} */
