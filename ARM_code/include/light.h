#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <stdio.h>
#include <unistd.h>

int open_light(int serial_fd);
int close_light(int serial_fd);


#endif