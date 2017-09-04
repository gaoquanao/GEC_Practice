#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>

int init_send_serial();
int close_send_serial(int serial_fd);

#endif