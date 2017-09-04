#include "mypng.h"

//创建用于解析PNG图片的结构体
fb_t fb;
//初始化fb结构体
if (!fb_initialize(&fb, "/dev/fb0"))
{
	fatal("Can't initialize display - %s", fb.result_string);
}
//显示png的函数原型
//&fb fb结构体的指针
//pos_x 和pos_y为图片起始位置
//最后一个参数是png图片在开发板上的路径
fb_draw_image_png(&fb,  pos_x, pos_y, const char *path);
