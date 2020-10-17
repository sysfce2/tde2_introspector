#!/bin/bash

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq

sudo apt-get install -qq g++-9
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90

sudo apt-get install -qq unzip

sudo apt-get install -qq build-essential xorg-dev libc++-dev

sudo apt-get install -qq clang-9 
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-9 900
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-9 900

CMAKE_VERSION=3.10.1
CMAKE_VERSION_DIR=v3.10

CMAKE_OS=Linux-x86_64
CMAKE_TAR=cmake-$CMAKE_VERSION-$CMAKE_OS.tar.gz
CMAKE_URL=http://www.cmake.org/files/$CMAKE_VERSION_DIR/$CMAKE_TAR
CMAKE_DIR=$(pwd)/cmake-$CMAKE_VERSION

wget --quiet $CMAKE_URL
mkdir -p $CMAKE_DIR
tar --strip-components=1 -xzf $CMAKE_TAR -C $CMAKE_DIR
export PATH=$CMAKE_DIR/bin:$PATH