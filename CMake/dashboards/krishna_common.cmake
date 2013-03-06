set(CTEST_BUILD_CONFIGURATION Debug)
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_FLAGS "-j4")
set(CTEST_TEST_ARGS PARALLEL_LEVEL 6)
set(CTEST_TEST_TIMEOUT 500)
set(ENV{DISPLAY} ":0")
set(ENV{CTEST_DASHBOARD_ROOT} "/source/dashboards")

# valgrind and gcov
set(dashboard_do_memcheck TRUE)
set(dashboard_do_coverage TRUE)

# rest
set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")
set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE "${CTEST_SCRIPT_DIRECTORY}/supp.txt")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "-q --leak-check=full --num-callers=50")
set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")
set(gcov_cxx_args "-O0 -fprofile-arcs -ftest-coverage")

# set cache
set(dashboard_cache "
  VES_HOST_SUPERBUILD:BOOL=TRUE
  VES_CXX_FLAGS:STRING=-Wall -Wextra ${gcov_cxx_args}
")
