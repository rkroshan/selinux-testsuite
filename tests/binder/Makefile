# Required for local building
INCLUDEDIR ?= /usr/include

TARGETS = check_binder client manager service_provider
LDLIBS += -lselinux -lrt
DEPS = binder_common.c binder_common.h

ifeq ($(shell test -e $(INCLUDEDIR)/linux/android/binderfs.h && echo true),true)
CFLAGS += -DHAVE_BINDERFS
TARGETS += check_binderfs
endif

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(TARGETS): $(DEPS)
