#!/bin/bash

echo $(dirname $0)
cd $(dirname $0)
git pull

ctest -S krishna_next.cmake -VV
ctest -S krishna_master.cmake -VV
