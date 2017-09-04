#include "ts.h"
#include "lcd.h"
#include "rec.h"
#include "main.h"
#include "serial.h"
#include "music.h"
#include "album.h"
#include "socket.h"
#include "mypng.h"
#include "showjpeg.h"
#include "handle_protcol.h"

#define G_MAIN            0
#define G_PICTURE         1
#define G_MUSIC			  2
#define G_REC             3
#define G_TEMP            4

unsigned int temp = 0;
unsigned int humid = 0;

unsigned char T_high = 0;
unsigned char T_last = 0;

unsigned char H_high = 0;
unsigned char H_last = 0;

int serial_fd = 0;
char serial_recv_buf[6] = {0};

unsigned int g_state = 0;
int ts_x = 0;
int ts_y = 0;

char jpegbuf[800*480*4] = {0};
int  jpeglength = 800*480*4;

void showjpg(char *jpgpath){
	lcd_draw_jpg(0,  0,  jpgpath, jpegbuf, jpeglength, 0);
}

void proc_main()
{
	showjpg("./res/main.jpg");
	while(1)
	{
		printf("into main\n");
		get_x_y(&ts_x, &ts_y);
		if((ts_x >= 70) && (ts_x <= 210) && (ts_y >= 170) && (ts_y <= 310))
		{
			printf("into music\n");
			g_state = G_MUSIC;
			ts_x = 0;
			ts_y = 0;
			break;
		}
	
		if((ts_x >= 280) && (ts_x <= 350) && (ts_y >= 170) && (ts_y <= 310))
		{
			printf("into picture\n");
			g_state = G_PICTURE;
			ts_x = 0;
			ts_y = 0;
			break;
		}

		if((ts_x >= 420) && (ts_x <= 550) && (ts_y >= 170) && (ts_y <= 310))
		{
			printf("into rec\n");
			g_state = G_REC;
			ts_x = 0;
			ts_y = 0;
			break;
		}

		if((ts_x >= 590) && (ts_x <= 730) && (ts_y >= 170) && (ts_y <= 310))
		{
			printf("into temp\n");
			g_state = G_TEMP;
			ts_x = 0;
			ts_y = 0;
			break;
		}
	}
}

