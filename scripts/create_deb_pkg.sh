#!/bin/sh

VERSION=$(scripts/version.sh)
EMAIL=pio.krzeminski@gmail.com
PBUILDERRC=scripts/.pbuilderrc

sudo pbuilder --create --configfile $PBUILDERRC
rm -fr ../qnapi_$VERSION*
dh_make -y -s -p qnapi_$VERSION -e $EMAIL --createorig
pdebuild --debbuildopts -sa --configfile $PBUILDERRC

cp -v $HOME/pbuilder/result/qnapi_*.deb .


