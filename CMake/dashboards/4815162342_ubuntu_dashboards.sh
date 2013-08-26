#!/bin/bash

cd $(dirname $0)
git pull

ctest -S 4815162342_ubuntu_next.cmake -VV
ctest -S 4815162342_ubuntu_master.cmake -VV
