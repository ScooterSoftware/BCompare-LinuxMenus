# bcompare-ext-nemo needs (tested on Ubuntu 10.10)
#  sudo apt-get install libglib2.0-dev
#  sudo apt-get install libncurses5-dev
#  sudo apt-get install libnemo-extension-dev

EXT_NAME=bcompare-ext-nemo
SOURCES=bcompare-nemo.c
OBJECTS=$(filter %.o,$(SOURCES:.c=.o))

LIBDIR=/usr/lib

FLAGS_32= -m32
FLAGS_64= -m64
WFLAGS=-Wall -Wmissing-prototypes
CFLAGS= $(WFLAGS) $(AFLAGS) -fPIC -g \
	$(shell pkg-config --cflags glib-2.0) \
	$(shell pkg-config --cflags libnemo-extension)
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
	mkdir -p $(LIBDIR)/nemo/extensions-3.0
	cp $(EXT_NAME).i386.so $(LIBDIR)/nemo/extensions-3.0/

install64: all
	mkdir -p $(LIBDIR)/nemo/extensions-3.0
	cp $(EXT_NAME).amd64.so $(LIBDIR)/nemo/extensions-3.0/

clean:
	rm -f *.o *.so
