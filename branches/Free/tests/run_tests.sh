#!/bin/sh

#remove old results
rm -f testResults/*

#Run tests
cd ..

CTEST_BINARY=ctest

if [ $(uname) == Darwin ]; then
CTEST_BINARY=/opt/local/bin/ctest  
fi

$CTEST_BINARY -D ExperimentalTest --no-compress-output
cp Testing/`head -n 1 Testing/TAG`/Test.xml CTestResults.xml
