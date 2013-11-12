include(4815162342_ubuntu_common.cmake)
set(CTEST_BUILD_NAME "ubuntu-12.04-x64-gcc-4.6.3-master")
set(dashboard_model "Nightly")
set(dashboard_source_name "ves-master")
set(dashboard_binary_name "build-master")

include(${CTEST_SCRIPT_DIRECTORY}/dashboard_common.cmake)
