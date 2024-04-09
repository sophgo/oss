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

cmake -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DCMAKE_MAKE_PROGRAM=make \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    -DBUILD_TESTING=OFF \
	-DCMAKE_VERBOSE_MAKEFILE=ON \
	-DHAVE_LOCALE_H=0 \
	-DHAVE_SYSLOG_H=0 \
	-DCMAKE_BUILD_TYPE=Release \
	-LH \
    $SCRIPT_DIR/json-c

cmake --build . --target install

popd
