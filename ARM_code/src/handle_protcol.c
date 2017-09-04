#include "handle_protcol.h"

//创建一个协议节点
protocol * create_pro()
{
	return malloc(sizeof(protocol));
}

//写入协议内容
int putData(protocol * pro,char dst,char cmd,char ui,char reserve,unsigned value)
{
	pro->command.command.dst = dst;
	pro->command.command.cmd = cmd;
	pro->command.command.ui = ui;
	pro->command.command.reserve = reserve;

	pro->value = value;
	return 0;
}

//释放协议节点
void free_pro(protocol * pro)
{
	return free(pro);
}

//转换为网络字节序
int host2net(protocol * pro)
{
	if(is_bigend()){
		return 0;
	}else{
		pro->command._command = htonl(pro->command._command);
		pro->value = htonl(pro->value);
		return 0;
	}
}

int is_bigend(){
	unsigned int a = 0x1;
	return (a>>1)?1:0;
}


//解析消息中的字段（包含网络序与主机序的转换）
int getData(protocol * pro,char * msg){
	char * tmp = msg;
	pro->command._command = ntohl(*(unsigned long int *)tmp);
	tmp += 4;
	pro->value = ntohl(*(unsigned long int *)tmp);
}

