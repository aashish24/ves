#!/bin/bash

cd $(dirname $0)

git stash
git pull

/opt/local/bin/ctest -S 4815162342_next.cmake -VV
/opt/local/bin/ctest -S 4815162342_master.cmake -VV