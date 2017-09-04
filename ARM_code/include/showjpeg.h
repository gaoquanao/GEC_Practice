#ifndef   __SHOWJPEG_H__
#define  __SHOWJPEG_H__

#include <stdio.h>
#include <string.h>
#include <fcntl.h>		 	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "lcdjpg.h"
#include "jpeglib.h"


#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)


void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color);
unsigned long file_size_get(const char *pfile_path);
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half);

#endif