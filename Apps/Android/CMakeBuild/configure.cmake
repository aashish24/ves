set(build_dir ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(source_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${build_dir}")
execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release -DVES_ANDROID_SUPERBUILD:BOOL=TRUE "${source_dir}"
                WORKING_DIRECTORY "${build_dir}")

