#!/bin/bash

ctest=$HOME/install/cmake-2.8.6/bin/ctest

cd /source/ves/dashboards
$ctest -S ./linux_memcheck_dashboard.cmake -VV
