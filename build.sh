#!/bin/bash
g++ -g src/bluebear.cpp src/main.cpp -Iinclude/lua -Iinclude/pepe -Llib -llua -ldl -o bluebear
