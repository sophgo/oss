#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

BUILD_DIR=$BUILD_DIR \
INSTALL_DIR=$INSTALL_DIR \
SYSROOT_PATH=$SYSROOT_PATH \
SDK_ARCH=$SDK_VER \
EXTRA_CMAKE_FLAGS="-DFLATBUFFERS_BUILD_TESTS=OFF" \
    $SCRIPT_DIR/build_generic_cmake.sh flatbuffers
