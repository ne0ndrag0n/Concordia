#!/bin/bash
g++ -std=c++11 -g src/utility.cpp src/object.cpp src/lot.cpp src/engine.cpp src/main.cpp -Iinclude/lua -Iinclude/pepe -Llib -llua -ldl -o bluebear
