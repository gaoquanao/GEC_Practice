#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdlib.h>
#include "handle_protcol.h"


int voice_socket_init();
int voice_wait_connect();
int recv_file(char * file_path);
int close_voice();


int data_socket_init();
int data_wait_connect();
int recv_data(protocol * msg_buff,unsigned int msg_len);
int send_voice_data(protocol * msg_buff, unsigned int msg_len);
int send_data(char * data,unsigned int len);
int close_data();

#endif