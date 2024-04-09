#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "Build live555, INSTALL_DIR=$INSTALL_DIR, BUILD_DIR=$BUILD_DIR"

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

pushd ${SCRIPT_DIR}/live555

./genMakefiles armlinux-no-openssl
CROSS_COMPILE=$CROSS_COMPILE make clean
CROSS_COMPILE=$CROSS_COMPILE make -j4
mkdir -p $PWD/install_tmp
DESTDIR=$PWD/install_tmp make install
cp -r $PWD/install_tmp/usr/local/* $INSTALL_DIR

popd
