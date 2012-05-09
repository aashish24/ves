set(shaders
  vesBackgroundTexture_frag.glsl
  vesBackgroundTexture_vert.glsl
  vesBlinnPhong_frag.glsl
  vesBlinnPhong_vert.glsl
  vesCap_frag.glsl
  vesCap_vert.glsl
  vesClipPlane_frag.glsl
  vesClipPlane_vert.glsl
  vesGouraudTexture_frag.glsl
  vesGouraudTexture_vert.glsl
  vesShader_frag.glsl
  vesShader_vert.glsl
  vesTestTexture_frag.glsl
  vesTestTexture_vert.glsl
  vesToonShader_frag.glsl
  vesToonShader_vert.glsl
  )

find_package(PythonInterp REQUIRED)
set(pyscript vesGenerateShaderSources.py)
set(factory_name vesBuiltinShaders)
set(cpp_sources "${CMAKE_CURRENT_BINARY_DIR}/${factory_name}.cpp")


foreach(shader ${shaders})
  get_filename_component(shader_we ${shader} NAME_WE)
  list(APPEND cpp_sources ${shader_we}.cpp)
endforeach()


add_custom_command(
  OUTPUT ${cpp_sources}
  DEPENDS ${shaders} ${pyscript}
  COMMAND ${PYTHON_EXECUTABLE}
  ARGS ${pyscript} "${CMAKE_CURRENT_BINARY_DIR}" ${factory_name} ${shaders}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )


ves_add_library(vesShaders "${cpp_sources}" "")
