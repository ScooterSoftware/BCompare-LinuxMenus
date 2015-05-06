# bcompare_ext_konq needs (test on Ubuntu 10.10)
#  sudo apt-get install automoc
#  sudo apt-get install g++
#  sudo apt-get install kdelibs5-dev
#  sudo apt-get install libkonq5-dev

EXT_NAME=bcompare_ext_konq
SOURCES=bcompare_ext_konq.cpp bcompare_ext_konq.h bcompare_ext_konq.moc
OBJECTS=$(filter %.o,$(SOURCES:.cpp=.o))

LIBDIR=/usr/lib

FLAGS_32= -m32
FLAGS_64= -m64

WFLAGS=-Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align \
	-Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security
CXX_FLAGS=$(AFLAGS) $(WFLAGS) -fno-exceptions -fno-check-new -fno-common \
	-Woverloaded-virtual -fno-threadsafe-statics -fvisibility=hidden \
	-fvisibility-inlines-hidden -O2 -g -DNDEBUG -DQT_NO_DEBUG -fPIC \
	-D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
	$(shell pkg-config --cflags QtGui) \
	$(shell pkg-config --cflags QtCore)

#
# NO_HINT_STRINGS define switcht
#
# older kde/qt versions (~qt 4.4 4.5) does not have setHelp
# in the KAction objection.  This define was put in to turn
# off hint strings while we need to compile for older distros
# (KUbuntu 8.10, OpenSuse 11.3).  It is set in the Makefile.
# Once this is not needed the define can be disabled and the
# hint strings will appear.
#

CXX_DEFINES = -DNO_HINT_STRINGS -DMAKE_KDIFF_EXT_LIB -D_BSD_SOURCE -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -DQT_NO_STL -DQT_NO_CAST_TO_ASCII -D_REENTRANT -DKDE_DEPRECATED_WARNINGS

LDFLAGS=-shared $(AFLAGS)

all: ext32 ext64

%.moc: %.h
	moc $< -o $@

%.o: %.cpp
	c++ $(CXX_DEFINES) $(CXX_FLAGS) -c $< -o $@

ext32:
	$(MAKE) AFLAGS=$(FLAGS_32) ARCHB=i386 $(EXT_NAME).so

ext64:
	$(MAKE) AFLAGS=$(FLAGS_64) ARCHB=amd64 $(EXT_NAME).so

$(EXT_NAME).so : $(SOURCES) $(OBJECTS)
	c++ $(LDFLAGS) $(OBJECTS) -o $@
	for name in $(basename $(OBJECTS)) ; do \
		mv $$name.o $$name.$(ARCHB).o ; \
	done
	mv $(EXT_NAME).so $(EXT_NAME).$(ARCHB).so

#  Auto install of both files is not needed as the host machine only needs
#  one file (the one appropriate for the host architecture).
#
install32: all
	mkdir -p $(LIBDIR)/kde4/
	cp $(EXT_NAME).i386.so $(LIBDIR)/kde4/$(EXT_NAME).so

install64: all
	mkdir -p $(LIBDIR)/kde4/
	cp $(EXT_NAME).amd64.so $(LIBDIR)/kde4/$(EXT_NAME).so

clean:
	rm -f *.o *.so *.moc
