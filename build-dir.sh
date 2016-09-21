#!/bin/sh
#
NR_CPUS=$(grep -c ^processor /proc/cpuinfo)
set -e
cd $1
make clean
make -j${NR_CPUS}
cd -
