#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>

#include "handle_protcol.h"

#define SERVICE_IP "192.168.200.125"

int socket_init();
int send_voice_file(char *file_path);
int close_voice_socket();

int net2host(protocol * pro);
int wx_socket_init();
int send_msg_weixin(protocol * data,unsigned int len);
int recv_data(char * msg_buff,unsigned int buff_len);
int close_all_wx();

#endif