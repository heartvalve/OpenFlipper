# This file contains helper functions to generate tests.


# This function generates a test for a mesh file plugin.
# a single object has to be contained in the file that is loaded!
#
# The script name of the file plugin is the first parameter (e.g. fileobj)
# The second parameter is the name of the file that should be loaded from the TestData directory.
# If that directory or file does not exist, no test will be generated!
#
# The third parameter is the script that will be used. It will be taken from the global tests directory
#
# An optional fourth parameter can be provided specifying a whole directory with meshes.
# The TEST_FILE will be ignored and tests will be run on the meshes in the given directory.
# For now the directory will not be traversed recursively.
# TODO: also take local scripts!
#
# Used files:
# fileMeshTest.ofs ( configured and copied script for meshes )
# run_file_test.cmake ( Executing the test and comparing the results)
function( run_single_object_file_mesh_test FILEPLUGIN TEST_FILE TEST_SCRIPT )

  if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData)
      return()
  endif()

  # construct the testname from target test file and the plugin directory we are in
  # Use only the plugin name and not the collection before it
  string (REGEX MATCH "Plugin-.+[/\\]?$" PLUGIN_DIR ${_plugin_dir})
  string (TOUPPER ${PLUGIN_DIR} PLUGIN_DIR)

  list( LENGTH ARGN opt_arg_length )
  # handle a whole directory with test files
  if ( opt_arg_length EQUAL 1 )
    list(GET ARGN 0 dir)

    if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData/${dir} )
        return()
    endif()

    # get the file ending (file type)
    string( REPLACE "file" "" file_ending ${FILEPLUGIN} )

    file( GLOB files ${CMAKE_SOURCE_DIR}/TestData/${dir}/*.${file_ending} )
    foreach( testfile ${files} )

      # get the file name
      file( RELATIVE_PATH filename ${CMAKE_SOURCE_DIR}/TestData/${dir} ${testfile} )

      configure_file(${CMAKE_SOURCE_DIR}/TestData/${dir}/${filename}
        ${OPENFLIPPER_TEST_FILES}/${dir}/${filename} COPYONLY)

      # replace possible forward slashes in TEST_FILE in order to avoid the creation
      # of a new directory when the test script is configured
      string (REPLACE "/" "-" TEST_FILE_NAME ${filename})
      set (TESTNAME "${PLUGIN_DIR}-${TEST_FILE_NAME}")

      set (TESTSCRIPTNAME "testscript-${TESTNAME}-${TEST_SCRIPT}")

      # update the output file to be inside the results directory
      set (OUTPUT_TEST_DATA_FILE "${OPENFLIPPER_TEST_RESULTS}/${TESTSCRIPTNAME}-result.txt" )

      # set file containing the information about the file to be checked
      set (TEST_FILE_INFO ${testfile}.info  )

      set (TEST_FILE ${dir}/${filename})

      # Only get the plugins name without possible collection stuff:
      string (REGEX MATCH "Plugin-.+[/\\]?$" _plugin_name ${_plugin_dir}) 

      # Configure the test script from the current directory with the given filenames and variables into the test directory
      configure_file(${CMAKE_SOURCE_DIR}/tests/${TEST_SCRIPT}
       ${CMAKE_BINARY_DIR}/tests/${_plugin_name}/${TESTSCRIPTNAME} @ONLY )

      # Execute the script by OpenFlipper and than run the result parser which checks for correct values.
      set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
      set( args "-c -b ${TESTSCRIPTNAME}" )
      add_test( ${TESTNAME}
         ${CMAKE_COMMAND}
         -D test_cmd=${test_cmd}
         -D test_args:string=${args}
         -D output_test=${OUTPUT_TEST_DATA_FILE}
         -D test_file_info=${TEST_FILE_INFO}
         -D result_checker=${OPENFLIPPER_TEST_BINARIES}/compareTool
         -P ${CMAKE_SOURCE_DIR}/tests/run_file_test.cmake
      )

      # Timeout after 3 minutes if we have an endless loop
      # Should be run serial to avoid collisons with other instances
      # Only one processor required
      set_tests_properties (
          ${TESTNAME} PROPERTIES
          TIMEOUT 180
          RUN_SERIAL TRUE
          PROCESSORS 1
      )
    endforeach()

  # handle a single test file
  elseif( opt_arg_length EQUAL 0)

    if ( NOT EXISTS ${CMAKE_SOURCE_DIR}/TestData/${TEST_FILE} )
        return()
    endif()

    configure_file(${CMAKE_SOURCE_DIR}/TestData/${TEST_FILE}
      ${OPENFLIPPER_TEST_FILES}/${TEST_FILE} COPYONLY)

    # replace possible forward slashes in TEST_FILE in order to avoid the creation
    # of a new directory when the test script is configured
    string (REPLACE "/" "-" TEST_FILE_NAME ${TEST_FILE})
    set (TESTNAME "${PLUGIN_DIR}-${TEST_FILE_NAME}")

    set (TESTSCRIPTNAME "testscript-${TESTNAME}-${TEST_SCRIPT}")

    # update the output file to be inside the results directory
    set (OUTPUT_TEST_DATA_FILE "${OPENFLIPPER_TEST_RESULTS}/${TESTSCRIPTNAME}-result.txt" )

    # set file containing the information about the file to be checked
    set (TEST_FILE_INFO ${CMAKE_SOURCE_DIR}/TestData/${TEST_FILE}.info  )

    # Only get the plugins name without possible collection stuff:
    string (REGEX MATCH "Plugin-.+[/\\]?$" _plugin_name ${_plugin_dir})

    # Configure the test script from the current directory with the given filenames and variables into the test directory
    configure_file(${CMAKE_SOURCE_DIR}/tests/${TEST_SCRIPT}
     ${CMAKE_BINARY_DIR}/tests/${_plugin_name}/${TESTSCRIPTNAME} @ONLY )

    # Execute the script by OpenFlipper and than run the result parser which checks for correct values.
    set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
    set( args "-c -b ${TESTSCRIPTNAME}" )
    add_test( ${TESTNAME}
       ${CMAKE_COMMAND}
       -D test_cmd=${test_cmd}
       -D test_args:string=${args}
       -D output_test=${OUTPUT_TEST_DATA_FILE}
       -D test_file_info=${TEST_FILE_INFO}
       -D result_checker=${OPENFLIPPER_TEST_BINARIES}/compareTool
       -P ${CMAKE_SOURCE_DIR}/tests/run_file_test.cmake
    )

    # Timeout after 3 minutes if we have an endless loop
    # Should be run serial to avoid collisons with other instances
    # Only one processor required
    set_tests_properties (
        ${TESTNAME} PROPERTIES
        TIMEOUT 180
        RUN_SERIAL TRUE
        PROCESSORS 1
    )
  endif()

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

  # Get the plugin name:
  # construct the testname from target test file and the plugin directory we are in
  # Use only the plugin name and not the collection before it
  string (REGEX MATCH "Plugin-.+[/\\]?$" PLUGIN_DIR ${_plugin_dir})
  string (TOUPPER ${PLUGIN_DIR} PLUGIN_DIR)


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

  # Only get the plugins name without possible collection stuff:
  string (REGEX MATCH "Plugin-.+[/\\]?$" _plugin_name ${_plugin_dir})

  # Configure the test script from the current directory with the given filenames and variables into the test directory
  configure_file( ${CMAKE_SOURCE_DIR}/${_plugin_dir}/tests/${TEST_SCRIPT}
                  ${OPENFLIPPER_TEST_FILES}/${_plugin_name}/${TEST_FILE} )

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
     -D result_checker=${OPENFLIPPER_TEST_BINARIES}/compareTool
     -P ${CMAKE_SOURCE_DIR}/tests/run_file_test.cmake
  )

  # Timeout after 3 minutes if we have an endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      ${TESTNAME} PROPERTIES
      TIMEOUT 180
      RUN_SERIAL TRUE
      PROCESSORS 1
  )
endfunction()





















