#include "rec.h"
#include "showjpeg.h"

static char jpegbuf[800*480*4] = {0};
static int  jpeglength = 800*480*4;

void showjpg_rec()
{
	char* pic_path[3] = {"./res/voice_1.jpg","./res/voice_2.jpg","./res/voice_3.jpg"};
	int i = 0;
	while(1)
	{
		if(3 == i)
		{
			i = 0;
		}
		lcd_draw_jpg(0,  0, pic_path[i], jpegbuf, jpeglength, 0);
		pthread_testcancel();
		sleep(1);
		i++;	
	}
}
