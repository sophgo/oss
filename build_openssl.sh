#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ${SDK_VER} = uclibc ]; then
  CROSS_COMPILE=arm-cvitek-linux-uclibcgnueabihf-
  BUILD_TARGETS=linux-armv4
elif [ ${SDK_VER} = 64bit ]; then
  CROSS_COMPILE=aarch64-linux-gnu-
  BUILD_TARGETS=linux-aarch64
elif [ ${SDK_VER} = glibc_riscv64 ]; then
  CROSS_COMPILE=riscv64-unknown-linux-gnu-
  BUILD_TARGETS=linux-generic64
elif [ ${SDK_VER} = musl_riscv64 ]; then
  CROSS_COMPILE=riscv64-unknown-linux-musl-
  BUILD_TARGETS=linux-generic64
else
  CROSS_COMPILE=arm-linux-gnueabihf-
  BUILD_TARGETS=linux-armv4
fi

#disable Abstract algorithm
#no-md2,no-md4,no-mdc2,no-ripemd

#disable Symmetric encryption algorithm
#no-des,no-rc2,no-rc4,no-rc5,no-idea,no-bf,no-cast,no-camellia

#disable Asymmetric encryption algorithm
#no-ec,no-dsa,no-ecdsa,no-dh,no-ecdh
#no-sm2 no-sm3 no-sm4

#disable Data compression algorithm
#no-comp

#no-asm 是否在编译过程中使用汇编代码加快编译过程
#no-sse2 启用/禁用SSE2指令集加速。如果你的CPU支持SSE2指令集，就可以打开，否则就要关闭
#no-rfc3779 启用/禁用实现X509v3证书的IP地址扩展
#no-threads 是否编译支持多线程的库

OPENSSL_OPTIONS=""

if [ ${SDK_VER} = uclibc ]
then
    OPENSSL_OPTIONS="\
    -DOPENSSL_SMALL_FOOTPRINT \
    -DOPENSSL_NO_CRYPTO_MDEBUG -DOPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE \
    -DOPENSSL_NO_MD2 -DOPENSSL_NO_RIPEMD -DOPENSSL_NO_MDC2 \
    -DOPENSSL_NO_RC2 -DOPENSSL_NO_RC4 -DOPENSSL_NO_RC5 -DOPENSSL_NO_IDEA -DOPENSSL_NO_BF -DOPENSSL_NO_CAMELLIA -DOPENSSL_NO_CAST \
    -DOPENSSL_NO_EC -DOPENSSL_NO_DSA -DOPENSSL_NO_EC2M -DOPENSSL_NO_EC_NISTP_64_GCC_128 \
    -DOPENSSL_NO_SM2 -DOPENSSL_NO_SM3 -DOPENSSL_NO_SM4 \
    -DOPENSSL_NO_COMP \
    -DOPENSSL_NO_SRP -DOPENSSL_NO_SRTP \
    -DOPENSSL_NO_SCTP -DOPENSSL_NO_SCRYPT -DOPENSSL_NO_SIPHASH -DOPENSSL_NO_POLY1305 \
    -DOPENSSL_NO_RFC3779 \
    -DOPENSSL_NO_ASM -DOPENSSL_NO_ARIA \
    -DOPENSSL_NO_BLAKE2 \
    -DOPENSSL_NO_CHACHA -DOPENSSL_NO_CMS -DOPENSSL_NO_CT \
    -DOPENSSL_NO_DGRAM -DOPENSSL_NO_DEVCRYPTOENG \
    -DOPENSSL_NO_EGD -DOPENSSL_NO_CAPIENG \
    -DOPENSSL_NO_SEED -DOPENSSL_NO_TS -DOPENSSL_NO_WHIRLPOOL -DOPENSSL_NO_RMD160 \
    -DOPENSSL_NO_STATIC_ENGINE -DOPENSSL_NO_ENGINE -DOPENSSL_NO_RDRAND \
    -DOPENSSL_NO_OCB -DOPENSSL_NO_FUZZ_LIBFUZZER \
    -DOPENSSL_NO_UNIT_TEST -DOPENSSL_NO_UI_CONSOLE \
    -DOPENSSL_PREFER_CHACHA_OVER_GCM \
    no-mdc2 no-md2 no-ripemd \
    no-rc2 no-rc4 no-rc5 no-idea no-bf no-cast no-camellia \
    no-ec2m no-ec no-dsa no-ecdh no-ecdsa no-ec_nistp_64_gcc_128 \
    no-sm2 no-sm3 no-sm4 \
    no-comp \
    no-srp no-srtp \
    no-sctp no-scrypt no-siphash no-poly1305 \
    no-rfc3779 \
    no-asm no-sse2 no-aria \
    no-blake2 \
    no-chacha no-cms no-ct \
    no-dtls no-deprecated no-devcryptoeng no-dgram \
    no-egd no-capieng no-dso \
    no-seed no-ts no-whirlpool no-rmd160 no-rdrand no-engine \
    no-fuzz-libfuzzer no-fuzz-afl no-ocb \
    no-unit-test no-ui-console \
    no-tls no-ssl2 no-ssl3 \
    no-hw no-threads \
    no-nextprotoneg "
fi


pushd $BUILD_DIR
CC=$CC $SCRIPT_DIR/openssl/Configure \
    ${BUILD_TARGETS} \
    --cross-compile-prefix=${CROSS_COMPILE} \
    --prefix=$INSTALL_DIR \
	shared \
    ${OPENSSL_OPTIONS}

sed -i "92,100s/CFLAGS=-Wall -O3/CFLAGS=-Wall -Os -ffunction-sections -fdata-sections/" ./Makefile
sed -i "92,100s/CXXFLAGS=-Wall -O3/CXXFLAGS=-Wall -Os -ffunction-sections -fdata-sections/" ./Makefile
sed -i "92,100s/LDFLAGS=/LDFLAGS=-Wl,--gc-sections/" ./Makefile

make -j4
make install
popd
