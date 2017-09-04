#ifndef __ALBUM_H__
#define __ALBUM_H__

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "list.h"

typedef struct node
{
	char path[256];
	struct node *prev;
	struct node *next;
}listnode, *my_list;

int album();

#endif