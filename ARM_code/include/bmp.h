#ifndef  __BMP_H__
#define __BMP_H__


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

int proc_bmp_800_480(char *bmp_path, unsigned int * lcd_buff);
int proc_bmp_not_800_480(char *bmp_path,unsigned int * lcd_buff,unsigned int x,unsigned int y);
int get_bmp_w_h(char * bmp_info_buff,unsigned int *w,unsigned int *h);
int find_bmp(char * bmp_path);

#endif