#!/bin/bash

cd $(dirname $0)
git pull

ctest -S trisol_ubuntu_next.cmake -VV
ctest -S trisol_ubuntu_master.cmake -VV
