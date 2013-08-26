#!/bin/bash

cd $(dirname $0)

git pull

export ANDROID_NDK=~/Softwares/Android-NDK/

ctest -S 4815162342_next.cmake -VV
ctest -S 4815162342_master.cmake -VV
