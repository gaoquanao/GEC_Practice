#include <stdio.h>
#include "websocket.h"
#include "handle_protcol.h"
#include <pthread.h>
#include "asr_sample.h"
#include <unistd.h>
#include "xml_read.h"
#include <string.h>


void *pthread_func(void * arg)
{
	init_websocket_v12();
}

void *pthread_socket_data(void *arg)//收到arm上发来的信息转发到微信端
{
	char msg_buff[8] = {0};
	while(1)
	{
		int ret = 0;
		ret = recv_data((protocol *)msg_buff, 8);//解决：ret判断接收长度，如果为0退出循环
		printf("[socket_data]:%s\n",msg_buff);

		if( -1 != ret )
		{
			host2net((protocol *)msg_buff);//将数据转化为网络字节序
			websocket_send((protocol *)msg_buff,8);
			memset((protocol *)msg_buff,0,sizeof(msg_buff));
		}
		else
		{
			printf("[socket_data]:arm disconnet \n");
			close_data();
			//break;
		}

		sleep(1);
	}
}

int main(int argc, char const *argv[])
{
	pthread_t nopoll_id;
	pthread_create(&nopoll_id,NULL,pthread_func,NULL);
	pthread_t aitalk_id;
	pthread_create(&aitalk_id,NULL,aitalk,NULL);
	pthread_t socket_data_id;
	pthread_create(&socket_data_id,NULL,pthread_socket_data,NULL);


	while(1){
		test_pro();
	}

	return 0;
}

int test_pro(){
	printf("============================================\n \
			1. 温度测试\n \
			2. 湿度测试\n \
			3. 光照测试\n \
			4. MQ_2测试\n \
			5. 字符串测试\n \
			6. 单字符测序\n\
			7. 状态测试（二进制）\n");
	printf("============================================\n");
	fflush(stdin);
	int tmp = 0;

	scanf("%d",&tmp);
	protocol *tmp_pro = create_pro();

	switch(tmp){
		case 1 :{
			putData(tmp_pro,WX_APP,WX_APP_T,0,0,40);
			break;
		}
		case 2 :{
			putData(tmp_pro,WX_APP,WX_APP_H,0,0,50);
			break;
		}
		case 3:{
			putData(tmp_pro,WX_APP,WX_APP_L,0,0,60);
			break;
		}
		case 4:{
			putData(tmp_pro,WX_APP,WX_APP_MQ_2,0,0,0xff);
			break;
		}
		case 5:{
			websocket_send("abcdefyq",8);
			return 0;
			break;
		}
		case 6:{
			websocket_send("A",1);
			return 0;
			break;
		}
		case 7:{
			putData(tmp_pro,WX_APP,WX_APP_FUNC_STATUS,0,0,LCD_OK|TEM_OK|H_OK|L_OK);
			break;
		}
		default:{
			printf("cmd error!\n");
			return -1;
		}
	}


	host2net(tmp_pro);
	websocket_send(tmp_pro,8);

	free_pro(tmp_pro);

	return 0;

}