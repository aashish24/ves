include(setup-headers.cmake)

add_subdirectory(ves)

if(VES_USE_VTK)
  add_subdirectory(vtkVES)
endif()
