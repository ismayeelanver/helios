#!/bin/bash

# Set display variable for Xephyr
DISPLAY_NUM=":1"
SCREEN_SIZE="1280x720"
XE_SERVER="Xephyr $DISPLAY_NUM -screen $SCREEN_SIZE -ac &"
WM_EXEC="./build/bin/helios"  # Replace with the path to your compiled WM binary

# Kill any existing Xephyr and X sessions on DISPLAY_NUM
killall Xephyr
killall "$WM_EXEC"

# Start Xephyr and run WM
echo "Starting Xephyr on $DISPLAY_NUM..."
eval "$XE_SERVER"
sleep 1

echo "Running Helios on display $DISPLAY_NUM..."
DISPLAY="$DISPLAY_NUM" "$WM_EXEC" &

# Wait for user to press Ctrl+C to stop
echo "Press Ctrl+C to stop the window manager and Xephyr."
wait
