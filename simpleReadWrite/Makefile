obj-m := kjDeviceModule.o

all:
	$(MAKE) -C /usr/src/linux-headers-5.3.0-46-generic/ M=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod* *.symvers *.order *.out *.kj*
