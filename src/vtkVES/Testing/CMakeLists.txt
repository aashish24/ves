
add_executable(TestKiwiViewer TestKiwiViewer.cpp)
target_link_libraries(TestKiwiViewer vtkVES GLESv2 EGL X11)
add_test(TestKiwiViewer ${EXECUTABLE_OUTPUT_PATH}/TestKiwiViewer ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)

add_executable(TestClipPlane TestClipPlane.cpp)
target_link_libraries(TestClipPlane vtkVES GLESv2 EGL X11)
add_test(TestClipPlane ${EXECUTABLE_OUTPUT_PATH}/TestClipPlane ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)

add_executable(TestTexture TestTexture.cpp)
target_link_libraries(TestTexture vtkVES GLESv2 EGL X11)
add_test(TestTexture ${EXECUTABLE_OUTPUT_PATH}/TestTexture ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)

add_executable(TestGradientBackground TestGradientBackground.cpp)
target_link_libraries(TestGradientBackground vtkVES GLESv2 EGL X11)
add_test(TestGradientBackground ${EXECUTABLE_OUTPUT_PATH}/TestGradientBackground ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
