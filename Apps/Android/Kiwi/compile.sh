#!/bin/sh

source tools.sh

cd build
make || exit

cd ..
$ANT debug || exit
