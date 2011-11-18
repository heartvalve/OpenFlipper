# some argument checking:
# test_cmd is the command to run with all its arguments
if( NOT test_cmd )
   message( FATAL_ERROR "Variable test_cmd not defined" )
endif( NOT test_cmd )

# output_blessed contains the name of the "blessed" output file
if( NOT md5 )
   message( FATAL_ERROR "Variable md5 not defined" )
endif( NOT md5 )

# output_test contains the name of the output file the test_cmd will produce
if( NOT output_test )
   message( FATAL_ERROR "Variable output_test not defined" )
endif( NOT output_test )

# convert the space-separated string to a list
separate_arguments( test_args )
message( ${test_args} )

execute_process(
   COMMAND ${test_cmd} ${test_args}
)

execute_process(
   COMMAND ${CMAKE_COMMAND} -E md5sum ${output_test}
   RESULT_VARIABLE test_not_successful
   OUTPUT_VARIABLE result_md5
)

#split into its two parts
string(REPLACE " " ";" result_md5 ${result_md5})

#Get the sum
list(GET result_md5 0 result_md5)

if ( NOT result_md5 STREQUAL md5 )
  message(STATUS "MD5 missmatch")
  message(STATUS "${result_md5}")
  message(STATUS "${md5}")
  set( test_not_successful "TRUE" )
endif()

if( test_not_successful )
   message( SEND_ERROR "${output_test} md5 is ${result_md5} but should be ${md5}!" )
endif( test_not_successful )
