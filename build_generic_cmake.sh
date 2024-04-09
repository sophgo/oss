#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ${SDK_VER} = uclibc ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-uclibc.cmake
elif [ ${SDK_VER} = 64bit ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-aarch64-linux.cmake
elif [ ${SDK_VER} = glibc_riscv64 ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-riscv64-linux.cmake
elif [ ${SDK_VER} = musl_riscv64 ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-riscv64-musl.cmake
else
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-linux-gnueabihf.cmake
fi

pushd $BUILD_DIR
cmake -G Ninja \
  -DCMAKE_BUILD_TYPE=RELEASE \
  -DCMAKE_SYSROOT=$SYSROOT_PATH \
  -DCMAKE_C_FLAGS_RELEASE=-O3 \
  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
  ${EXTRA_CMAKE_FLAGS} \
  $SCRIPT_DIR/$1
cmake --build . --target install
popd
