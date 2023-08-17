
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./led_app /dev/100ask_led on
 * ./led_app /dev/100ask_led off
 */
int main(int argc, char **argv)
{
	int fd;
	int len;
	char status;
	
	/* 1. 判断参数 */
	if (argc != 3) 
	{
		printf("Usage: %s <dev> <on | off>\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	/* 3. 写文件或读文件 */
	if (0 == strcmp(argv[2], "on")){
		status = 1;
		write(fd, &status, 1);
	}
	else {
		write(fd, &status, 1);		
	}
	
	close(fd);
	
	return 0;
}
