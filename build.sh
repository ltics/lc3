#!/bin/bash
set +e
set -x

rm -rf build
mkdir build && cd build
cmake ..
make
make test
if [ ! -f "lc3" ]
then
    echo "File lc3 does not exist"
    exit 1
fi
