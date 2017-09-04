#ifndef __TS_H__
#define __TS_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include "tslib.h"


struct parameter
{
	unsigned int pos_x;
	unsigned int pos_y;
	unsigned int press;
};

void *get_ts(void  * arg);

int my_ts_open();
int get_x_y(int *x,int *y);
int my_ts_close();

#endif