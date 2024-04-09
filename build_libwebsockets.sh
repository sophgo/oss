#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

BUILD_DIR=$BUILD_DIR \
INSTALL_DIR=$INSTALL_DIR \
SYSROOT_PATH=$SYSROOT_PATH \
SDK_ARCH=$SDK_VER \
EXTRA_CMAKE_FLAGS="-DZLIB_INCLUDE_DIR=../../zlib/include -DZLIB_LIBRARY=../../install/zlib/lib/libz.so -DLWS_OPENSSL_INCLUDE_DIRS=../../install/openssl/include/ -DLWS_OPENSSL_LIBRARIES=../../install/openssl/lib/libssl.so;../../install/openssl/lib/libcrypto.so" \
    $SCRIPT_DIR/build_generic_cmake.sh libwebsockets