int proc_picture()
{
	int control_flag = 0; //用于控制中心的出现与消失

	my_list init_list(void)
	{
		my_list head = malloc(sizeof(listnode));
		if( head == NULL )
		{
			perror("malloc node fail\n");
			exit(1);
		}
		head->prev = head->next = head;
		return head;
	}
	
	my_list insert_node(my_list new, my_list p)
	{
		if( (new == NULL) || (p == NULL) )
		{
			perror("trying to insert null node into list!\n");
			exit(1);
		}
		new->prev = p;
		new->next = p->next;
		p->next = new;
		new->next->prev = new;
	}

	int pic_num = 0;
	char tmp_path[256] = {0};

	my_list new = NULL;
	my_list head = init_list();

	DIR * dir = NULL;
	dir = opendir("./picture");
	if (NULL == dir)
	{
		printf("open image_path error \n");
		return -1;
	}

	struct dirent * tmp;
	printf("start read dir\n");
	while((tmp = readdir(dir)) != NULL)
	{
		if( (tmp->d_type == 8) && ( strstr(tmp->d_name,".jpg") != NULL || strstr(tmp->d_name,".bmp") != NULL  || strstr(tmp->d_name,".png") != NULL ) )
		{
			sprintf(tmp_path, "./picture/%s", tmp->d_name);	
			if(pic_num == 0)
			{
				stpcpy(head->path, tmp_path);
				printf("%s\n", head->path);
			}
			else
			{
				new = init_list();
				stpcpy(new->path, tmp_path);
				printf("%s\n", new->path);
				insert_node(new,head);
			}
			pic_num++;
		}
	}

	my_list p = head;
 	//显示链表中的第一张图
	if(strstr(p->path, ".bmp") != NULL)
	{
		show_bmp(p->path);
	}
	else
	{
		showjpg(p->path);
	}

	usleep(200000);

	while(1)
	{
		 get_x_y(&ts_x, &ts_y);
		if(control_flag == 0)
		{
			if(ts_x != 0 && ts_y != 0) //如果触碰屏幕，进入控制中心
			{
				printf("show control center\n");
				lcd_draw_jpg(28, 20, "./res/back_65_65.jpg", jpegbuf, jpeglength, 0);
				lcd_draw_jpg(0, 480-63, "./res/pic_control.jpg", jpegbuf, jpeglength, 0);
				control_flag = 1; //进入控制中心界面
				ts_x = 0;
				ts_y = 0;
				usleep(200000);
			}
		}
		else //处理控制中心界面  退出，上一张，下一张，自动播放
		{
			//退出图片浏览器
			if((ts_x >= 29) && (ts_x <= 92) && (ts_y >= 19) && (ts_y <= 81))
			{
				printf("break out image viewer\n");
				g_state = G_MAIN;
				control_flag = 0;
				ts_x = 0;
				ts_y = 0;
				break;
			}
			//上一张			
			else if ( (ts_y >= 410) &&(ts_x <= 266) && (ts_x >= 0))
			{
				printf("prev picture\n");
				p = p->prev;
				if(strstr(p->path, ".bmp") != NULL)
				{
					show_bmp(p->path);
				}
				else
				{
					showjpg(p->path);
				}
				control_flag = 0;
				ts_x = 0;
				ts_y = 0;
				usleep(200000);
			}
			//循环播放
			else if ( (ts_y >410) && (ts_x < 533) && (ts_x > 266))
			{
				printf("into auto play mode\n");
				ts_x = 0;
				ts_y = 0;

				my_ts_close();

				struct parameter arg;
				arg.pos_x = 0;
				arg.pos_y = 0;
				arg.press = 0;

				pthread_t thread_xy;
				pthread_create(&thread_xy, NULL, get_ts, &(arg));

				while(1)
				{
					// get_x_y(&ts_x, &ts_y);
					//创建返回触摸屏坐标的线程
					if((arg.pos_x != 0) && (arg.pos_y != 0))
					{
						control_flag = 0;
						printf("break out auto play\n");
						pthread_cancel(thread_xy);
						my_ts_open();
						break;
					}
					else
					{
						if(strstr(p->path, ".bmp") != NULL)	{
							show_bmp(p->path);
						}
						else{
							showjpg(p->path);
						}
						sleep(1);
						p = p->next;
					}

				}
			}
			//下一张
			else if ( (ts_y >= 410) && (ts_x < 800) && (ts_x > 533))
			{
				printf("next picture\n");
				p = p->next;
				if(strstr(p->path, ".bmp") != NULL)
				{
					show_bmp(p->path);
				}
				else
				{
					showjpg(p->path);
				}
				control_flag = 0;
				ts_x = 0;
				ts_y = 0;
				usleep(200000);
			}
			else {
				//点击非控制区域，隐藏控制中心
				control_flag = 0;
				printf("exit control center\n");
				showjpg(p->path);
				ts_x = 0;
				ts_y = 0;
				usleep(200000);
			}

		}

	}
}

