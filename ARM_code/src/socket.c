#include "socket.h"

/*发送语音文件给服务器*/
static int socke_fd = -1;
int socket_init()
{
	socke_fd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port =  htons(23333);
	addr.sin_addr.s_addr = inet_addr(SERVICE_IP);

	printf("[ARM] : start to connect AI talk at 23333 : %s \n", SERVICE_IP);

	int ret = connect(socke_fd, (struct sockaddr *) &addr,sizeof(struct sockaddr));
	if(-1 == ret){
		printf("[ARM] :  connect to AI talk failed!\n");
		close_voice_socket();
		return -1;
	}
	else
	{
		printf("[ARM] :  connected to AI talk at 23333\n");
	}

}

//发送文件
int send_voice_file(char *file_path)
{
	FILE *file_p = fopen(file_path,"r");
	if(file_p ==  NULL){
		printf("open example.wav failed!\n");
	}

	int file_size = 0;
	fseek(file_p, 0, SEEK_END);//将文件指针偏移到文件末尾
	file_size = ftell(file_p);//获取当前的偏移量（偏移量就是文件大小）
	fseek(file_p, 0, SEEK_SET);//将文件指针偏移到文件开头

	char send_buff[2048] = {0};
	int len = 0;
	len = send(socke_fd, send_buff, 2048, 0);

	bzero(send_buff,sizeof(send_buff));
	int total = 0;
	while((len = fread(send_buff, sizeof(char),sizeof(send_buff),file_p)) > 0)
	{
		int ret = send(socke_fd, send_buff, len, 0);
		total += ret;
		printf("[ARM] : send : %d /%d /%d\n", len, total, file_size);
		bzero(send_buff,sizeof(send_buff));
	}

	fclose(file_p);
	return 0;

}

//关闭socket
int close_voice_socket()
{
	close(socke_fd);
	socke_fd = -1;
	return 0;
}


/*接收和发送数据给小程序的socket*/
static int wx_socket_fd= -1;

int wx_socket_init()
{
	wx_socket_fd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port =  htons(24444);
	addr.sin_addr.s_addr = inet_addr(SERVICE_IP);

	printf("[ARM] : start to connect at 24444 %s \n", SERVICE_IP);
	int ret = connect(wx_socket_fd, (struct sockaddr *) &addr,sizeof(struct sockaddr));
	if(-1 == ret)
	{
		printf("[ARM] : connect 24444 failed!\n");
		close_all_wx();
		return -1;
	}
	else
	{
		printf("[ARM] : connected to 24444 \n");
	}
}

int send_msg_weixin(protocol * data,unsigned int len){
	if(wx_socket_fd == -1){
		if (-1 == wx_socket_init())
		{
			return -1;
		}
	}
	send(wx_socket_fd, data, len, 0);
	return 0;
}

int net2host(protocol * pro)
{  
	pro->command._command = ntohl(pro->command._command);
	pro->value = ntohl(pro->value);
}  


int recv_data(char * msg_buff, unsigned int buff_len)
{
	if(wx_socket_fd == -1){
		if (-1 == wx_socket_init())
		{
			return -1;
		}
	}

	int msg_len = 0;
	msg_len = recv(wx_socket_fd, msg_buff, buff_len, 0);
	if (msg_len !=0)
	{
		printf("[ARM] : recv %o from wechat\n", msg_buff);
	}
	return msg_len;
}

int close_all_wx()
{
	close(wx_socket_fd);
	wx_socket_fd = -1;
	return 0;
}