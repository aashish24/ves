#
# This function includes ${dir}/headers.cmake to get a list of header files and
# include directories. It will install the header files and add the include directories.
#
function(process_dir dir)
    set(headers)
    set(header_dirs)
    include(${dir}/headers.cmake)

    set(headers_with_dir)
    foreach(header ${headers})
      list(APPEND headers_with_dir ${dir}/${header})
    endforeach()
    if(headers_with_dir)
      ves_install_files("${headers_with_dir}" "${VES_INSTALL_INCLUDE_DIR}")
    endif()

    set(headers_with_dir)
    foreach(header ${build_headers})
      list(APPEND headers_with_dir "${CMAKE_CURRENT_BINARY_DIR}/${dir}/${header}")
    endforeach()
    if(headers_with_dir)
      ves_install_build_files("${headers_with_dir}" "${VES_INSTALL_INCLUDE_DIR}")
    endif()

    set(header_dirs_with_dir)
    foreach(header_dir ${header_dirs})
      list(APPEND header_dirs_with_dir ${dir}/${header_dir})
    endforeach()
    if(header_dirs_with_dir)
      ves_add_relative_include_dirs("${header_dirs_with_dir}")
    endif()

    set(header_dirs_with_dir)
    foreach(header_dir ${build_header_dirs})
      list(APPEND header_dirs_with_dir ${dir}/${header_dir})
    endforeach()
    if(header_dirs_with_dir)
      ves_add_build_relative_include_dirs("${header_dirs_with_dir}")
    endif()

endfunction()


process_dir(.)
process_dir(ves)
process_dir(shaders)

if(VES_USE_VTK)
  process_dir(kiwi)
endif()


