# bcompare-ext-nautilus bcompare-ext-thunarx bcompare_ext_konq need 
#(tested on Ubuntu 10.10)
#  sudo apt-get install libglib2.0-dev
#  sudo apt-get install libncurses5-dev
#  sudo apt-get install libnautilus-extension-dev
#  sudo apt-get install libthunar-vfs-1-dev
#  sudo apt-get install libthunarx-2-dev
#  sudo apt-get install automoc
#  sudo apt-get install g++
#  sudo apt-get install kdelibs5-dev
#  sudo apt-get install libkonq5-dev

# To compile 32 & 64 the following are needed
#  sudo apt-get g++-multilib
#  sudo apt-get gcc-multilib

all: nautilus thunarx konq

nautilus: force_look
	$(MAKE) -C nautilus

thunarx: force_look
	$(MAKE) -C thunar

konq: force_look
	$(MAKE) -C konq

clean:
	$(MAKE) -C nautilus clean
	$(MAKE) -C thunar clean
	$(MAKE) -C konq clean

force_look:
	true
