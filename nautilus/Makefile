# Makefile for version nautilus/extensions-4
# bcompare-ext-nautilus needs (tested on Ubuntu 10.10)
#  sudo apt-get install libglib2.0-dev
#  sudo apt-get install libncurses5-dev
#  sudo apt-get install libnautilus-extension-dev

EXT_NAME=bcompare-ext-nautilus
SOURCES=bcompare-nautilus.c
OBJECTS=$(filter %.o,$(SOURCES:.c=.o))

LIBDIR=/usr/lib

FLAGS_32= -m32
FLAGS_64= -m64
WFLAGS=-Wall -Wmissing-prototypes
CFLAGS= $(WFLAGS) $(AFLAGS) -fPIC -g \
	$(shell pkg-config --cflags glib-2.0) \
	$(shell pkg-config --cflags libnautilus-extension-4) \
	$(shell pkg-config --cflags gtk4)
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

#  Auto install of both files is not needed as the host machine only needs
#  one file (the one appropriate for the host architecture).
#
install32: all
	mkdir -p $(LIBDIR)/nautilus/extensions-2.0
	cp $(EXT_NAME).i386.so $(LIBDIR)/nautilus/extensions-2.0/

install64: all
	mkdir -p $(LIBDIR)/nautilus/extensions-2.0
	cp $(EXT_NAME).amd64.so $(LIBDIR)/nautilus/extensions-2.0/

clean:
	rm -f *.o *.so
