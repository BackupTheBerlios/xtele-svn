/** @file 
 * Miscallenous utility functions.
 * @ingroup libxtele
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include "xtelelib.h"

static DebugLevel G_debug_level = DEBUG_ALL;

/** @addtogroup libxtele
 * @{
 */
/** Sets the internal debug level.
 * All ::xtele_print calls with level > DebugLevel will be printed.
 * @param level The new debug level.
 */
void xtele_set_debuglevel(DebugLevel level) {
	G_debug_level = level;
}

/** Custom print function.
 * It uses the same syntax than printf.
 @param level The debug level of the message printed.
 @param object The name of the object printing the message.
 @param format 'printf' style format string...
 */
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

/** Compares 2 strings.
 * If the strings are different it returns 0.
 * This function is case-sensitive.
 * @param chaine1 The 1st string.
 * @param chaine2 The 2nd string.
 * @return 1 if the 2 strings are equal.
 */
int streq(const char* chaine1, const char* chaine2) {
	return !strcmp(chaine1, chaine2);
}

/** Concatenetes 2 strings. */
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

/** Compares 2 pointers.
 * If the pointers are different it returns 0.
 * @param p1 The 1st string.
 * @param p2 The 2nd string.
 * @return 1 if the 2 pointers are the same.
 */
int memeq(void* p1, void* p2) {
	return p1 == p2;
}

/** Set the O_NONBLOCK flag on file descriptors, for non-blocking reading et writing operations. */
void xtele_set_nonblock(int file) {
	int oldflags = fcntl(file, F_GETFL, 0);

	if (oldflags != -1) {
		oldflags |= O_NONBLOCK;
		fcntl(file, F_SETFL, oldflags);
	}
}

/** Returns the user directory ($HOME under UNIX).
 * @warning Don't free the result of this function !
 */
char* xtele_userdir(void) {
	return getenv("HOME");
}

/** Returns the user configuration directory ($HOME/.xtele under UNIX).
 * You must free the result after use.
 */
char* xtele_userconfdir(void) {
	char *userdir, *userconfdir, *relative = "/.xtele/";

	userdir = xtele_userdir();
	userconfdir = xt_strcat(userdir, relative);

	return userconfdir;
}

/** Returns the user configuration file ($HOME/.xtele/xtele.conf under UNIX).
 * You must free the result after use.
 */
char* xtele_userconffile(void) {
	char *userconffile, *userconfdir, *conffile = "xtele.conf";

	userconfdir = xtele_userconfdir();
	userconffile = xt_strcat(userconfdir, conffile);
	free(userconfdir);

	return userconffile;
}

/** Returns the user module directory ($HOME/.xtele/modules under UNIX).
 * You must free the result after use.
 */
char* xtele_usermoduledir(void) {
	char *usermoduledir, *userconfdir, *moduledir = "modules/";

	userconfdir = xtele_userconfdir();
	usermoduledir = xt_strcat(userconfdir, moduledir);
	free(userconfdir);

	return usermoduledir;
}
/** @} */
