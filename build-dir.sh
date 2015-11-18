#!/bin/sh
#
set -e
cd $1
make clean
make -j3
cd -
