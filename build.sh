#!/bin/bash
g++ -g bluebear.cpp main.cpp -Iinclude/lua -Iinclude/pepe -Llib -llua -ldl -o bluebear
