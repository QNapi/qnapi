#!/bin/sh

make distclean
export DST_DIR=../qnapi-$VERSION
mkdir $DST_DIR
cp -rv * $DST_DIR
find $DST_DIR | grep .git | xargs rm -fr
rm -fr $DST_DIR/deps/libmediainfo
rm -fr $DST_DIR/win32/content/7za.exe
rm -fr $DST_DIR/macx/content/7za
rm -fr $DST_DIR/macx/QNapi*app
rm -fr $DST_DIR/macx/QNapi*dmg
rm -fr $DST_DIR/libqnapi/tmp
rm -fr $DST_DIR/gui/tmp
rm -fr $DST_DIR/tmp
rm -fr $DST_DIR/appveyor.yml
rm -fr $DST_DIR/.travis.yml
rm -fr $DST_DIR/qnapi
rm -fr $DST_DIR/QNapi*.AppImage
tar -zcvf qnapi-$VERSION.tar.gz $DST_DIR
rm -fr $DST_DIR

