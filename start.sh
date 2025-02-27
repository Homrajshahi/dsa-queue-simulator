#!/bin/bash

# Build both programs
make clean
make all

# Start the simulator with proper graphics settings
SDL_VIDEODRIVER=x11 SDL_RENDER_DRIVER=software ./bin/simulator &

# Wait a moment for simulator to initialize
sleep 2

# Start the vehicle generator
./bin/traffic_gen &

# Wait for user input to stop
echo "Press Enter to stop the simulation"
read

# Kill both programs
killall simulator traffic_gen

