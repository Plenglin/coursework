#!/bin/csh

java SortPerf insert 20 500 200 insert.dat
java SortPerf merge 20 500 200 merge.dat
java SortPerf quick 20 500 200 quick.dat