obj-m = setestsuite_module.o

TARGETS = finit_load init_load
LDLIBS += -lselinux
KDIR = /usr/src/kernels/$(shell uname -r)

all: $(TARGETS)
	$(MAKE) -C $(KDIR) M=$(PWD)

clean:
	rm -f $(TARGETS)
	rm -f *.o *.ko *.cmd *.mod *.mod.c .*.cmd Module.symvers modules.order
