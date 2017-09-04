#include "bmp.h"

#define BMP_W_800 800
#define BMP_H_480 480
#define BMP_PIX_SIZE 3
//bmp 解析相关的代码
#define BMP_H_INFO 54

int proc_bmp_800_480(char *bmp_path,unsigned int * lcd_buff)
{

	//打开图片
	int fd_bmp = open(bmp_path,O_RDONLY);
	if(-1 == fd_bmp)
	{
		printf("open %s error\n",bmp_path);
		return -1;
	}

	//偏移过文件头
	int ret = lseek(fd_bmp,54,SEEK_SET);
	if(-1 == ret)
	{
		printf("lseek error!\n");
		return -1;
	}

	//读数据
	char buff[BMP_W_800 * BMP_H_480 * BMP_PIX_SIZE] = {0};
	read(fd_bmp, buff, BMP_W_800 * BMP_H_480 * BMP_PIX_SIZE);

	//RGB信息 转换为 ARGB信息
	int i = 0;
	int j = 0;
	for(i = 0 ;i < BMP_H_480; i++) 
	{
		for(j = 0 ;j < BMP_W_800; j++)
		{
			lcd_buff[(BMP_H_480 - 1 - i)*BMP_W_800 + j] = 0x0 <<24 |buff[(i*BMP_W_800+j)*BMP_PIX_SIZE] << 0 | \
										buff[(i*BMP_W_800+j)*BMP_PIX_SIZE + 1] <<8 | \
										buff[(i*BMP_W_800+j)*BMP_PIX_SIZE + 2] << 16;
		}
	}

	close(fd_bmp);

	return 0;
}


int get_bmp_w_h(char * bmp_info_buff,unsigned int *w,unsigned int *h){

	if(bmp_info_buff == NULL || w == NULL || h == NULL){
		printf("pos NULL\n");
		return -1;
	}
	unsigned int _bmp_w = *(unsigned int *)(bmp_info_buff + 18);   //18 偏移到图片的宽度信息的偏移量
	unsigned int _bmp_h = *(unsigned int *)(bmp_info_buff + 18 + 4); //4 偏移过bmp的宽度信息
	
	printf("get_bmp_w_h: w = %d, h = %d\n",_bmp_w, _bmp_h);

	*w = _bmp_w;
	*h = _bmp_h;

	return 0;
}

int proc_bmp_not_800_480(char *bmp_path,unsigned int * lcd_buff,unsigned int x,unsigned int y)
{
	int fd_bmp = open(bmp_path,O_RDONLY);
	if(-1 == fd_bmp)
	{
		printf("open %s error\n",bmp_path);
		return -1;
	}

	//获取图片的长宽
	char bmp_info_buff[BMP_H_INFO] = {0};
	read(fd_bmp, bmp_info_buff, BMP_H_INFO);
	
	unsigned int bmp_w = 0; 
	unsigned int bmp_h = 0;
	get_bmp_w_h(bmp_info_buff, &bmp_w, &bmp_h);

	int ret = lseek(fd_bmp, 54, SEEK_SET);
	if(-1 == ret)
	{
		printf("lseek error!\n");
		return -1;
	}

	char buff[BMP_W_800 * BMP_H_480 * BMP_PIX_SIZE] = {0};
	read(fd_bmp, buff, bmp_w * bmp_h * BMP_PIX_SIZE);

	unsigned int _bmp_w = 0;
	unsigned int _bmp_h = 0;
	if(bmp_h + y > BMP_H_480){
		_bmp_h = BMP_H_480 - y;
	}

	if(bmp_w + x > BMP_W_800){
		_bmp_w = BMP_W_800 - x;
	}

	int i = 0;
	int j = 0;
	for(i = 0 ;i < _bmp_h; i++) 
	{
		for(j = 0 ;j < _bmp_w; j++)
		{
			lcd_buff[(i+y)*BMP_W_800 + (j+x)] = 0x0 <<24 |buff[(i*bmp_w+j)*BMP_PIX_SIZE] << 0 | \
										buff[(i*bmp_w+j)*BMP_PIX_SIZE + 1] <<8 | \
										buff[(i*bmp_w+j)*BMP_PIX_SIZE + 2] << 16;
		}
	}
	close(fd_bmp);

	return 0;
}


int find_bmp(char * bmp_path)
{
	DIR * dirp;
	dirp = opendir(bmp_path);
	if(NULL == dirp)
	{
		printf("open %s error\n",bmp_path);
		return -1;
	}

	struct dirent *_dir_p;
	while(1)
	{
	    _dir_p = readdir(dirp);
	    if(_dir_p == NULL)
	    {
	    	closedir(dirp);
	    	printf("_dir_p == NULL!\n");
	    	return 0;
	    }
	    printf("d_name = %s\n",_dir_p->d_name);
	}
}