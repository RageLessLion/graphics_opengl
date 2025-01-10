#!/bin/bash

cd /Users/napoleansolo/graphics_opengl/build || exit
cmake --build .
cd /Users/napoleansolo/graphics_opengl/build/bin/Debug
./GFXFramework 
