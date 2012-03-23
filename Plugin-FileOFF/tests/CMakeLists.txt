
# Include Testing package
if(BUILD_TESTING)

  include( ${CMAKE_SOURCE_DIR}/tests/testGenerators.cmake )

  # ========================================================================
  # ========================================================================
  # Tests
  # ========================================================================
  # ========================================================================
  run_single_object_file_mesh_test("fileoff" "Plugin-FileOFF/cube.off" "fileMeshTest.ofs")

endif()
