#include <stdio.h>
#include "xtelelib.h"

void xtele_ext_init(xtele_object* xtele) {
	xtele_message_send_string(xtele, "xtele/conf/module/get", "prop4");
}
