set(CTEST_SITE "krishna.kitware")
set(CTEST_BUILD_NAME "linux-x64-gcc-4.4.3")
set(CTEST_BUILD_CONFIGURATION Debug)
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_FLAGS "-j4")
set(CTEST_TEST_ARGS PARALLEL_LEVEL 6)
set(CTEST_TEST_TIMEOUT 500)
set(ENV{DISPLAY} ":0")

set(CTEST_DASHBOARD_ROOT "/source/ves/dashboards")
set(dashboard_model "Nightly")
set(dashboard_source_name "ves")
set(dashboard_binary_name "build")
set(dashboard_do_memcheck TRUE)
set(dashboard_do_coverage TRUE)

set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")
set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE "/source/ves/dashboards/supp.txt")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "-q --leak-check=full --num-callers=50")
#set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all --leak-check=full --show-reachable=yes --num-callers=50")

set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")

set(dashboard_build_test_dir "/source/ves/dashboards/build/CMakeExternals/Build/ves-host")

set(dashboard_cache "
VES_HOST_SUPERBUILD:BOOL=TRUE
VES_CXX_FLAGS:STRING=-Wall -Wextra -Wno-unused-variable -O0 -fprofile-arcs -ftest-coverage
")

include(${CTEST_SCRIPT_DIRECTORY}/cmake_common.cmake)
