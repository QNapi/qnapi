#!/bin/sh
VERSION_FILE=src/version.h
cat $VERSION_FILE | grep QNAPI_DISPLAYABLE_VERSION | awk '{gsub(/"/, "", $3); print $3}'
