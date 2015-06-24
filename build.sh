#!/bin/bash
g++ -std=c++11 -g -rdynamic src/utility.cpp src/object.cpp src/lot.cpp src/engine.cpp src/main.cpp -Iinclude/lua -Iinclude/bluebear -Llib -llua -ldl -o bbexec
