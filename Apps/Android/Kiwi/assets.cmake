set(kiwi_data ../../iOS/Kiwi/Kiwi/Data)

set(assets
  "textured_sphere.vtp"
  "teapot.vtp"
  "bunny.vtp"
  "visible-woman-hand.vtp"
  "AppendedKneeData.vtp"
  "cturtle.vtp"
  "MountStHelen.vtp"
  "shuttle.vtp"
  "nasa-blue-marble.kiwi"
  "Buckyball.vtp"
  "caffeine.pdb"
  "head.vti"
  "kiwi.png"
)

foreach(f ${assets})
  get_filename_component(fname ${f} NAME)
  configure_file("${kiwi_data}/${f}" "${CMAKE_BINARY_DIR}/assets/${fname}" COPYONLY)
endforeach()
