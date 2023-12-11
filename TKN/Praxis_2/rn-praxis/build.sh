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
    ./build/webserver localhost 1234
  else
    echo "Make failed, unable to compile the program."
  fi
else
  echo "CMake failed, unable to generate build files."
fi

