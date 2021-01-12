#!/bin/bash

# Used to build tests with dynamic dependencies defined by makedeps
# $1 should be path to the test
# $2 should be the name of the test without the .h
# $3 should be local path to any other deps for build
# $4 should be any flags
DEPS=$(cat $1/makedeps| grep $2 | cut -d':' -f2)
echo "g++ -o $1/$2.test $1/$2.cpp $DEPS $3 $4"
g++ -o $1/$2.test $1/$2.cpp $DEPS $3 $4