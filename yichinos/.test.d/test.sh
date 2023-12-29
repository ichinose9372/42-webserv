#!/bin/bash

c++ testMainconfig.cpp \
    MainConfig.cpp \
    MainConfig.hpp \
    Locations.cpp \
    Locations.hpp \
    Servers.cpp \
    Servers.hpp \
    ExclusivePath.cpp \
    ExclusivePath.hpp \
    -arch arm64 \
    -L/usr/local/lib -lgtest -lgtest_main

./a.out 

rm -rf a.out *.gch

