#!/bin/sh

find gui/src libqnapi/src -name '*.cpp' -exec clang-format -i {} \;
find gui/src libqnapi/src -name '*.h' -exec clang-format -i {} \;

