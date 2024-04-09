#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -z ${SDK_VER} ]; then
  echo "SDK_VER not set, please specify 32bit | 64bit | uclibc | glibc_riscv64 | musl_riscv64"
  exit 1
else
  echo "SDK_VER=${SDK_VER}"
fi

if [ -z ${SYSROOT_PATH} ]; then
  echo "SYSROOT_PATH not set, please specify"
  exit 1
else
  echo "SYSROOT_PATH=${SYSROOT_PATH}"
fi

if [ ! -e ${SYSROOT_PATH} ]; then
  echo "$SYSROOT_PATH not exist"
  exit 1
fi

oss_list=(
  "zlib"
  "glog"
  "flatbuffers"
  "opencv"
  "live555"
  "sqlite3"
  "ffmpeg"
  "thttpd"
  "openssl"
  "libwebsockets"
  "json-c"
  "nanomsg"
  "miniz"
  "uv"
  "cvi-json-c"
  "cvi-miniz"
)

# build tarball
rm -rf $SCRIPT_DIR/tarball/${SDK_VER}
mkdir -p $SCRIPT_DIR/tarball/${SDK_VER}
for name in ${oss_list[@]}
do
  echo "build $name tarball ${SDK_VER}"
  echo $SCRIPT_DIR/run_build.sh -n $name -t $SCRIPT_DIR/tarball/${SDK_VER} -r ${SYSROOT_PATH} -s ${SDK_VER}
  $SCRIPT_DIR/run_build.sh -n $name -t $SCRIPT_DIR/tarball/${SDK_VER} -r ${SYSROOT_PATH} -s ${SDK_VER}
done

# build install
rm -rf $SCRIPT_DIR/install/${SDK_VER}
mkdir -p $SCRIPT_DIR/install/${SDK_VER}
for name in ${oss_list[@]}
do
  echo "build $name install ${SDK_VER}"
  $SCRIPT_DIR/run_build.sh -n $name -i $SCRIPT_DIR/install/${SDK_VER} -r ${SYSROOT_PATH} -s ${SDK_VER}
done

# extract
rm -rf $SCRIPT_DIR/extract/${SDK_VER}
mkdir -p $SCRIPT_DIR/extract/${SDK_VER}
for name in ${oss_list[@]}
do
  echo "build $name extract ${SDK_VER}"
  $SCRIPT_DIR/run_build.sh -n $name \
      -e -t $SCRIPT_DIR/tarball/${SDK_VER} -i $SCRIPT_DIR/extract/${SDK_VER}
done
