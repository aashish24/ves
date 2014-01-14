set(CTEST_SITE "4815162342-ubuntu.kitware")
set(CTEST_BUILD_CONFIGURATION Debug)
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_FLAGS "-j4")
set(CTEST_TEST_ARGS PARALLEL_LEVEL 2)
set(CTEST_TEST_TIMEOUT 500)
set(CTEST_DASHBOARD_ROOT "${CTEST_SCRIPT_DIRECTORY}/../../..")
set(ENV{DISPLAY} ":0")
set(ENV{ANDROID_NDK} "~/Softwares/Android-NDK/")


# valgrind and gcov
set(dashboard_do_memcheck TRUE)
set(dashboard_do_coverage TRUE)
set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")
set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE "${CTEST_SCRIPT_DIRECTORY}/4815162342_ubuntu_valgrind_supp.txt")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "-q --leak-check=full --num-callers=50")
#set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all --leak-check=full --show-reachable=yes --num-callers=50")
set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")
set(gcov_cxx_args "-O0 -fprofile-arcs -ftest-coverage")


set(dashboard_cache "
VES_ANDROID_SUPERBUILD:BOOL=FALSE
VES_HOST_SUPERBUILD:BOOL=TRUE
VES_CXX_FLAGS:STRING=-Wall -Wextra ${gcov_cxx_args}
")
