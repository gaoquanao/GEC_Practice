#include "ts.h"

int fd;
// 触摸屏的打开
int my_ts_open()
{
	// 打开触摸屏
	fd = open("/dev/event0",O_RDWR);
	if(fd == -1)
	{
		perror("open ts error!\n");
		return -1;
	}
	return 0;
}
// 获取触摸屏x，y坐标的函数
int get_x_y(int *x,int *y)
{
	struct input_event myevent;
	static int count = 0;
	while(1)
	{
		// 读取触摸屏反馈的x，y坐标
		read(fd, &myevent, sizeof(struct input_event));
		// 判断读取的结构体myevent中三个成员
		if(myevent.type == EV_ABS)
		{
			if(myevent.code == ABS_X)
			{
				count++;
				*x = myevent.value; //解引用
			}
				
			if(myevent.code == ABS_Y)
			{
				count++;
				*y = myevent.value;
			}
			if(count == 2)
			{
				count = 0;
				break;
			}
		}
		
	}
	return 0;
}

int my_ts_close()
{
	close(fd);
	return 0;
}

void *get_ts(void  * arg)
{
	printf("into get_ts thread\n");
	struct parameter *myarg;
	myarg = (struct parameter *) arg;
	//1.打开触摸屏设备文件  ts_open()
	struct tsdev *ts = ts_open("/dev/event0", 0);
	struct ts_sample sample;

	//2.配置触摸屏 ts_config()
	ts_config(ts);

	//3.读取触摸屏参数 ts_read()
	while(1)
	{
		ts_read(ts, &sample, 1); 
		myarg->pos_x = sample.x;
		myarg->pos_y = sample.y;
		myarg->press = sample.pressure;
	}
	ts_close(ts);
}