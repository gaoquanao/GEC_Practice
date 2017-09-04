#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__


#include <stdio.h>
#include "nopoll.h"

int init_websocket_v12();
int websocket_send(void * msg, unsigned int len);



#endif
