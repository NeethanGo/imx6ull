#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>


// 开启内核printk日志打印：echo "7  4  1  7" > /proc/sys/kernel/printk

int fd = 0;

static void sig_handler(int sig)
{
     int val = 0;

    if(fd > 0){
        read(fd, &val, 4);
        printf("get key: 0x%x\n", val);
    }
}

       

int main(int argc, char **argv)
{
    int flags;

    if(argc != 2){
        printf("Usage: %s <dev>\n", argv[0]);
        return -1;
    }

    signal(SIGIO, sig_handler);

    fd = open(argv[1], O_RDWR);
    if(fd == -1){
        printf("open %s failed!\n", argv[1]);
        return -1;
    }

    // 将文件描述符 fd 配置为支持异步通知，
    // 并且当与该文件描述符相关的事件发生时，通知当前进程。
    fcntl(fd, F_SETOWN, getpid());
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags|FASYNC);

    while(1){

        printf("waitting press key...\n");
        sleep(2);
        
    }
    
    close(fd);
    return 0;
}