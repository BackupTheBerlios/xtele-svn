#include "core.h"
/** @file 
 * Main.
 */
 
/** @mainpage 
 * xtele is a modular TV software for linux.\n
 * webpage : http://xtele.berlios.de
 */ 

/** Main function.
 * This is the main funtion
 */
int main(int argc, char **argv) {
	xtele_core_init();

	xtele_print(INFO, "xtele", "Exiting...\n");
	return 0;
}
