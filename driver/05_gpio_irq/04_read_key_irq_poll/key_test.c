#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>


// 开启内核printk日志打印：echo "7  4  1  7" > /proc/sys/kernel/printk

int main(int argc, char **argv)
{
    int fd;
    int val,ret;
    struct pollfd fds[1];

    if(argc != 2){
        printf("Usage: %s <dev>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR);
    if(fd == -1){
        printf("open %s failed!\n", argv[1]);
        return -1;
    }

    fds[0].fd = fd;
    fds[0].events = POLLIN;

    while(1){
        ret = poll(fds, 1, 2000);
        if(ret == 1 && fds[0].revents&POLLIN){
            read(fd, &val, 4);
            printf("get key: 0x%x\n", val);
        } else {
            printf("get key timeout\n");
        }
    }
    
    close(fd);
    return 0;
}