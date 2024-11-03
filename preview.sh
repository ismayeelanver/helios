#!/bin/zsh


ninja -C ./build

Xephyr :1 -screen 800x600 &

DISPLAY=:1

./build/bin/helios 

