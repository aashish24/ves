
add_executable(TestRender TestRender.cpp)
target_link_libraries(TestRender vtkVES ves vtkIOXML vtkCommonDataModel GLESv2 EGL X11 m)
add_test(TestRender ${EXECUTABLE_OUTPUT_PATH}/TestRender ${VES_SOURCE_DIR} 1)
