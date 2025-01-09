#!/bin/bash

cd /Users/napoleonsolo/gfx-framework/build || exit
cmake --build .
cd /Users/napoleonsolo/gfx-framework/build/bin/Debug
./GFXFramework 
