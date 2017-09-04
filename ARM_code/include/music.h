#ifndef  __MUSIC_H__
#define __MUSIC_H__


#include <stdio.h>				
#include <fcntl.h>				
#include <errno.h>				
#include <stdlib.h>				
#include <string.h>	
#include <dirent.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include "list.h"

typedef struct music_node
{
	char filepath[256];
	char blur_picpath[256];//模糊图片路径预设
	char little_picpath[256];//小图片路径预设
	struct music_node *prev;
	struct music_node *next;
}music_listnode, *music_list;

music_list find_music();

#endif