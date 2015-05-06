This is the build file for the 'konq' directory version that is meant to run on KDE 4.6.0 and lower.  The 'kde' directory is meant for KDE 4.6.1 and newer.

Currently built on KUbuntu 9.10 64 bit.

NOTES:
KUbuntu 9.10 is the sweet spot that builds the libraries such that they will run on
older and newer distros.  This is most important for the konqueror code that is
linking with QT libraries.

64 bit was used to take advantage of the native 64 bit code.  The 64 bit libraries
build better on 64 bit machines while the 32 bit libraries don't seem to care.

BUILD:
- Copy ContextMenus Directory to build machine.
- Install all needed development tools and libraries (See top of top level Makefile
  for list).
- CD to ContextMenus directory.
- Type 'make'.

To make just 32 bit, run 'make ext32'.  To make just 64 bit, run 'make ext64'.


