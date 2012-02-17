set(kiwi_data ../../iOS/Kiwi/Kiwi/Data)

set(assets
  ${kiwi_data}/AppendedKneeData.vtp
  ${kiwi_data}/Buckyball.vtp
  ${kiwi_data}/MountStHelen.vtp
  ${kiwi_data}/bunny.vtp
  ${kiwi_data}/caffeine.pdb
  ${kiwi_data}/shuttle.vtp
  ${kiwi_data}/teapot.vtp
  ${kiwi_data}/visible-woman-hand.vtp
  ${kiwi_data}/head.vti
  ${kiwi_data}/kiwi.png
  ${kiwi_data}/cturtle.vtp
  ${kiwi_data}/textured_sphere.vtp
  ${kiwi_data}/earth.jpg
  )

foreach(f ${assets})
  get_filename_component(fname ${f} NAME)
  configure_file(${f} ${CMAKE_BINARY_DIR}/assets/${fname} COPYONLY)
endforeach()
