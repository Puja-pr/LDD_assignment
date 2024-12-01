
obj-m = pchar_ioctl.o

pchar_ioctl.ko: pchar_ioctl.c
		make -C /lib/modules/$$(uname -r)/build M=$$(pwd) modules
clean:
		make -C /lib/modules/$$(uname -r)/build M=$$(pwd) clean

.PHONY: clean


	
