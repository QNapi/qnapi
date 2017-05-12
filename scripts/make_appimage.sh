#!/bin/sh

make INSTALL_ROOT=appdir install
find appdir

wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt*.AppImage
unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH

./linuxdeployqt*.AppImage ./appdir/usr/share/applications/*.desktop -bundle-non-qt-libs
sudo apt-get install -qq p7zip-full
cp -v /usr/lib/p7zip/7za ./appdir/usr/bin/7za
./linuxdeployqt*.AppImage ./appdir/usr/share/applications/*.desktop -appimage

find ./appdir -executable -type f -exec ldd {} \; | grep " => /usr" | cut -d " " -f 2-3 | sort | uniq

find QNapi*

rm -fr linuxdeployqt*.AppImage
rm -fr appdir

