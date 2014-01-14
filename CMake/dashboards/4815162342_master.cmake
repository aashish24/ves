include(${CTEST_SCRIPT_DIRECTORY}/4815162342_common.cmake)
set(CTEST_BUILD_CONFIGURATION Release)
set(CTEST_BUILD_NAME "osx-10.8-clang-4.1-master-release")
set(dashboard_model "Nightly")
set(dashboard_source_name "ves-master")
set(dashboard_binary_name "build-master")
set(CTEST_PROJECT_BRANCH master)

include(${CTEST_SCRIPT_DIRECTORY}/dashboard_common.cmake)
