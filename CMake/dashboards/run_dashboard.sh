#!/bin/bash

ctest=/usr/bin/ctest

cd /source/ves/dashboards
$ctest -S ./linux_memcheck_dashboard.cmake -VV
