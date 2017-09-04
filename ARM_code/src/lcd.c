#include "lcd.h"

#define LCD_W 800
#define LCD_H 480
#define LCD_RED 0x00ff0000
#define LCD_PIX_SIZE 4


//显示相关的代码
void set_color(unsigned int *buff_lcd,unsigned int rgb_hex)
{
	// unsigned int buff_lcd[LCD_W * LCD_H] = {0};
	int i = 0;
	int j = 0;
	for(i = 0; i < LCD_H ;i++)
	{
		for(j = 0; j < LCD_W; j++)
		{
			buff_lcd[i*LCD_W + j] = rgb_hex;
		}
	}
}

int show_color(unsigned int rgb_hex)
{
	//打开设备文件/dev/fb0
	int fd_lcd = open("/dev/fb0",O_WRONLY);
	if(-1 == fd_lcd)
	{
		printf("open /dev/fb0 error!\n");
		return -1;
	}

	//构造一个写的数据（封装为函数）
	unsigned int buff_lcd[LCD_W * LCD_H] = {0};
	set_color(buff_lcd,rgb_hex);
	
	//LCD_W = 800,LCD_H = 480,LCD_PIX_SIZE = 4	
	int ret = write(fd_lcd,buff_lcd,LCD_W * LCD_H * LCD_PIX_SIZE);
	if(-1 == ret)
	{
		printf("write lcd_buff to lcd error!\n");
		close(fd_lcd);
		return -1;
	}

	//关闭/dev/fb0
	if(close(fd_lcd))
	{
		printf("close fd_lcd error！\n");
		return -1;
	}
	return 0;
}

int show_bmp(char * bmp_path)
{
	//打开设备文件/dev/fb0
	int fd_lcd = open("/dev/fb0",O_RDWR);
	if(-1 == fd_lcd)
	{
		printf("open /dev/fb0 error!\n");
		return -1;
	}
	//映射为内存mmap
	unsigned int * buff_lcd = NULL;
	buff_lcd  = (unsigned int *)mmap(NULL,LCD_W * LCD_H * LCD_PIX_SIZE, PROT_WRITE, MAP_SHARED,\
			fd_lcd, 0);
	if(buff_lcd == MAP_FAILED)
	{
		printf("mmap error :errno = %d!\n",errno);
		return -1;
	}

	proc_bmp_800_480(bmp_path, buff_lcd);

	//关闭/dev/fb0
	munmap(buff_lcd,LCD_W * LCD_H * LCD_PIX_SIZE);
	if(close(fd_lcd))
	{
		printf("close fd_lcd error！\n");
		return -1;
	}
	return 0;
}