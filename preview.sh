#!/bin/bash

ninja -j8 -C build
pwd

DISPLAY_NUM=":1"
SCREEN_SIZE="1280x720"
XE_SERVER="Xephyr $DISPLAY_NUM -screen $SCREEN_SIZE -ac &"
BUILD="./build/bin"
WM_EXEC="$BUILD/helios"


killall Xephyr
killall "$WM_EXEC"

echo "Starting Xephyr on $DISPLAY_NUM..."
eval "$XE_SERVER"
sleep 1

echo "Running Helios on display $DISPLAY_NUM..."
DISPLAY="$DISPLAY_NUM" "$WM_EXEC" &


echo "Press Ctrl+C to stop the window manager and Xephyr."
wait
