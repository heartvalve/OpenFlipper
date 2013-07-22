# some argument checking:
# test_cmd is the command to run with all its arguments
if( NOT test_cmd )
   message( FATAL_ERROR "Variable test_cmd not defined" )
endif( NOT test_cmd )

# convert the space-separated string to a list
separate_arguments( test_args )

# ===================================================
# Run OpenFlipper with the given script
# ===================================================

if ( NOT EXISTS ${test_cmd} )
    message("Unable to find test command : ${test_cmd}")
    message(SEND_ERROR "Test Executable missing!")
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
