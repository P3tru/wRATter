#!/bin/bash

echo "Setting up ROOT_INCLUDE_PATH"
if [ -z "$ROOT_INCLUDE_PATH" ]; then
  export ROOT_INCLUDE_PATH=${PWD}/include
else
  export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${PWD}/include
fi

echo "Setting up LD_LIBRARY_PATH"
if [ -z "$LD_LIBRARY_PATH" ]; then
  export LD_LIBRARY_PATH=${PWD}
else
  export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PWD}
fi
