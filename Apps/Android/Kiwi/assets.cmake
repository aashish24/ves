set(kiwi_data ../../iOS/KiwiViewer/KiwiViewer/data)

set(assets
  "${kiwi_data}/Can Simulation.zip"
  "${kiwi_data}/Kinect Point Cloud.pcd"
  "${kiwi_data}/Molecule PDB-7WGA.pdb"
  "${kiwi_data}/NA-MIC Knee Atlas.vtp"
  "${kiwi_data}/NASA Blue Marble.zip"
  "${kiwi_data}/NLM Visible Human Project.vti"
  "${kiwi_data}/PHASTA Blood Flow Sim.zip"
  "${kiwi_data}/SPL-PNL Brain Atlas.zip"
  "${kiwi_data}/Velodyne Lidar.zip"
  "${kiwi_data}/textured_sphere.vtp"
  )

foreach(f ${assets})
  get_filename_component(fname ${f} NAME)
  configure_file("${f}" "${CMAKE_BINARY_DIR}/assets/${fname}" COPYONLY)
endforeach()
