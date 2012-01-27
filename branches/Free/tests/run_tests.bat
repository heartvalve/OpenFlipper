
del testResults\*

del ..\CTestResults.xml

ctest -D ExperimentalTest -C Release --no-compress-output

head.bat 1 Testing\TAG

for /f "delims=" %a in ('head.bat 1 Testing\TAG') do @set result=%a 

echo %result% 

cd Testing

cd %result%

copy /Y Test.xml ..\..\..\CTestResults.xml

cd ..

cd ..

