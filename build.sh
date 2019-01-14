#!/bin/bash
set -x #echo on
DIR_BIN=bin
DIR_SRC=src
mkdir -p $DIR_BIN

CFLAGS="-std=c11 -O2 -g -pedantic -Wall -Wextra -Wpointer-arith -Wshadow -Wstrict-prototypes"
gcc $CFLAGS $DIR_SRC/texttable.c $DIR_SRC/texttable_run.c -o $DIR_BIN/texttable