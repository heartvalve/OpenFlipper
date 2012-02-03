# This file contains helper functions to generate tests.


# This function generates a test for a mesh file plugin.
# a single object has to be contained in the file that is loaded!
#
# The script name of the file plugin is the first paraeter (e.g. fileobj)
# The second parameter is the name of the file that should be loaded fro the toplevel TestData directory.
# If that directory or file does not exist, no test will be generated! 
#
# The third parameter is the script that will be used. It will be taken from the global tests directory
# TODO: also take local scripts!
#
# Used files:
# fileMeshTest.ofs ( configured and copied script for meshes )
# run_file_test.cmake ( Executing the test and comparing the results) 
function( run_single_object_file_mesh_test FILEPLUGIN TEST_FILE TEST_SCRIPT  )

  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData)
      return()
  endif()

  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData/${TEST_FILE} )
      return()
  endif()

  configure_file(${CMAKE_SOURCE_DIR}/TestData/${TEST_FILE}
    ${OPENFLIPPER_TEST_FILES}/${TEST_FILE} COPYONLY)

  # construct the testname from target test file and the plugin directory we are in
  string (TOUPPER ${_plugin_dir} PLUGIN_DIR)
  set (TESTNAME "${PLUGIN_DIR}-${TEST_FILE}")

  set (TESTSCRIPTNAME "testscript-${TESTNAME}-${TEST_SCRIPT}")

  # update the output file to be inside the results directory
  set (OUTPUT_TEST_DATA_FILE "${OPENFLIPPER_TEST_RESULTS}/${TESTSCRIPTNAME}-result.txt" )

  # set file containing the information about the file to be checked
  set (TEST_FILE_INFO ${CMAKE_SOURCE_DIR}/TestData/${TEST_FILE}.info  )

  # Configure the test script from the current directory with the given filenames and variables into the test directory
  configure_file(${CMAKE_SOURCE_DIR}/tests/${TEST_SCRIPT}
   ${CMAKE_BINARY_DIR}/tests/${_plugin_dir}/${TESTSCRIPTNAME} @ONLY )

  # Execute the script by OpenFlipper and than run the result parser which checks for correct values.
  set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
  set( args "-c -b ${TESTSCRIPTNAME}" )
  add_test( ${TESTNAME}
     ${CMAKE_COMMAND}
     -D test_cmd=${test_cmd}
     -D test_args:string=${args}
     -D output_test=${OUTPUT_TEST_DATA_FILE}
     -D test_file_info=${TEST_FILE_INFO}
     -D result_checker=${CMAKE_BINARY_DIR}/tests/compareTool
     -P ${CMAKE_SOURCE_DIR}/tests/run_file_test.cmake
  )

  # Timeout after 2 minutes if we have an endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      ${TESTNAME} PROPERTIES
      TIMEOUT 120
      RUN_SERIAL TRUE
      PROCESSORS 1
  )
endfunction()




# This function generates a test for algorithmic plugins.
#
# The script name that is used is the first parameter. It has to be inside the plugins test directory (e.g. Plugin-Smoother/tests/script.ofs)
# It will be configured such that the following variables get replaced automatically:
#
# @OPENFLIPPER_TEST_INPUT_FILE@     : The file that should be loaded to run the algorithm on
# @OPENFLIPPER_TEST_REFERENCE_FILE@ : The reference that will be used for the compare operation
# @OUTPUT_TEST_DATA_FILE@           : The file that the results will be written to
#
# The second parameter is the name of the input file that will be loaded from the TestData/Plugin-<Name> directory.
#
# The third paramater is a reference file that is used to compare the algorithms output with. An info file with the
# name of this reference file has to exist specifying the expected results of the algorithm and the compare that is
# performed by the script.
#
#
function( run_algorithm_test TEST_SCRIPT INPUT_FILE INPUT_REFERENCE )

  #Get the plugin name:
  string (TOUPPER ${_plugin_dir} PLUGIN_DIR)

  # check if we have the testData directory
  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData)
      return()
  endif()



 # Check if we find the script file
 if ( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT} )
     message("No Input File: ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT} ")
     return()
 endif()


  # Check if we find the input file
  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_FILE} )
      message("No Input File:  ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_FILE} ")
      return()
  endif()

  set (OPENFLIPPER_TEST_INPUT_FILE "${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_FILE}")
  
  # Check if we find the reference file
  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_REFERENCE} )
      message("No Reference File  ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_REFERENCE} ")
      return()
  endif()
 
  set (OPENFLIPPER_TEST_REFERENCE_FILE "${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_REFERENCE}")
    
  # Check if we find the reference result file
  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_REFERENCE}.info )
      message("No Result info File ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_REFERENCE}.info ")
      return()
  endif()

  # construct the testname from target test file and the plugin directory we are in
  set (TESTNAME "${PLUGIN_DIR}-${TEST_SCRIPT}-${INPUT_FILE}")

  # update the output file to be inside the results directory
  set (OUTPUT_TEST_DATA_FILE "${OPENFLIPPER_TEST_RESULTS}/${TESTNAME}-result.txt" )

  # set file containing the information about the file to be checked
  set (TEST_FILE_INFO ${CMAKE_SOURCE_DIR}/TestData/${_plugin_dir}/${INPUT_REFERENCE}.info  )

  # Create the test scripts directory if it does not exist
  if (NOT EXISTS  ${OPENFLIPPER_TEST_FILES}/${_plugin_dir})
    FILE(MAKE_DIRECTORY ${OPENFLIPPER_TEST_FILES}/${_plugin_dir} )
  endif()
  
  # Configure the test script from the current directory with the given filenames and variables into the test directory
  configure_file( ${CMAKE_SOURCE_DIR}/${_plugin_dir}/tests/${TEST_SCRIPT} 
                  ${OPENFLIPPER_TEST_FILES}/${_plugin_dir}/${TEST_FILE} )

  # Set the filename and path for the configured script
  
  set(TESTSCRIPTNAME "${OPENFLIPPER_TEST_FILES}/${_plugin_dir}/${TEST_SCRIPT}" )
  
  # Execute the script by OpenFlipper and than run the result parser which checks for correct values.
  set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
  set( args "-c -b ${TESTSCRIPTNAME}" )
  add_test( ${TESTNAME}
     ${CMAKE_COMMAND}
     -D test_cmd=${test_cmd}
     -D test_args:string=${args}
     -D output_test=${OUTPUT_TEST_DATA_FILE}
     -D test_file_info=${TEST_FILE_INFO}
     -D result_checker=${CMAKE_BINARY_DIR}/tests/compareTool
     -P ${CMAKE_SOURCE_DIR}/tests/run_file_test.cmake
  )

  # Timeout after 2 minutes if we have an endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      ${TESTNAME} PROPERTIES
      TIMEOUT 120
      RUN_SERIAL TRUE
      PROCESSORS 1
  )
endfunction()





















