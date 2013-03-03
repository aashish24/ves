#!/bin/bash

cd $(dirname $0)
git pull

ctest -S trisol_next.cmake -VV
ctest -S trisol_master.cmake -VV
