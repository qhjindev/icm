#!/bin/bash
if [ -z "$LR_ROOT" ]; then
  echo " Missing LR_ROOT environment "
fi

RPATH=`readlink -f $0`
OPATH=`pwd`
CPATH=`dirname $RPATH`

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
cd $CPATH
rm -rf cmake_server_build
mkdir cmake_server_build
cd cmake_server_build
checkReturn " change to cmake_host_build "
cmake .. -DCMAKE_INSTALL_PREFIX=$LR_ROOT
checkReturn " execute cmake "
make -j4
checkReturn " execute make "
make install
checkReturn " install icm "

cd $OPATH
