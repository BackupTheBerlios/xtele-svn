#include <X11/Xlib.h>
#include <X11/extensions/Xvlib.h>
#include <string.h>
#include <stdio.h>
#include "xtelelib.h"

static Display* disp = NULL;
static Window win = 0;
static int nport = -1;

static void find_adaptators(void) {
	int i, num_adaptator;
	XvAdaptorInfo* adaptator_info;

	disp = XOpenDisplay(NULL);
	XvQueryAdaptors(disp, RootWindow(disp, 0), &num_adaptator, &adaptator_info);
	for(i = 0; i < num_adaptator; i++) {
		int num_encodings, num_atts;
		int j;
		XvEncodingInfo *encoding_info;
		
		if(adaptator_info[i].type & XvVideoMask) {
			nport = adaptator_info[i].base_id;
			xtele_print(DEBUG_2, "xv", "Adaptator '%s' on port '%i' chosen.\n", adaptator_info[i].name,  adaptator_info[i].base_id);
			return;
		}
		XvFreeAdaptorInfo(adaptator_info);
	}
}

static void xv_start() {
	GC gc;
	unsigned int width, height, border, depth;
	int x, y;
	Window root_win;

	if(disp && nport > 0) {
		gc = XCreateGC(disp, win, 0, NULL);
		XGetGeometry(disp, win, &root_win, &x, &y, &width, &height, &border, &depth);

		/* Affichage de la video (Xv) */
		XvSetPortAttribute(disp, nport, XInternAtom(disp, "XV_MUTE", False), 0);
		XvPutVideo(disp, nport, win, gc, 0, 0, 756, 576, 0, 0, width, height);

		XFreeGC(disp, gc);
		XFlush(disp);
	}
}

static void xv_stop() {
	if(disp && nport > 0 && win) {
		XvSetPortAttribute(disp, nport, XInternAtom(disp, "XV_MUTE", False), 1);
		XvStopVideo(disp, nport, win);
		XFlush(disp);
	}
}

static void xv_set_freq(int freq) {
	if(disp) {
		/* Freq mult of 62.5kHz (or 62.5Hz) */
		XvSetPortAttribute(disp, nport, XInternAtom(disp, "XV_FREQ", False), freq / 62500);
		XFlush(disp);
	}
}

void xv_manage_events(xtele_object* message, void* xtele) {
	void* data;

	data = xtele_message_data(message);
	xtele_print(DEBUG_2, "xv", "Received message '%s' from sender : '%s' - Data type : '%i'\n", message->name, xtele_message_sender(message), xtele_message_type(message));
	if(streq(message->name, "xv/start")) {
		xtele_message_send_string(xtele, "x11/prop/get", "window");
	} else if(streq(message->name, "x11/prop")) {
		xtele_prop* prop;

		prop = data;
		if(streq(prop->name, "window")) {
			win = *((int*) prop->data);
			xv_start();
		} 
	} else if(streq(message->name, "xtele/action/quit")) {
		xv_stop();
		XCloseDisplay(disp);
		disp = NULL;
	} else if(streq(message->name, "channel/set/freq")) {
		xv_set_freq(*((int *) data));
	} else if(streq(message->name, "xv/stop")) {
		xv_stop();
	}
}		

void xtele_ext_init(xtele_object* xtele) {
	find_adaptators();
	xtele_message_send_string(xtele, "xtele/message/receive", "xtele/action/quit");
	xtele_message_send_string(xtele, "xtele/message/receive", "channel/set/freq");
	xtele_message_send_string(xtele, "xtele/message/receive", "x11/prop");
	xtele_message_send_string(xtele, "xtele/message/receive", "xv/start");
	xtele_message_send_string(xtele, "xtele/message/receive", "xv/stop");
	xtele_message_listen_register(xtele, xv_manage_events, xtele);
}

