#!/bin/bash
g++ -std=c++11 -g -rdynamic include/json/jsoncpp.cpp src/utility.cpp src/lotentity.cpp src/lot.cpp src/engine.cpp src/main.cpp -Iinclude/json -Iinclude/json/json -Iinclude/lua -Iinclude/bluebear -Llib -llua -ldl -o bbexec
