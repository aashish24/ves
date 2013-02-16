#!/bin/bash

ctest=$HOME/tools/cmake/install_head/bin/ctest

cd /source/ves/dashboards
$ctest -S ./linux_memcheck_dashboard.cmake -VV
