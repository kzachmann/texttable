#!/bin/bash
# Building and running the unit tests requires the unit test framework cmocka (https://cmocka.org/)
set -x #echo on
DIR_BIN=bin
DIR_SRC=src
DIR_COV=doc/coverage
COV_NAME=coverage

rm -r $DIR_COV
mkdir -p $DIR_BIN
mkdir -p $DIR_COV

rm texttable.gcno
rm texttable.gcda
rm texttable_test.gcno
rm texttable_test.gcda

## Build
CFLAGS="-std=c11 -O2 -g -pedantic -Wall -Wextra -Wpointer-arith -Wshadow -Wstrict-prototypes --coverage -DUNIT_TESTING"
LIBS="-lcmocka"
gcc $CFLAGS $DIR_SRC/texttable.c $DIR_SRC/texttable_test.c $LIBS -o $DIR_BIN/texttable_test

## Run test cases
./$DIR_BIN/texttable_test

## Create coverage report
lcov --rc lcov_branch_coverage=1 --no-external --capture --directory . --output-file $COV_NAME.info
genhtml --legend --branch-coverage --function-coverage $COV_NAME.info --output-directory $DIR_COV