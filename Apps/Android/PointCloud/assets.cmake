set(assets
  ../../iOS/Kiwi/Kiwi/Data/cturtle.vtp
  ../../../src/shaders/vesShader_vert.glsl
  ../../../src/shaders/vesShader_frag.glsl
  )

foreach(f ${assets})
  get_filename_component(fname ${f} NAME)
  configure_file(${f} ${CMAKE_BINARY_DIR}/assets/${fname} COPYONLY)
endforeach()
