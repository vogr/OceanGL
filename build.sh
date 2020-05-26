#!/usr/bin/env bash

# Build the program in the directory "./build"

# By default, but the program in Release mode
#   $ ./build.sh

# Alternatively, you may specify a release type
#   $ ./build.sh "Release"
# or
#   $ ./build.sh "Debug"
# or
#   $ ./build.sh "RelWithDebInfo"

cd "$(dirname "$0")" &&
cmake -S . -B "./build" -DCMAKE_BUILD_TYPE=${1:Release} &&
cmake --build "./build"

