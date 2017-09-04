#ifndef __LCD_H__
#define __LCD_H__


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "bmp.h"

int show_bmp(char * bmp_path);
int show_color(unsigned int rgb_hex);
void set_color(unsigned int *buff_lcd,unsigned int rgb_hex);

#endif