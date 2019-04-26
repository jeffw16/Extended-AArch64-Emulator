#!/bin/bash

make clean
make -f gprofMakefile
./arm sample/t10000000 # 10 million
gprof arm gmon.out > profile_info
