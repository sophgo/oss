#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

DEP_DIR=$BUILD_DIR/dep_dir
mkdir -p $DEP_DIR
# build dep libs, should remove if we could maintain a sysroot
dep_list=(
  "zlib"
)
for name in ${dep_list[@]}
do
  $SCRIPT_DIR/run_build.sh -n $name -i $DEP_DIR -r ${SYSROOT_PATH} -s ${SDK_VER}
done

ENABLE_NEON_OPTION="-DENABLE_NEON=ON -DENABLE_FAST_MATH=ON"

if [ ${SDK_VER} = uclibc ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-uclibc.cmake
elif [ ${SDK_VER} = 64bit ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-aarch64-linux.cmake
  ENABLE_NEON_OPTION=""
elif [ ${SDK_VER} = glibc_riscv64 ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-riscv64-linux.cmake
  ENABLE_NEON_OPTION=""
elif [ ${SDK_VER} = musl_riscv64 ]; then
  TOOLCHAIN_FILE=$SCRIPT_DIR/toolchain-riscv64-musl.cmake
  ENABLE_NEON_OPTION=""
else
  TOOLCHAIN_FILE=$SCRIPT_DIR/opencv/platforms/linux/arm-gnueabi.toolchain.cmake
fi

pushd $BUILD_DIR

cmake -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DCMAKE_MAKE_PROGRAM=make \
    -DCMAKE_C_FLAGS="-I $DEP_DIR/include" \
    -DCMAKE_CXX_FLAGS="-I $DEP_DIR/include" \
    -D CMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ${ENABLE_NEON_OPTION} \
    -D WITH_CUDA=OFF \
    -D WITH_MATLAB=OFF \
    -D BUILD_DOCS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_TESTS=OFF \
    -D BUILD_SHARED_LIBS=ON \
    -D BUILD_opencv_objdetect=OFF \
    -D BUILD_opencv_video=OFF \
    -D BUILD_opencv_videoio=OFF \
    -D BUILD_opencv_features2d=OFF \
    -D BUILD_opencv_flann=OFF \
    -D BUILD_opencv_highgui=OFF \
    -D BUILD_opencv_ml=OFF \
    -D BUILD_opencv_photo=OFF \
    -D BUILD_opencv_python=OFF \
    -D BUILD_opencv_shape=OFF \
    -D BUILD_opencv_stitching=OFF \
    -D BUILD_opencv_superres=OFF \
    -D BUILD_opencv_ts=OFF \
    -D BUILD_opencv_videostab=OFF \
    -D WITH_DC1394=OFF -D WITH_GPHOTO2=OFF -D WITH_LAPACK=OFF \
    -D WITH_OPENCL=OFF -D WITH_OPENGL=OFF -D WITH_VA_INTEL=OFF \
    $SCRIPT_DIR/opencv

cmake --build . --target install

popd

rm  -rf $DEP_DIR
