#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/dpms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "xtelelib.h"

#define FEN_X 768
#define FEN_Y 576
#define FS_X 1024
#define FS_Y 768

static Display* disp = NULL;
static Window win = 0;
static char* title;

Cursor x11_transp_curseur(void) {
	unsigned char bm_no_data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	XColor black, dummy;
	Pixmap bm_no;
	Cursor no_ptr;
	Colormap color_map;

	bm_no = XCreateBitmapFromData (disp, win, bm_no_data, 8, 8);
	color_map = DefaultColormapOfScreen(DefaultScreenOfDisplay(disp));
	XAllocNamedColor (disp, color_map, "black", &black, &dummy);
	no_ptr = XCreatePixmapCursor (disp, bm_no, bm_no, &black, &black, 0, 0);

	return no_ptr;
}

void x11_open_disp(xtele_object* xtele) {
	disp  = XOpenDisplay(NULL);
}

void x11_close_disp(void) {
	XCloseDisplay(disp);
	disp = NULL;
}

void x11_cree_window(xtele_object* xtele) {
	XSetWindowAttributes attributes;
	Cursor no_ptr;

	attributes.override_redirect = True;
	attributes.event_mask = ButtonPressMask | KeyPressMask;

	win = XCreateWindow(disp, DefaultRootWindow(disp), 0, 0, FS_X, FS_Y, 0,
			CopyFromParent, CopyFromParent, CopyFromParent,
			CWOverrideRedirect | CWEventMask, &attributes);

	Xutf8SetWMProperties(disp, win, "xtele", NULL, NULL, 0, NULL, NULL, NULL);
	
	/* Creation du curseur tranparent */
	no_ptr = x11_transp_curseur();
	XDefineCursor(disp, win, no_ptr);
	XFreeCursor(disp, no_ptr);
	
	XMapWindow(disp, win);
	XFlush(disp);
}

void x11_destroy_window(void) {
	XUnmapWindow(disp, win);
	XFlush(disp);
}

void x11_mouse_control(xtele_object* xtele) {
	while(disp) {
		XEvent e;
		XNextEvent(disp, &e);
		if(e.type == ButtonPress) {
			char button[9];

			snprintf(button, 9, "button_%i", e.xbutton.button);
			xtele_message_send_string(xtele, "x11/input/mouse", button);
			if(!strcmp(button, "button_4"))
				xtele_message_send_void(xtele, "channels/previous");
			if(!strcmp(button, "button_5"))
				xtele_message_send_void(xtele, "channels/next");
			
		} else if(e.type == KeyPress) {
			char key[7];
			char buffer[2];

			XLookupString((XKeyEvent*) &e, buffer, 2, NULL, NULL);
			snprintf(key, 7, "key_%s", buffer);
			xtele_message_send_string(xtele, "x11/input/key", buffer);
			if(!strcmp(key, "key_q"))
				xtele_message_send_void(xtele, "xtele/action/quit");
		}
	}
}

void x11_manage_events(xtele_object* message, void* xtele) {
	xtele_print(DEBUG_2, "x11", "Received message '%s' from sender '%s' of type : '%i'\n", message->name, xtele_message_sender(message), xtele_message_type(message));
	if(streq(message->name, "x11/start")) {
		pthread_t thread;

		x11_open_disp(xtele);
		x11_cree_window(xtele);
		pthread_create( &thread, NULL, (void*) x11_mouse_control, xtele);
	} else if(xtele_message_is(message,  "x11/stop")) {
		x11_destroy_window();
	} else if(xtele_message_is(message,  "x11/prop/get")) {
		if(streq(xtele_message_data(message), "window") && win) {
			xtele_prop *win_prop;
			int* pwin;

			pwin = malloc(sizeof(int));
			*pwin = win;
			win_prop = xtele_prop_new("window", XTELE_TYPE_INT, pwin);
			xtele_message_send_prop(xtele, "x11/prop", win_prop);
			xtele_prop_destroy(win_prop);
		}
	}
}

void xtele_ext_init(xtele_object* xtele) {
	xtele_message_send_string(xtele, "xtele/message/receive", "x11/start");
	xtele_message_send_string(xtele, "xtele/message/receive", "x11/stop");
	xtele_message_send_string(xtele, "xtele/message/receive", "x11/prop/get");
	xtele_message_listen_register(xtele, x11_manage_events, xtele);
}
