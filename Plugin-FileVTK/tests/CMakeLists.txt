
# Include Testing package
if(BUILD_TESTING)

  include( ${CMAKE_SOURCE_DIR}/tests/testGenerators.cmake )

  # ========================================================================
  # ========================================================================
  # Tests
  # ========================================================================
  # ========================================================================
  run_single_object_file_mesh_test("filevtk" "Plugin-FileVTK/triangle_mesh_ascii.vtk" "fileMeshTest.ofs")

endif()
