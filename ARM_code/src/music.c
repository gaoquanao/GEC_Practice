#include "music.h"

music_list music_init_list(void)
{
	music_list head = malloc(sizeof(music_listnode));
	if( head == NULL )
	{
		perror("malloc node fail\n");
		exit(1);
	}
	head->prev = head->next = head;
	return head;
}

music_list insert_music_node(music_list new, music_list p)
{
	if( (new == NULL) || (p == NULL) )
	{
		perror("tring to insert null node into list!\n");
		exit(1);
	}
	new->next = p;
	new->prev = p->prev;
	p->prev->next = new;
	p->prev = new;
}

//需要定义music_list 来接收
music_list find_music()
{
	int mp3_num = 0;

	DIR *dir = NULL;
	struct dirent *tmp;
	music_list new = NULL;
	music_list head = music_init_list(); //头结点

	char tmp_path[256] = {0};
	char tmp_blurpicpath[256] = {0};//模糊图片路径预设
	char tmp_littlepicpath[256] = {0};//小图片路径预设

	dir = opendir("./music");
	if(NULL == dir)
	{
		perror("opendir fail!\n");
		exit(1);
	}

	while((tmp = readdir(dir)) != NULL)
	{
		if( (tmp->d_type == 8) && ( strstr(tmp->d_name, ".mp3") != NULL ) )
		{
			char *filename;
			filename = tmp->d_name;
			sprintf(tmp_path,"./music/%s", filename);
			char *last_dot = strrchr(filename, '.');
			*last_dot = '\0'; //修改.为‘\0’以获取文件名前缀
			sprintf(tmp_littlepicpath, "./music/%s_240.jpg", filename);
			sprintf(tmp_blurpicpath, "./music/%s_bg.jpg", filename);

			if(mp3_num == 0)
			{
				stpcpy(head->filepath, tmp_path);
				stpcpy(head->little_picpath, tmp_littlepicpath);
				stpcpy(head->blur_picpath, tmp_blurpicpath);
				printf("%s %s %s\n",head->filepath, head->little_picpath, head->blur_picpath);
			}
			else
			{
				new = music_init_list();
				stpcpy(new->filepath, tmp_path);
				stpcpy(new->little_picpath, tmp_littlepicpath);
				stpcpy(new->blur_picpath, tmp_blurpicpath);
				printf("%s %s %s\n",new->filepath, new->little_picpath, new->blur_picpath);
				insert_music_node(new, head);				
			}
			mp3_num++;
		}
	}
	return head;
}
