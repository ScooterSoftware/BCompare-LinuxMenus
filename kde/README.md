# Beyond Compare plugin for KDE5 or KDE6

## Description
Dolphin right-click plugin.

## Prerequisites to build plugin

### Prerequisites for KDE6

For Ubuntu the following packages are necessary:

```
apt install qt6-base-dev kf6-kcoreaddons-dev kf6-ki18n-dev kf6-kio-dev
```

You may find in kubuntu 24.10 onwards the library names are different, in which case use:

```
apt install qt6-base-dev libkf6coreaddons-dev libkf6i18n-dev libkf6kio-dev
```

For Arch Linux the following packages are necessary:

```
pacman -S --needed kcoreaddons ki18n kio
```

## Build and install
### Build for KDE5

```
cmake -DTARGET_KDE6=OFF . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_TESTING=OFF
make
```

If the host building this plugin is targeting an old system, for example a debian 10, you should
add the following option to cmake command line: `-DUSE_KDEINIT_EXE=ON`

Generated plugin can be found in `bin/kf5/kfileitemaction/bcompare_ext_kde.so`

It can be copied manually to `/usr/lib/qt/plugins/kf5/kfileitemaction/`

### Build for KDE6

```
cmake -DTARGET_KDE6=ON . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_TESTING=OFF
make
```

Generated plugin can be found in `bin/kf6/kfileitemaction/bcompare_ext_kde.so`

It can be copied manually to `/usr/lib/qt6/plugins/kf6/kfileitemaction/`

### Install

To install directly on the system:
`sudo make install`

To "install" in a fakeroot in order to create a package:
`make DESTDIR="$pkgdir" install`

## Translation

### Information

 - https://techbase.kde.org/Development/Tutorials/Localization/i18n_Build_Systems
 - https://api.kde.org/frameworks/ki18n/html/prg_guide.html

### Update translation

```
git clone https://invent.kde.org/sysadmin/l10n-scripty.git
cd BCompare-LinuxMenus/kde
export PATH="$PATH:/path-to/l10n-scripty"
extract-messages.sh
msgmerge --update --backup=none --previous po/fr/bcompare_ext_kde.po po/bcompare_ext_kde.pot
rm po/bcompare_ext_kde.pot
```
