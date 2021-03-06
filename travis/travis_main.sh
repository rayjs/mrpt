#!/bin/bash

set -e   # Make sure any error makes the script to return an error code

MRPT_DIR=`pwd`
BUILD_DIR=build

CMAKE_C_FLAGS="-Wall -Wextra -Wabi -O2"
CMAKE_CXX_FLAGS="-Wall -Wextra -Wabi -O2"

function prepare_build_dir()
{
  # Make sure we dont have spurious files:
  cd $MRPT_DIR
  git clean -fd || true

  rm -fr $BUILD_DIR || true
  mkdir -p $BUILD_DIR
  cd $BUILD_DIR
}

function build ()
{
  prepare_build_dir

  # gcc is too slow and we have a time limit in Travis CI: exclude examples when building with gcc
  if [ "$CC" == "gcc" ]; then
    BUILD_EXAMPLES=FALSE
  else
    BUILD_EXAMPLES=TRUE
  fi

  if [ "$DEPS" == "minimal" ]; then
    DISABLE_PYTHON_BINDINGS=ON
  else
    DISABLE_PYTHON_BINDINGS=OFF
  fi

  VERBOSE=1 cmake $MRPT_DIR \
    -DBUILD_EXAMPLES=$BUILD_EXAMPLES \
    -DBUILD_APPLICATIONS=TRUE \
    -DBUILD_TESTING=FALSE \
    -DDISABLE_PYTHON_BINDINGS=$DISABLE_PYTHON_BINDINGS

  make -j3

  cd $MRPT_DIR
}

command_exists () {
    type "$1" &> /dev/null ;
}

function test ()
{
  # gcc is too slow and we have a time limit in Travis CI:
  if [ "$CC" == "gcc" ] && [ "$TRAVIS_OS_NAME" == "osx" ]; then
	return
  fi

  prepare_build_dir
  cmake $MRPT_DIR -DBUILD_APPLICATIONS=FALSE -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
  # Remove gdb use for coverage test reports.
  # Use `test_gdb` to show stack traces of failing unit tests.
#  if command_exists gdb ; then
#    make test_gdb
#  else
    make test
#  fi

  cd $MRPT_DIR
}

case $TASK in
  build ) build;;
  test ) test;;
esac
