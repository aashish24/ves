#!/bin/bash
ctest=$(which ctest)
${VES_SOURCE_ROOT:?"Need to set VES_SOURCE_ROOT to the VES clone directory"}
cd ${VES_SOURCE_ROOT}/CMake/dashboards
$ctest -S ./linux_memcheck_dashboard.cmake -VV
