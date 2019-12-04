TARGETS=client server

LDLIBS+= -lselinux

# Test SO_PEERSEC for sockets created by socketpair(2).
ifneq ($(shell ../kvercmp $$(uname -r) 4.18),-1)
	TARGETS += socketpair
endif

all: $(TARGETS)
clean:
	rm -f $(TARGETS)
