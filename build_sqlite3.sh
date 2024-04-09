#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ${SDK_VER} = uclibc ]; then
  AUTOMAKE_HOST=arm-cvitek-linux-uclibcgnueabihf
  CC=arm-cvitek-linux-uclibcgnueabihf-gcc
elif [ ${SDK_VER} = 64bit ]; then
  AUTOMAKE_HOST=aarch64-linux-gnu
  CC=aarch64-linux-gnu-gcc
elif [ ${SDK_VER} = glibc_riscv64 ]; then
  AUTOMAKE_HOST=riscv64-unknown-linux-gnu
  CC=riscv64-unknown-linux-gnu-gcc
elif [ ${SDK_VER} = musl_riscv64 ]; then
  AUTOMAKE_HOST=riscv64-unknown-linux-musl
  CC=riscv64-unknown-linux-musl-gcc
else
  AUTOMAKE_HOST=arm-linux-gnueabihf
  CC=arm-linux-gnueabihf-gcc
fi

pushd $BUILD_DIR
CC=$CC $SCRIPT_DIR/sqlite3/configure \
    --build=x86_64-linux-gnu \
    --host=${AUTOMAKE_HOST} \
    --prefix=$INSTALL_DIR
make -j4
make install
popd
