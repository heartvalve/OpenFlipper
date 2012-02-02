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
# Compare
# ===================================================



if ( WIN32 )
  set(result_checker "${result_checker}.exe")

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
