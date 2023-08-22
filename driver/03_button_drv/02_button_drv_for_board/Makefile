
# 1. 使用不同的开发板内核时, 一定要修改KERN_DIR
# 2. KERN_DIR中的内核要事先配置、编译, 为了能编译内核, 要先设置下列环境变量:
# 2.1 ARCH,          比如: export ARCH=arm
# 2.2 CROSS_COMPILE, 比如: export CROSS_COMPILE=arm-buildroot-linux-gnueabihf-
# 2.3 PATH,          比如: export PATH=$PATH:~/workspace/imx6ull/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin
# 注意: 不同的开发板不同的编译器上述3个环境变量不一定相同,
#       请参考各开发板的高级用户使用手册

KERN_DIR = ~/workspace/imx6ull/100ask_imx6ull-sdk/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=`pwd` modules 
	$(CROSS_COMPILE)gcc -Wall -Werror -o button_test button_test.c 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	rm -f button_test

# 参考内核源码drivers/char/ipmi/Makefile
# 要想把a.c, b.c编译成ab.ko, 可以这样指定:
# ab-y := a.o b.o
# obj-m += ab.o

# button_drv.c board_xxx.c 编译成100ask.ko
100ask_button-y := button_drv.o board_imx6ull.o
obj-m	+= 100ask_button.o

