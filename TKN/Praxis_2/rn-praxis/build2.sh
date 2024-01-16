#!/bin/bash

# Run CMake
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Check if CMake was successful
if [ $? -eq 0 ]; then
  # Run Make if CMake was successful
  make -C build

  # Check if Make was successful
  if [ $? -eq 0 ]; then
    # Run the compiled program
    PRED_ID=16384 PRED_IP=127.0.0.1 PRED_PORT=2001 SUCC_ID=16384 SUCC_IP=127.0.0.1 SUCC_PORT=2001 ./build/webserver 127.0.0.1 2002 49152
  else
    echo "Make failed, unable to compile the program."
  fi
else
  echo "CMake failed, unable to generate build files."
fi

