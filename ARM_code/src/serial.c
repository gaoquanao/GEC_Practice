#include "serial.h"

struct termios termios_old;

int init_send_serial()
{
	//1. 打开串口设备
	int serial_fd = open("/dev/s3c2410_serial3",O_RDWR);
	
	if(serial_fd == -1){
		printf("main:open serial error!\n");
		return -1;
	}
	//2.设置串口
	
	//获取串口属性
	struct termios termios_current;//为了在原来的基础上改变串口的设置（属性）

	tcgetattr(serial_fd, &termios_old);
	tcgetattr(serial_fd, &termios_current);


	//设置位raw模式
	cfmakeraw(&termios_current);
	//设置波特率
	cfsetspeed(&termios_current,B115200);

	//设置bit位 8位
	termios_current.c_cflag &= ~CSIZE;//11 0000
	termios_current.c_cflag |= CS8; //11 0000

	//设置停止位
	termios_current.c_cflag &= ~CSTOPB;//

	//关闭奇偶校验
	termios_current.c_cflag &= ~PARENB;

	termios_current.c_cflag &= ~CRTSCTS;

	tcflush(serial_fd,TCIOFLUSH);
	
	termios_current.c_cc[VTIME] = 0;
   	termios_current.c_cc[VMIN] = 1;

    tcsetattr(serial_fd,TCSANOW,&termios_current);

	return serial_fd;
}

int close_send_serial(int serial_fd)
{
	tcsetattr(serial_fd, TCSANOW, &termios_old);
	close(serial_fd);
	return 0;
}