#!/bin/bash

make -C ..

c++ mainTest.cpp -arch arm64 -L/usr/local/lib -lgtest -lgtest_main

./a.out