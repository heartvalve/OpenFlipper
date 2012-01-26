
del testResults\*

ctest -D ExperimentalTest -C Release --no-compress-output

copy Testing\2*\Test.xml ..\CTestResults.xml
