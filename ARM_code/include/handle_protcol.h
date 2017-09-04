#ifndef __HANDLE_PROTCOL_H__
#define __HANDLE_PROTCOL_H__

#include <stdio.h>
#include <stdlib.h>
typedef struct _protocol
{
	union 
	{
		unsigned int _command;
		struct
		{
			char reserve;// 本机为小端，所以这个存在低地址
			char ui;
			char cmd;
			char dst;
			
		} command;

	}command;

	unsigned int value;

} protocol;

//目的字段
enum _Dst
{
	WX_APP, //微信小程序
	BOARD    //开发板
};

//发给微信小程序的命令字段
enum _Cmd_WX 
{
	WX_APP_T,//温度
	WX_APP_H,//湿度
	WX_APP_L,//光照
	WX_APP_MQ_2,//MQ_2
	WX_APP_FUNC_STATUS

};

//发给单板的命令字段
enum _Cmd_BOARD 
{
	BOARD_LED,//LED
	BOARD_WIN,//窗户
	BOARD_UI_CONTRL//界面控制
};

enum _Vaule_Status
{
	LCD_OK = 0x01,//lcd，状态ok
	TEM_OK = 0X02,//温度状态ok
	H_OK = 0x04,//湿度状态ok
	L_OK = 0x08//光照状态ok
};


#define ON 1
#define OFF 0


// int protocol_test(protocol * pro);
protocol * create_pro();
int putData(protocol * pro,char dst,char cmd,char ui,char reserve,unsigned value);
void free_pro(protocol * pro);
int is_bigend();
int host2net(protocol * pro);

int getData(protocol * pro,char * msg);
#endif