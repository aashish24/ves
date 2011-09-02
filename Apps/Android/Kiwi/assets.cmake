set(kiwi_data ../../iOS/Kiwi/Kiwi/Data)

set(assets
  ${kiwi_data}/AppendedKneeData.vtp
  ${kiwi_data}/Buckyball.vtp
  ${kiwi_data}/MountStHelen.vtp
  ${kiwi_data}/bunny.vtp
  ${kiwi_data}/caffeine.pdb
  ${kiwi_data}/current.vtk
  ${kiwi_data}/motor.g
  ${kiwi_data}/shuttle.vtp
  ${kiwi_data}/teapot.vtp
  ${kiwi_data}/visible-woman-hand.vtp
  ../../../src/shaders/Shader.fsh
  ../../../src/shaders/Shader.vsh
)

foreach(f ${assets})
  get_filename_component(fname ${f} NAME)
  configure_file(${f} ${CMAKE_SOURCE_DIR}/assets/${fname} COPYONLY)
endforeach()
