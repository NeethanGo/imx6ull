#include <stdlib.h>
#include <unistd.h>

void k(void)
{
    int *x = malloc(8 * sizeof(int));
    x[9] = 0; //数组下标越界
} //内存未释放

int main(void)
{
    k();

    // while (1)
    // {
    //     sleep(1);
    // }

    return 0;
}