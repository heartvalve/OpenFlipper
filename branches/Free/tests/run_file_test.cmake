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
   message( FATAL_ERROR "Variable test_file_info not defined" )
endif( NOT test_file_info )

if( NOT result_checker )
   message( FATAL_ERROR "Variable result_checker not defined" )
endif( NOT result_checker )


# convert the space-separated string to a list
separate_arguments( test_args )

# ===================================================
# Run OpenFlipper with the given script
# ===================================================

if ( NOT EXISTS ${test_cmd} )
    message("Unable to find test command : ${test_cmd}")
    message(SEND_ERROR "Test Executable missing!")
endif()

# clear previous test results
if ( EXISTS ${output_test} )
  file(REMOVE ${output_test})
endif()

# Execute the process with the given arguments
# Timeout after 5 minutes
execute_process(
   COMMAND ${test_cmd} ${test_args}
   TIMEOUT 300
   RESULT_VARIABLE PROCESSRESULT
)

if ( NOT ${PROCESSRESULT} EQUAL 0 )
    message("Problem when executing proccess. Return code was ${PROCESSRESULT}")
    message("Test command: ${test_cmd} ${test_args}")
    message(SEND_ERROR "Process execution failed!")
endif()

# ===================================================
# Compare
# ===================================================



if ( WIN32 )
  set(result_checker "${result_checker}.exe")
endif()

if ( NOT EXISTS ${result_checker} )
    message("Unable to find result checkr : ${result_checker}")
    message(SEND_ERROR "Result checker missing!")
endif()

message( "Executing: ${result_checker} ${output_test} ${test_file_info} " )

# Timeout after 2 minutes
execute_process(
   COMMAND ${result_checker} ${output_test} ${test_file_info}
   TIMEOUT 60
   RESULT_VARIABLE PROCESSRESULT
)

if ( NOT ${PROCESSRESULT} EQUAL 0 )
  message(SEND_ERROR "Compare Tool execution failed!")
  set(test_not_successful TRUE)
endif()

if( test_not_successful )
   message( SEND_ERROR "Test Failed! See messages above to see what went wrong!" )
endif( test_not_successful )
