#include "core.h"
/** @file 
 * Main.
 */
 
/** @mainpage 
 * xtele is a modular TV software for linux.\n
 * webpage : http://xtele.berlios.de
 * @htmlonly <a href="http://developer.berlios.de" title="BerliOS Developer"> <img src="http://developer.berlios.de/bslogo.php?group_id=2146" width="124px" height="32px" border="0" alt="BerliOS Developer Logo"></a> @endhtmlonly
 */ 

/** Main function.
 * This is the main funtion
 */
int main(int argc, char **argv) {
	xtele_core_init();

	xtele_print(INFO, "xtele", "Exiting...\n");
	return 0;
}
