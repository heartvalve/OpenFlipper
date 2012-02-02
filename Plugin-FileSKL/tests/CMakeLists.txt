
# Include Testing package
if(BUILD_TESTING)

  include( ${CMAKE_SOURCE_DIR}/tests/testGenerators.cmake )

  # ========================================================================
  # ========================================================================
  # Tests
  # ========================================================================
  # ========================================================================
  run_single_object_file_mesh_test("fileskl" "SkeletonWithAnimation.skl" "fileSkeletonTest.ofs")
  run_single_object_file_mesh_test("fileskl" "SkeletonWithoutAnimation.skl" "fileSkeletonTest.ofs")

endif()