int proc_music()
{
	fb_t fb;
	if (!fb_initialize(&fb, "/dev/fb0"))
	{
		fatal("Can't initialize display - %s", fb.result_string);
	}

	music_list head = NULL;
	head = find_music();
	showjpg(head->blur_picpath);//模糊的背景
	lcd_draw_jpg(280, 87, head->little_picpath, jpegbuf, jpeglength, 0); //第一张专辑封面

	fb_draw_image_png(&fb, 28, 20,"./res/back_65_65.png"); //返回按钮
	fb_draw_image_png(&fb, 271, 363, "./res/white_prev.png"); //播放控制按钮
	fb_draw_image_png(&fb, 378, 363, "./res/white_pause.png");	
	fb_draw_image_png(&fb, 484, 363, "./res/white_last.png");

	int state_flag = 0;
	int touch_flag = 0;

	char tmp_path[256];

	music_list p = head;

	while(1)
	{
		get_x_y(&ts_x, &ts_y);
		if((ts_x >= 376) && (ts_x <= 422) && (ts_y >= 365) && (ts_y <= 411))
		{
			if(state_flag == 0)
			{
				if(touch_flag == 1)
				{
					showjpg(p->blur_picpath);//模糊的背景
					lcd_draw_jpg(280, 87, p->little_picpath, jpegbuf, jpeglength, 0); //第一张专辑封面
					fb_draw_image_png(&fb, 28, 20,"./res/back_65_65.png"); //返回按钮
					fb_draw_image_png(&fb, 271, 363, "./res/white_prev.png"); //播放控制按钮
					fb_draw_image_png(&fb, 484, 363, "./res/white_last.png");
					fb_draw_image_png(&fb, 378, 363, "./res/white_play.png");
					system("killall -CONT madplay &");
					touch_flag = 0;
				}
				else
				{
					//点击播放
					showjpg(p->blur_picpath);//模糊的背景
					lcd_draw_jpg(280, 87, p->little_picpath, jpegbuf, jpeglength, 0); //第一张专辑封面
					fb_draw_image_png(&fb, 28, 20,"./res/back_65_65.png"); //返回按钮
					fb_draw_image_png(&fb, 271, 363, "./res/white_prev.png"); //播放控制按钮
					fb_draw_image_png(&fb, 484, 363, "./res/white_last.png");
					fb_draw_image_png(&fb, 378, 363, "./res/white_play.png");
					sprintf(tmp_path,"madplay \"%s\" &", p->filepath);

					system(tmp_path);
				}
				state_flag = 1;
			}
			else
			{
				//再次点击暂停
				showjpg(p->blur_picpath);//模糊的背景
				lcd_draw_jpg(280, 87, p->little_picpath, jpegbuf, jpeglength, 0); //第一张专辑封面
				fb_draw_image_png(&fb, 28, 20,"./res/back_65_65.png"); //返回按钮
				fb_draw_image_png(&fb, 271, 363, "./res/white_prev.png"); //播放控制按钮
				fb_draw_image_png(&fb, 484, 363, "./res/white_last.png");
				fb_draw_image_png(&fb, 378, 363, "./res/white_pause.png");	
				system("killall -STOP madplay &");
				state_flag  = 0;
				touch_flag = 1;
			}
			ts_x = 0;ts_y = 0;			
		}
		if((ts_x >= 271) && (ts_x <= 317) && (ts_y >= 365) && (ts_y <= 411))
		{
			//上一首
			system("killall -KILL madplay");
			p = p->prev;
			showjpg(p->blur_picpath); //背景切换
			lcd_draw_jpg(280, 87, p->little_picpath, jpegbuf, jpeglength, 0); //专辑封面
			fb_draw_image_png(&fb, 271, 363, "./res/white_prev.png"); //播放控制按钮
			fb_draw_image_png(&fb, 484, 363, "./res/white_last.png");
			fb_draw_image_png(&fb, 378, 363, "./res/white_play.png");
			fb_draw_image_png(&fb, 28, 20,"./res/back_65_65.png"); //返回按钮
			sprintf(tmp_path,"madplay \"%s\" & ",p->filepath);

			system(tmp_path);
			ts_x = 0;ts_y = 0;
			state_flag = 1;
		}

		if((ts_x >= 484) && (ts_x <= 530) && (ts_y >= 365) && (ts_y <= 411))
		{
			//下一首
			system("killall -KILL madplay &");
			p = p->next;
			showjpg(p->blur_picpath);//背景切换
			lcd_draw_jpg(280, 87, p->little_picpath, jpegbuf, jpeglength, 0); //专辑封面
			fb_draw_image_png(&fb, 271, 363, "./res/white_prev.png"); //播放控制按钮
			fb_draw_image_png(&fb, 484, 363, "./res/white_last.png");
			fb_draw_image_png(&fb, 378, 363, "./res/white_play.png");
			fb_draw_image_png(&fb, 28, 20,"./res/back_65_65.png"); //返回按钮
			sprintf(tmp_path,"madplay \"%s\" & ",p->filepath);

			system(tmp_path);
			ts_x = 0; ts_y = 0;
			state_flag = 1;
		}

		if((ts_x >= 29) && (ts_x <= 92) && (ts_y >= 19) && (ts_y <= 81))
		{
			system("killall -9 madplay &");
			g_state = G_MAIN;
			ts_x = 0;ts_y = 0;
			break;
		}
	}

}

int proc_rec()
{
	printf("we are in rec\n");
	//创建循环播放录音界面的线程
	pthread_t pthread_rec;
	pthread_create(&pthread_rec,NULL,(void *)showjpg_rec,NULL);
	system("arecord -d3 -c1 -r16000 -twav -fS16_LE example.wav");
	
	//socket连接并发送文件   运行socket可执行文件接收  本机IP192.168.200.125
	if (-1 == socket_init())
	{
		printf("we will set main\n");
		g_state = G_MAIN;
		ts_x = 0;
		ts_y = 0;
		pthread_cancel(pthread_rec);			
		return 0;
	}
	send_voice_file("example.wav");

	pthread_cancel(pthread_rec);
	showjpg("./res/voice_success.jpg");
	
	close_voice_socket();

	while(1)
	{
		get_x_y(&ts_x, &ts_y);
		if((ts_x >= 29) && (ts_x <= 92) && (ts_y >= 19) && (ts_y <= 81))
		{
			g_state = G_MAIN;
			ts_x = 0;
			ts_y = 0;	 			
			break;
		}
	}
}

