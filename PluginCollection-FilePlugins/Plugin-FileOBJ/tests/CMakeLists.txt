
# Include Testing package
if(BUILD_TESTING)

  include( ${CMAKE_SOURCE_DIR}/tests/testGenerators.cmake )

  # ========================================================================
  # ========================================================================
  # Tests
  # ========================================================================
  # ========================================================================
  run_single_object_file_mesh_test("fileobj" "Plugin-FileOBJ/covering-branch.obj" "fileMeshTest.ofs")
  run_single_object_file_mesh_test("fileobj" "Plugin-FileOBJ/triangle2D_only_points.obj" "fileMeshTest.ofs")

endif()
