#!/bin/bash
g++ -std=c++11 -g src/utility.cpp src/bluebear.cpp src/main.cpp -Iinclude/lua -Iinclude/pepe -Llib -llua -ldl -o bluebear
