#include <stdlib.h>
#include "xtelelib.h"
#include "channels.h"

static channel_list* channels = NULL;

channel* channel_new(char* nom, int freq) {
	channel* new_chan;
	int* pfreq;

	pfreq = malloc(sizeof(int));
	*pfreq = freq;
	new_chan = xtele_prop_new(nom, XTELE_TYPE_INT, pfreq);
	
	return new_chan;
}

void channels_append(char* nom, int freq) {
	channel* new_chan;

	new_chan = channel_new(nom, freq);
	channels = xtele_listb_insert(channels, new_chan);
}

void channels_set_current(xtele_object* xtele) {
	if(channels) {
		xtele_print(DEBUG, "channels", "Chaine : %s\n", channel_get_name(channels));
		xtele_message_send_int(xtele, "channel/set/freq", channel_get_freq(channels));
	}
}

void channels_next(xtele_object* xtele) {
	if(channels) {
		channels = channels->next;
		channels_set_current(xtele);
	}
}

void channels_previous(xtele_object* xtele) {
	if(channels) {
		channels = channels->prev;
		channels_set_current(xtele);
	}
}

void channels_manage_events(xtele_object* message, void* xtele) {
	xtele_print(DEBUG_2, "channels", "Received message '%s' from sender : '%s' - Data type : '%i'\n", message->name, xtele_message_sender(message), xtele_message_type(message));
	if(xtele_message_is(message, "channels/next")) {
		channels_next(xtele);
	} else if(xtele_message_is(message, "channels/previous")) {
		channels_previous(xtele);
	} else if(xtele_message_is(message, "xtele/conf/module")) {
		xtele_list* clist;
				
		clist = XTELE_PROP(xtele_message_data(message))->data;
		while(clist) {
			xtele_print(DEBUG, "channels", "Channel : %s, freq : %s\n", XTELE_PROP(clist->data)->name, XTELE_PROP(clist->data)->data);
			channels_append(XTELE_PROP(clist->data)->name, atoi(XTELE_PROP(clist->data)->data));
			xtele_list_next(clist);
		}
//		channels_set_current(xtele);
	}
}

void xtele_ext_init(xtele_object* xtele) {
	xtele_message_send_string(xtele, "xtele/message/receive", "channels/next");
	xtele_message_send_string(xtele, "xtele/message/receive", "channels/previous");
	xtele_message_send_string(xtele, "xtele/conf/module/get", "channels");
	xtele_message_listen_register(xtele, channels_manage_events, xtele);
}
