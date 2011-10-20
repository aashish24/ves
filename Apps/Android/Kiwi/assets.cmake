set(kiwi_data ../../iOS/Kiwi/Kiwi/Data)

set(assets
  ${kiwi_data}/AppendedKneeData.vtp
  ${kiwi_data}/Buckyball.vtp
  ${kiwi_data}/MountStHelen.vtp
  ${kiwi_data}/bunny.vtp
  ${kiwi_data}/caffeine.pdb
  ${kiwi_data}/current.stl
  ${kiwi_data}/motor.g
  ${kiwi_data}/shuttle.vtp
  ${kiwi_data}/teapot.vtp
  ${kiwi_data}/visible-woman-hand.vtp
  ${kiwi_data}/head.vti
  ${kiwi_data}/kiwi.png
  ../../../src/shaders/Shader.fsh
  ../../../src/shaders/Shader.vsh
  ../../../src/shaders/BackgroundTexture.fsh
  ../../../src/shaders/BackgroundTexture.vsh
  ../../../src/shaders/BlinnPhong.fsh
  ../../../src/shaders/BlinnPhong.vsh
  ../../../src/shaders/ClipPlane.fsh
  ../../../src/shaders/ClipPlane.vsh
  ../../../src/shaders/ToonShader.fsh
  ../../../src/shaders/ToonShader.vsh
  )

foreach(f ${assets})
  get_filename_component(fname ${f} NAME)
  configure_file(${f} ${CMAKE_BINARY_DIR}/assets/${fname} COPYONLY)
endforeach()
