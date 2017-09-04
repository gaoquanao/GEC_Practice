#include "showjpeg.h"

static int  g_fb_fd;
static int *g_pfb_memory;
static char g_color_buf[FB_SIZE]={0};

void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(g_pfb_memory+y*800+x)=color;
}

unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	return filesize;
}


int lcd_draw_jpg(unsigned int x, unsigned int y,  const char *pjpg_path, char *pjpg_buf, unsigned int jpg_buf_size, unsigned int jpg_half)  
{
	//初始化LCD
	g_fb_fd = open("/dev/fb0", O_RDWR);
	
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	g_pfb_memory  = (int *)mmap(	NULL, 					//映射区的开始地址，设置为NULL时表示由系统决定映射区的起始地址
									FB_SIZE, 				//映射区的长度
									PROT_READ|PROT_WRITE, 	//内容可以被读取和写入
									MAP_SHARED,				//共享内存
									g_fb_fd, 				//有效的文件描述词
									0						//被映射对象内容的起点
								);
	
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo; // 解码
	struct 	jpeg_error_mgr 			jerr;	// 错误处理
	
	char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i = 0;
	unsigned int	color = 0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
	int	  jpg_fd;
	unsigned int 	jpg_size;
		
	if(pjpg_path != NULL)
	{
		/* 申请jpg资源，权限可读可写 */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* 获取jpg文件的大小 */
		jpg_size = file_size_get(pjpg_path);	

		/* 为jpg文件申请内存空间 */	
		pjpg = malloc(jpg_size);

		/* 读取jpg文件所有内容到内存 */		
		read(jpg_fd, pjpg, jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		pjpg = pjpg_buf;
	}
    // 以下5句代码是用来做jpg图片解码
	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo, pjpg, jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	

	unsigned int 	jpg_width = cinfo.image_width;
	unsigned int 	jpg_height = cinfo.image_height;


	if(jpg_half) //1
	{
		x_e	= x_s+(cinfo.output_width/2);
		y_e	= y  +(cinfo.output_height/2);		
		
		/*读解码数据*/
		while(cinfo.output_scanline < cinfo.output_height)
		{		
			pcolor_buf = g_color_buf;
			
			/* 读取jpg一行的rgb值 */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);			
			
			/* 再读取jpg一行的rgb值 */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);

			for(i=0; i<(cinfo.output_width/2); i++)
			{
				/* 获取rgb值 */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* 显示像素点 */
				lcd_draw_point(x,y,color);
				
				pcolor_buf +=6;
				
				x++;
			}
			/* 换行 */
			y++;					
			x = x_s;
		}
	}
	else
	{
		x_e	= x_s+cinfo.output_width;
		y_e	= y  +cinfo.output_height;	

		/*读解码数据*/
		while(cinfo.output_scanline < cinfo.output_height )
		{		
			pcolor_buf = g_color_buf;
			
			/* 读取jpg一行的rgb值 */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
			
			for( i = 0 ; i < cinfo.output_width ;  i++)
			{
				/* 获取rgb值 */
				color = 	*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* 显示像素点 */
				lcd_draw_point(x, y, color);
				pcolor_buf +=3;
				x++;
			}
			/* 换行 */
			y++;			
			x = x_s;
		}		
	}
	
	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* 关闭jpg文件 */
		close(jpg_fd);	
		/* 释放jpg文件内存空间 */
		free(pjpg);		
	}
	//LCD关闭
	/* 取消内存映射 */
	munmap(g_pfb_memory, FB_SIZE);
	
	/* 关闭LCD设备 */
	// printf("weight: %d, height: %d\n", jpg_width, jpg_height);
	close(g_fb_fd);
	return 0;
}
