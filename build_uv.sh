#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

EXTRA_CMAKE_FLAGS="-DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_TESTING=OFF"

BUILD_DIR=$BUILD_DIR \
INSTALL_DIR=$INSTALL_DIR \
SYSROOT_PATH=$SYSROOT_PATH \
SDK_ARCH=$SDK_ARCH \
EXTRA_CMAKE_FLAGS=$EXTRA_CMAKE_FLAGS \
	$SCRIPT_DIR/build_generic_cmake.sh uv