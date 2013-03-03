include(${CTEST_SCRIPT_DIRECTORY}/krishna_common.cmake)
set(ENV{CTEST_PROJECT_BRANCH} "next")
set(ENV{CTEST_DASHBOARD_MODEL} "Nightly")
include(${CTEST_SCRIPT_DIRECTORY}/dashboard.cmake)
