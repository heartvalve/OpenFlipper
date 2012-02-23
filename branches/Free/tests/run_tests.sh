#!/bin/sh

#remove old results
rm -f testResults/*

#Run tests
cd ..
ctest -D ExperimentalTest --no-compress-output
cp Testing/`head -n 1 Testing/TAG`/Test.xml CTestResults.xml
