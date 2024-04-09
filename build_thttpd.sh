#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "Build thttpd, INSTALL_DIR=$INSTALL_DIR, BUILD_DIR=$BUILD_DIR"

if [ ${SDK_VER} = uclibc ]; then
  CROSS_COMPILE=arm-cvitek-linux-uclibcgnueabihf-
elif [ ${SDK_VER} = 64bit ]; then
  CROSS_COMPILE=aarch64-linux-gnu-
elif [ ${SDK_VER} = glibc_riscv64 ]; then
  CROSS_COMPILE=riscv64-unknown-linux-gnu-
elif [ ${SDK_VER} = musl_riscv64 ]; then
  CROSS_COMPILE=riscv64-unknown-linux-musl-
else
  CROSS_COMPILE=arm-linux-gnueabihf-
fi

pushd ${SCRIPT_DIR}/thttpd

CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++ AR=${CROSS_COMPILE}ar make clean
CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++ AR=${CROSS_COMPILE}ar make
mkdir -p $INSTALL_DIR/include
cp $PWD/thttpd.h $INSTALL_DIR/include/
cp $PWD/libhttpd.h $INSTALL_DIR/include/
cp $PWD/timers.h $INSTALL_DIR/include/
mkdir -p $INSTALL_DIR/lib
cp $PWD/libthttpd.a $INSTALL_DIR/lib/
cp $PWD/libthttpd.so $INSTALL_DIR/lib/

popd
