#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "xtelelib.h"

struct termios saved_attributes;

void reset_input_mode (void) {
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void) {
	struct termios tattr;

	if (!isatty (STDIN_FILENO)) {
		fprintf (stderr, "Not a terminal.\n");
		return;
	}

	tcgetattr(STDIN_FILENO, &saved_attributes);

	/* Set the funny terminal modes. */
	tcgetattr (STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

void term_keyboard_control(void* xtele) {
	int c;

	while((c = getchar()) ) {
		char key[7];

		snprintf(key, 7, "key_%c", c);
		xtele_message_send_string(xtele, "xtele/input/key", key);
		if(!strcmp(key, "key_q")) {
			xtele_message_send_void(xtele, "xtele/action/quit");
		} else if(!strcmp(key, "key_s")) {
			xtele_message_send_void(xtele, "x11/start");
			xtele_message_send_void(xtele, "xv/start");
		} else if(!strcmp(key, "key_v")) {
			xtele_message_send_void(xtele, "xv/start");
		} else if(!strcmp(key, "key_u")) {
			xtele_message_send_void(xtele, "x11/stop");
		}
	}
}

void term_keyb_manage_event(xtele_object* message, void* extension) {
	xtele_print(DEBUG_2, "term_keyb_input", "Received message '%s' from '%s' - Data type : '%i'\n", message->name, xtele_message_sender(message), xtele_message_type(message));
	if(streq(message->name, "xtele/action/quit")) {
		reset_input_mode();
	}
}

void xtele_ext_init(xtele_object* xtele) {
	pthread_t thread;

	xtele_message_send_string(xtele, "xtele/message/receive", "xtele/action/quit");
	set_input_mode();
	pthread_create( &thread, NULL, (void*) term_keyboard_control, xtele);
	pthread_detach(thread);
	xtele_message_listen_register(xtele, term_keyb_manage_event, xtele);
}