void *update_temp_humid(void *arg)
{
	char filepath[256] = {0};

	while(1)
	{
		sprintf(filepath, "./res/%d.jpg",T_high);
		lcd_draw_jpg(145, 190, filepath, jpegbuf, jpeglength, 0);
		bzero(filepath, sizeof(filepath));
	
		sprintf(filepath, "./res/%d.jpg",T_last);
		lcd_draw_jpg(230, 190, filepath, jpegbuf, jpeglength, 0);
		bzero(filepath, sizeof(filepath));
		
		sprintf(filepath, "./res/purple_%d.jpg",H_high);
		lcd_draw_jpg(485, 190, filepath, jpegbuf, jpeglength, 0);
		bzero(filepath, sizeof(filepath));
	
		sprintf(filepath, "./res/purple_%d.jpg",H_last);
		lcd_draw_jpg(570, 190, filepath, jpegbuf, jpeglength, 0);
		bzero(filepath, sizeof(filepath));
	
		sleep(5);
	}

}

int proc_temp()
{
	showjpg("./res/temp.jpg"); //显示背景图

	pthread_t update;
	pthread_create(&update, NULL, update_temp_humid, NULL);

	while(1)
	{
		get_x_y(&ts_x, &ts_y);
		if((ts_x >= 29) && (ts_x <= 92) && (ts_y >= 19) && (ts_y <= 81))
		{
			printf("break out temp\n");
			g_state = G_MAIN;
			pthread_cancel(update);
			ts_x = 0;
			ts_y = 0;
			break;
		}
	}

}

//收到小程序传回的数据
void *pthread_wx(void * arg)
{
	printf("[ARM] : pthread_wx started!\n");
	char msg_buff[8] = {0};
	int msg_num = 0;
	while(1)
	{
		msg_num = recv_data(msg_buff, 8);
		if (-1 == msg_num)
		{
			break;
		}
		protocol * msg_buff_p = (protocol *)msg_buff;
		printf("Old : Command = %d, value = %d\n", msg_buff_p->command._command, msg_buff_p->value );
		net2host(msg_buff_p);

		// 进行位操作, 获取命令
		if(msg_num != 0)
		{
			printf("New : Command = %d, value = %d\n", msg_buff_p->command._command, msg_buff_p->value );
			switch(msg_buff_p->command.command.cmd)
			{
				case 0:
					//LED
					if(msg_buff_p->value > 0){  // value 的值不是等于1
						open_light(serial_fd);
					}
					else{
						close_light(serial_fd);
					}
					continue;
				case 1:
					continue;
				case 2:
					continue;
			}
			return 0;
		}	
	}

}

void *read_from_serial()
{
	while(1)
	{
		read(serial_fd, serial_recv_buf, 6);
		T_high = serial_recv_buf[1] - '0';
		H_high = serial_recv_buf[4] - '0';
		T_last = serial_recv_buf[2] - '0';
		H_last = serial_recv_buf[5] - '0';

		temp = T_high*10 + T_last;
		humid = H_high*10 +  H_last;
		// printf("%d %d %d %d %d %d\n",T_high, T_last, H_high, H_last, temp, humid);
	}
}

void * send2weixin(void * arg)
{
	protocol * pro = create_pro();
	protocol * temp_pro = create_pro();
	
	while(1)
	{
		putData(pro, WX_APP, WX_APP_H, 0, 0, humid);
		putData(temp_pro, WX_APP, WX_APP_T, 0, 0, temp);
		printf("[send2weixin]:humid %d\n", humid);
		printf("[send2weixin]:temp %d\n", temp);
		//发送湿度数据 5s
		if ( (-1 == send_msg_weixin(pro, 8)) || (1 == send_msg_weixin(temp_pro, 8)) )
		{
			printf("we will quit send to weixin thread\n");
			break;
		}
		sleep(5);
	}
}

int main(int argc, char const *argv[])
{
	//创建线程用以接收从小程序传来的数据并分析
	pthread_t thread_wx;
	pthread_create(&thread_wx, NULL, pthread_wx, NULL);

	//用以每隔5秒向小程序发送数据
	pthread_t thread_wx_test;
	pthread_create(&thread_wx_test, NULL, send2weixin, NULL);

	serial_fd = init_send_serial();
	printf("serial_fd : %d\n", serial_fd);
	
	//创建读取串口数据的线程
	pthread_t thread_serial;
	pthread_create(&thread_serial, NULL, read_from_serial, NULL);

	signal(SIGPIPE, SIG_IGN);

	my_ts_open();

	while(1)
	{
		switch(g_state)
		{
			case G_MAIN:
				proc_main();
				break;
			case G_PICTURE:
				proc_picture();
				break;
			case G_MUSIC:
				proc_music();
				break;
			case G_REC:
				proc_rec();
				break;
			case G_TEMP:
				proc_temp();
				break;
		}
	}
	return 0;
}

