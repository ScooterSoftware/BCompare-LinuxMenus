# bcompare-ext-thunarx needs (tested on Ubuntu 10.10)
#   sudo apt-get install libglib2.0-dev
#   sudo apt-get install libthunar-vfs-1-dev
#   sudo apt-get install libncurses5-dev
#   sudo apt-get install libthunarx-3-dev

EXT_NAME=bcompare-ext-thunarx
SOURCES=bcompare-thunarx.c
OBJECTS=$(filter %.o,$(SOURCES:.c=.o))

LIBDIR=/usr/lib

FLAGS_32= -m32
FLAGS_64= -m64
WFLAGS=-Wall -Wmissing-prototypes
CFLAGS= $(WFLAGS) $(AFLAGS) -fPIC -g \
	$(shell pkg-config --cflags glib-2.0) \
	$(shell pkg-config --cflags thunarx-3)
LDFLAGS=-shared $(AFLAGS)

all: ext32 ext64

ext32:
	$(MAKE) AFLAGS=$(FLAGS_32) ARCHB=i386 $(EXT_NAME).so

ext64:
	$(MAKE) AFLAGS=$(FLAGS_64) ARCHB=amd64 $(EXT_NAME).so

$(EXT_NAME).so : $(OBJECTS)
	gcc $(LDFLAGS) $(OBJECTS) -o $@
	for name in $(basename $(OBJECTS)) ; do \
		mv $$name.o $$name.$(ARCHB).o ; \
	done
	mv $(EXT_NAME).so $(EXT_NAME).$(ARCHB).so

install32: all
	mkdir -p $(LIBDIR)/thunarx-3/
	cp $(EXT_NAME).i386.so $(LIBDIR)/thunarx-3/

install64: all
	mkdir -p $(LIBDIR)/thunarx-3/
	cp $(EXT_NAME).amd64.so $(LIBDIR)/thunarx-3/
clean:
	rm -f *.o *.so
