obj-m += slab_ex.o

all:
	make -C /lib/modules/6.1.0-13-amd64/build M=$(PWD) modules

clean:
	make -C /lib/modules/6.1.0-13-amd64/build M=$(PWD) clean
