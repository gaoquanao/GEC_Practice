#include "socket.h"

/*=========语音文件上传，返回识别结果===========*/

static int socked_fd = -1;
static int con_fd = -1;

int voice_socket_init()
{
	printf("AI talk socket started!\n");
	socked_fd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port =  htons(23333);
	addr.sin_addr.s_addr = INADDR_ANY;

	bind(socked_fd, (struct sockaddr *) &addr,sizeof(struct sockaddr));
	
	listen(socked_fd,5);

	int on = 1; 
	int ret = setsockopt( socked_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ); 
	if (-1 == ret)
	{
		printf("[voice_socket_init]:set port reuse failed!\n");
	}
	
	return 0;
}

int voice_wait_connect()
{
	if(socked_fd == -1){
		voice_socket_init();
	}
	struct sockaddr addr_recv;
	socklen_t addrlen = 0;
	printf("[linux voice]:wait ARM connect......\n");
	con_fd =  accept(socked_fd,&addr_recv,&addrlen);
	printf("client connect fd : %d\n", con_fd);
	return 0;
}

int recv_file(char * file_path)
{
	if(con_fd == -1){
		voice_wait_connect();
	}

	//6. 收文件的大小
	char msg_buff[2048] = {0};
	ssize_t msg_len = 0;
	msg_len = recv(con_fd, msg_buff, 2048, 0);

	//7. 转换收到的文件大小为整型
	int file_size = 0;
	sscanf(msg_buff,"%d", &file_size);
	printf("file_size = %d\n", file_size);

	//8. 开始接收文件内容
	FILE *file_p = fopen(file_path,"w+");

	bzero(msg_buff,sizeof(msg_buff));
	int total_len = 0;
	while((msg_len = recv(con_fd,msg_buff,2048,0)) > 0)
	{
		
		fwrite(msg_buff, sizeof(char),msg_len,file_p);
		total_len += msg_len;
		printf("recv:%d/%d\n", total_len, file_size);
		if(total_len == file_size)
		{
			break;
		}
			
		bzero(msg_buff, sizeof(msg_buff));
		msg_len = 0;
	}

	fclose(file_p);
	return 0;
}

int close_voice()
{
	close(con_fd);
	close(socked_fd);
	printf("[linux]:close voice socket", con_fd);
	socked_fd = -1;
	con_fd = -1;
	return 0;
}


/*===============数据转发===============*/
static int data_fd = -1;
static int con_data_fd = -1;

int data_socket_init()
{
	data_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port =  htons(24444);
	addr.sin_addr.s_addr =INADDR_ANY;

	bind(data_fd, (struct sockaddr *) &addr,sizeof(struct sockaddr));
	
	listen(data_fd,5);

	int on = 1; 
	int ret = setsockopt( data_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ); 
	if (-1 == ret)
	{
		printf("[data_socket_init]:set port reuse failed!\n");
	}

	return 0;
}

int data_wait_connect()
{
	if(data_fd == -1)
	{
		data_socket_init();
	}

	struct sockaddr addr_recv;
	socklen_t addrlen = 0;

	printf("[data_socket] : wait arm connect\n");
	con_data_fd = accept(data_fd,&addr_recv,&addrlen);

	printf("[data_socket] : ARM connected  fd=%d\n", con_data_fd);				

	return 0;
}

int recv_data(protocol * msg_buff, unsigned int msg_len){
	if(con_data_fd == -1)
	{
		data_wait_connect();
	}

	ssize_t recv_len = 0;
	recv_len = recv(con_data_fd, msg_buff, msg_len, 0);
	printf("[data_socket]: recv %s\n", (char*)msg_buff);//可能越界

	if(!recv_len)
	{
		return -1;
	}

	return 0;
}

int send_voice_data(protocol * msg_buff, unsigned int msg_len)
{
	if(con_data_fd == -1)
	{
		data_wait_connect();
	}

	int send_len = 0;
	send_len = send(con_data_fd, msg_buff, msg_len, 0);
	printf("[data_socket]: send %o\n", msg_buff);

	if(!send_len)
	{
		return -1;
	}

	return 0;
}

int send_data(char * data, unsigned int len)
{
	if(con_data_fd == -1){
		data_wait_connect();
	}
	send(con_data_fd, data, len, 0);
	return 0;
}

int close_data()
{
	close(con_data_fd);
	close(data_fd);
	data_fd = -1;
	con_data_fd = -1;
	return 0;
}
