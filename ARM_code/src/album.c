#include "album.h"

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
	new->next = p;
	new->prev = p->prev;
	p->prev->next = new;
	p->prev = new;
}


int album()
{
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
		if( (tmp->d_type == 8) && ( strstr(tmp->d_name,".jpg") != NULL || strstr(tmp->d_name,".bmp") != NULL ))
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
	while(1)
	{
		if(strstr(p->path, ".bmp") != NULL)
		{
			show_bmp(p->path);
		}
		else
		{
			showjpg(p->path);
		}
		sleep(1);
		p = p->next;
	}
}
