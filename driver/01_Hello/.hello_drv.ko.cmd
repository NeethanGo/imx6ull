cmd_/workspace/testDriver/01_Hello/hello_drv.ko := arm-buildroot-linux-gnueabihf-ld -EL -r  -T ./scripts/module-common.lds --build-id  -o /workspace/testDriver/01_Hello/hello_drv.ko /workspace/testDriver/01_Hello/hello_drv.o /workspace/testDriver/01_Hello/hello_drv.mod.o ;  true
