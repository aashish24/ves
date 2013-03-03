
# Query site name
if (DEFINED ENV{CTEST_SITE})
  set(CTEST_SITE "$ENV{CTEST_SITE}")
else()
  execute_process(COMMAND hostname
                  OUTPUT_VARIABLE HOSTNAME
                  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  string(REGEX MATCH "^([^.]*)(\\..*)?$" not_used "${HOSTNAME}")
  set(CTEST_SITE "${CMAKE_MATCH_1}") # Work around CMake bug 13792
endif()

set(CTEST_BUILD_CONFIGURATION Debug)
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_FLAGS "-j4")
set(CTEST_TEST_ARGS PARALLEL_LEVEL 6)
set(CTEST_TEST_TIMEOUT 500)
set(ENV{DISPLAY} ":0")

# Set dashboard root
if(DEFINED ENV{CTEST_DASHBOARD_ROOT})
  set(CTEST_DASHBOARD_ROOT "$ENV{CTEST_DASHBOARD_ROOT}")
else()
  set(CTEST_DASHBOARD_ROOT "/source/ves/dashboards")
endif()

# Set project branch
if(DEFINED ENV{CTEST_PROJECT_BRANCH})
  set(CTEST_PROJECT_BRANCH "$ENV{CTEST_PROJECT_BRANCH}")
else()
  set(CTEST_PROJECT_BRANCH "master")
endif()

# Set dashboard model
if(DEFINED ENV{CTEST_DASHBOARD_MODEL})
  set(CTEST_DASHBOARD_MODEL "$ENV{CTEST_DASHBOARD_MODEL}")
else()
  set(CTEST_DASHBOARD_MODEL "Nightly")
endif()

if(APPLE)
  set(dashboard_arch "osx_${CMAKE_SYSTEM_PROCESSOR}")
else ()
  set(dashboard_arch "linux_${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(dashboard_model "${CTEST_DASHBOARD_MODEL}")
set(dashboard_source_name "ves_${CTEST_PROJECT_BRANCH}_${dashboard_model}")
set(dashboard_binary_name "ves_${CTEST_PROJECT_BRANCH}_${dashboard_model}")
set(dashboard_do_memcheck TRUE)
set(dashboard_do_coverage TRUE)

set(${CMAKE_SYSTEM_PROCESSOR})
set(CTEST_BUILD_NAME "${CTEST_PROJECT_BRANCH}_${dashboard_arch}")
set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")
set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE "${CTEST_DASHBOARD_ROOT}/supp.txt")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "-q --leak-check=full --num-callers=50")
#set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all --leak-check=full --show-reachable=yes --num-callers=50")

set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")

set(dashboard_build_test_dir "${CTEST_DASHBOARD_ROOT}/builds/${dashboard_binary_name}/CMakeExternals/Build/ves-host")

set(dashboard_cache "
  VES_HOST_SUPERBUILD:BOOL=TRUE
  VES_CXX_FLAGS:STRING=-Wall -Wextra -Wno-unused-variable -O0 -fprofile-arcs -ftest-coverage
")

include(${CTEST_SCRIPT_DIRECTORY}/cmake_common.cmake)
