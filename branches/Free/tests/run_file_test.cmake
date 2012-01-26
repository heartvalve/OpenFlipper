# some argument checking:
# test_cmd is the command to run with all its arguments
if( NOT test_cmd )
   message( FATAL_ERROR "Variable test_cmd not defined" )
endif( NOT test_cmd )

# output_test contains the name of the output file the test_cmd will produce
if( NOT output_test )
   message( FATAL_ERROR "Variable output_test not defined" )
endif( NOT output_test )

# output_test contains the info about the loaded file
if( NOT test_file_info )
   message( FATAL_ERROR "Variable test_file_info= not defined" )
endif( NOT test_file_info )

# convert the space-separated string to a list
separate_arguments( test_args )
message( ${test_args} )

# ===================================================
# Run OpenFlipper with the given script
# ===================================================

# Execute the process with the given arguments
# Timeout after 2 minutes
execute_process(
   COMMAND ${test_cmd} ${test_args}
   TIMEOUT 120
   RESULT_VARIABLE PROCESSRESULT
)

if ( NOT ${PROCESSRESULT} EQUAL 0 )
    message(SEND_ERROR "Process execution failed!")
endif()

# ===================================================
# Collect list of what should be tested
# ===================================================

# Read the info file
file (STRINGS ${test_file_info} FILEINFO)

#Create a list of properties we want to test
foreach(loop ${FILEINFO})
  string ( REGEX REPLACE "=.*$" "" TMP ${loop} )
  list( APPEND CHECKS ${TMP})
endforeach()

# ===================================================
# Read the files
# ===================================================

# read the test output
file (READ ${output_test} TESTOUTPUT)

# read the test output
file (READ ${test_file_info} INFOFILE)

# ===================================================
# Compare
# ===================================================

# Now go through all checks:
foreach(loop ${CHECKS})

  #Get value from results:
  string (
      REGEX REPLACE
      "^.*${loop}=([^\n]*).*$" "\\1"
      RESULT  ${TESTOUTPUT}
  )

  #Get value from Info:
  string (
      REGEX REPLACE
      "^.*${loop}=([^\n]*).*$" "\\1"
      EXPECTED  ${INFOFILE}
  )

  if ( NOT ${EXPECTED} EQUAL ${RESULT} )
    message(WARNING "Mismatching values for ${loop}: EXPECTED ${EXPECTED} but got ${RESULT}!")
    set(test_not_successful true)
  endif()

endforeach()

if( test_not_successful )
   message( SEND_ERROR "Test Failed! See messages above to see what went wrong!" )
endif( test_not_successful )
