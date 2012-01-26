
del testResults\*

ctest -D ExperimentalTest -C Release --no-compress-output

cd Testing

cd 2*

copy /Y Test.xml ..\..\..\CTestResults.xml

cd ..

cd ..

