#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>


// 开启内核printk日志打印：echo "7  4  1  7" > /proc/sys/kernel/printk
 

int main(int argc, char **argv)
{
    int flags, i;
    int val = 0;
    int fd = 0;

    if(argc != 2){
        printf("Usage: %s <dev>\n", argv[0]);
        return -1;
    }

    // 以非阻塞方式打开
    fd = open(argv[1], O_RDWR | O_NONBLOCK);
    if(fd == -1){
        printf("open %s failed!\n", argv[1]);
        return -1;
    }
    for(i=0; i<10; i++){
        if(read(fd, &val, 4) == 4){
            printf("get key: 0x%x\n", val);
        } else {
            printf("warning, get key: -1\n");
        }
    }

    // 以阻塞方式打开
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    while(1){
        if(read(fd, &val, 4) == 4){
            printf("while get key: 0x%x\n", val);
        } else {
            printf("while warning, get key: -1\n");
        }
         
    }
    
    close(fd);
    return 0;
}