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

set(CTEST_BUILD_NAME "${CTEST_PROJECT_BRANCH}_${dashboard_arch}")
set(CTEST_SOURCE_DIRECTORY ${CTEST_DASHBOARD_ROOT}/srcs/${dashboard_source_name})
set(CTEST_BINARY_DIRECTORY ${CTEST_DASHBOARD_ROOT}/builds/${dashboard_binary_name})

include(${CTEST_SCRIPT_DIRECTORY}/dashboard_common.cmake)
