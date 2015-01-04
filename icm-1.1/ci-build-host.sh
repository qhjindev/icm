#!/bin/bash
if [ -z "$LR_HOST_ROOT" ]; then
  echo " Missing LR_HOST_ROOT environment "
fi

RPATH=`readlink -f $0`
OPATH=`pwd`
if [ -z "$RPATH" ]; then
CPATH=$OPATH
else
CPATH=`dirname $RPATH`
fi

function checkReturn
{
  if [ $? -ne 0 ]; then
    echo ""
    echo "Failed to  $1 "
    exit -1
  else
    echo " Succeed to $1 "
  fi
}

MAKE_CMD=make
if [ "`uname`" == "HP-UX" ]; then
MAKE_CMD=gmake
fi

cd $CPATH
rm -rf cmake_host_build
mkdir cmake_host_build
cd cmake_host_build
checkReturn " change to cmake_host_build "
cmake .. -DON_HOST=true -DCMAKE_INSTALL_PREFIX=$LR_HOST_ROOT
checkReturn " execute cmake "
$MAKE_CMD -j4
checkReturn " execute make "
$MAKE_CMD install
checkReturn " install icm "

cd $OPATH
