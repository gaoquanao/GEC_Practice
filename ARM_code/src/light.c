#include "light.h"


int open_light(int serial_fd)
{
    char buf[3] = "11";
    printf("[pthread_wx]: open light\n");
    if(write(serial_fd, buf, 3) == -1)
    {
        printf("write to serial failed!\n");
    }

    printf("[open light] serial_fd: %d, buf:%s\n", serial_fd, buf );
}

int close_light(int serial_fd)
{
    char buf[3] =  "00";
    printf("[pthread_wx]: close light\n");
    if(write(serial_fd, buf, 3) == -1)
    {
        printf("write to serial failed!\n");
    }
    printf("[close light] serial_fd: %d, buf:%s\n", serial_fd, buf );
}