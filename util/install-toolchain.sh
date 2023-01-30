#!/bin/sh

TMPDIR="/toolchain"
PREFIX="/opt/cross"

download_toolchain() {
    git clone git://sourceware.org/git/binutils-gdb.git
    git clone git://gcc.gnu.org/git/gcc.git
}

compile_toolchain() {
    ARCH=$1
    TARGET=$ARCH-elf

    mkdir -p build-$TARGET-binutils
    cd build-$TARGET-binutils
    ../binutils-gdb/configure --target=$TARGET --prefix=$PREFIX --disable-nls
    make -j$(nproc)
    make install
    cd ..

    mkdir -p build-$TARGET-gcc
    cd build-$TARGET-gcc
    ../gcc/configure --target=$TARGET --prefix=$PREFIX --enable-languages=c,c++ --disable-nls
    make -j$(nproc) all-gcc
    make install-gcc
    cd ..
}

mkdir -p $TMPDIR
cd $TMPDIR

download_toolchain

compile_toolchain "x86_64"

cd ..
rm -rf $TMPDIR
