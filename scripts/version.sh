#!/bin/sh
VERSION_FILE=libqnapi/src/version.h
(grep QNAPI_DISPLAYABLE_VERSION | awk '{gsub(/"/, "", $3); print $3}') < $VERSION_FILE

