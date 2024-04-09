#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

#Default configuration
CONFIG_DEMUXER="--enable-demuxer=mov --enable-demuxer=mpegts --enable-demuxer=image2 --enable-demuxer=mjpeg --enable-demuxer=pcm_s16le --enable-demuxer=aac --enable-parser=aac --enable-decoder=aac --enable-mdct --enable-fft"
CONFIG_DECODER="--enable-decoder=mjpeg --enable-decoder=pcm_s16le"
CONFIG_OTHER=""

if [ ${SDK_VER} = uclibc ]; then
  CCPREFIX=arm-cvitek-linux-uclibcgnueabihf-
  ARCH=arm
  #disable demuxer and decoder
  CONFIG_DEMUXER="--enable-small --disable-demuxer=mov --disable-demuxer=mpegts --disable-demuxer=image2 --disable-demuxer=mjpeg --disable-demuxer=pcm_s16le"
  CONFIG_DECODER="--disable-decoder=mjpeg --disable-decoder=hevc --disable-decoder=h264 --disable-decoder=pcm_s16le"
elif [ ${SDK_VER} = 64bit ]; then
  CCPREFIX=aarch64-linux-gnu-
  ARCH=aarch64
elif [ ${SDK_VER} = glibc_riscv64 ]; then
  CCPREFIX=riscv64-unknown-linux-gnu-
  ARCH=riscv64
  CONFIG_OTHER="--enable-pic --cpu=rv64imafd"
elif [ ${SDK_VER} = musl_riscv64 ]; then
  CCPREFIX=riscv64-unknown-linux-musl-
  ARCH=riscv64
  CONFIG_OTHER="--enable-pic --cpu=rv64imafd"
  sed -i "45s/__off_t/off_t/g" ${SCRIPT_DIR}/ffmpeg/libavformat/file.c
else
  CCPREFIX=arm-linux-gnueabihf-
  ARCH=arm
fi

pushd $BUILD_DIR
$SCRIPT_DIR/ffmpeg/configure \
    --enable-cross-compile \
    --cross-prefix=${CCPREFIX} \
    --arch=${ARCH} \
    --target-os=linux \
    --disable-avdevice \
    --disable-avfilter \
    --disable-swscale \
    --disable-everything \
    --enable-shared \
    --enable-muxer=mp4 \
    --enable-muxer=mov \
    --enable-muxer=mpegts \
    $CONFIG_DEMUXER \
    $CONFIG_DECODER \
    $CONFIG_OTHER \
    --enable-protocol=file \
    --prefix=$INSTALL_DIR
## extra flags
# --enable-gpl
# --enable-libx264
# --enable-nonfree
# --enable-libaacplus
# --extra-cflags="-I/my/path/were/i/keep/built/arm/stuff/include"
# --extra-ldflags="-L/my/path/were/i/keep/built/arm/stuff/lib"
# --extra-libs=-ldl
make -j4
make install
popd
