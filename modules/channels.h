//#include "utils.h"
#include "xtelelib.h"

typedef xtele_listb channel_list;
typedef xtele_prop channel;

#define channel_get_freq(chan) (*((int*) XTELE_PROP(chan->data)->data))
#define channel_get_name(chan) (XTELE_PROP(chan->data)->name)

