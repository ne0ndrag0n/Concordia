#!/bin/bash
g++ -std=c++14 -g -rdynamic \
include/json/jsoncpp.cpp \
src/utility.cpp \
src/lotentity.cpp \
src/eventmanager.cpp \
src/lot.cpp \
src/engine.cpp \
src/main.cpp \
src/configmanager.cpp \
-Iinclude/json \
-Iinclude/json/json \
-Iinclude/lua \
-Iinclude/bluebear \
-Llib \
-llua \
-ldl \
-o bbexec
