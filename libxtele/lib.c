#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include "xtelelib.h"

static DebugLevel G_debug_level = DEBUG_ALL;

void xtele_set_debuglevel(DebugLevel level) {
	G_debug_level = level;
}

void xtele_print(DebugLevel level, char* object, char* format, ...) {
	if(level <= G_debug_level) {
		va_list ap;

		if(object)
			fprintf(stderr, "'%s' - ", object);
		va_start(ap, format);
		vfprintf(stderr, format, ap);
		va_end(ap);
	}
}

int streq(const char* chaine1, const char* chaine2) {
	return !strcmp(chaine1, chaine2);
}

char* xt_strcat(char* chaine1, char* chaine2) {
	char* chaine_cat;
	int chaine1_len, chaine2_len;

	chaine1_len = strlen(chaine1);
	chaine2_len = strlen(chaine2) + 1;
	chaine_cat = malloc((chaine1_len + chaine2_len) * sizeof(char));
	memcpy(chaine_cat, chaine1, chaine1_len);
	memcpy(chaine_cat + chaine1_len, chaine2, chaine2_len);

	return chaine_cat;
}

int memeq(void* p1, void* p2) {
	return p1 == p2;
}

void xtele_set_nonblock(int file) {
	int oldflags = fcntl(file, F_GETFL, 0);

	if (oldflags != -1) {
		oldflags |= O_NONBLOCK;
		fcntl(file, F_SETFL, oldflags);
	}
}

char* xtele_userdir(void) {
	return getenv("HOME");
}

char* xtele_userconfdir(void) {
	char *userdir, *userconfdir, *relative = "/.xtele/";

	userdir = xtele_userdir();
	userconfdir = xt_strcat(userdir, relative);

	return userconfdir;
}

char* xtele_userconffile(void) {
	char *userconffile, *userconfdir, *conffile = "xtele.conf";

	userconfdir = xtele_userconfdir();
	userconffile = xt_strcat(userconfdir, conffile);
	free(userconfdir);

	return userconffile;
}

char* xtele_usermoduledir(void) {
	char *usermoduledir, *userconfdir, *moduledir = "modules/";

	userconfdir = xtele_userconfdir();
	usermoduledir = xt_strcat(userconfdir, moduledir);
	free(userconfdir);

	return usermoduledir;
}
