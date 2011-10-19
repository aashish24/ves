
add_executable(TestKiwiViewer TestKiwiViewer.cpp)
target_link_libraries(TestKiwiViewer vtkVES GLESv2 EGL X11)
add_test(TestKiwiViewer ${EXECUTABLE_OUTPUT_PATH}/TestKiwiViewer ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)

add_executable(TestClipPlane TestClipPlane.cpp)
target_link_libraries(TestClipPlane vtkVES GLESv2 EGL X11)
add_test(TestClipPlane ${EXECUTABLE_OUTPUT_PATH}/TestClipPlane ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
