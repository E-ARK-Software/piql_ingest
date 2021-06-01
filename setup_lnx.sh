#!/bin/sh
#
# setup_lnx.sh
# sets up build environment for LNX
#

TARGET_PLATFORM=`uname -r`
if [ `echo "$TARGET_PLATFORM" | grep "el6"` ] ; then
    # CentOS6
    if [ "$QTDIR" == "" -o "$QTDIR" == "/usr/lib64/qt-3.3" ] ; then
        QTDIR=/usr/local/Qt-5.5.0
    fi
    if [ "$QMAKESPEC" == "" ] ; then
        QMAKESPEC=linux-g++-32
    fi
    echo "INFO: Target platform is CentOS6"
elif [ `echo "$TARGET_PLATFORM" | grep "4.4.0"` ] ; then
    # Ubuntu 16.04
    if [ "$QTDIR" == "" ] ; then
        QTDIR=/usr/local/Qt-5.5.0
    fi
    if [ "$QMAKESPEC" == "" ] ; then
        QMAKESPEC=linux-g++-32
    fi
    echo "INFO: Target platform is Ubuntu 16.04"
elif [ `uname | grep "Darwin"` ] ; then
    echo "INFO: Target platform is Mac Darwin"
else
    echo "ERROR: Unsupported target: $(uname -r)"
    return 1
fi

# Setup Qt
PATH=$QTDIR/bin:$PATH
LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export QTDIR PATH LD_LIBRARY_PATH QMAKESPEC

# QT Warnings are fatal - enable as needed
#QT_FATAL_WARNINGS=1
#export QT_FATAL_WARNINGS

export CV_ROOT=$PWD
if [ "$CV_INSTALLERS" == "" ] ; then
    export CV_INSTALLERS=$PWD/../installers
fi
export CV_RELEASES=$PWD/../releases
export CV_FWTRUNK=$PWD/../fwtrunk

# Thirdparty setup
if [ "$CV_THIRDPARTY_DIR" == "" ] ; then
    export CV_THIRDPARTY=/var/lib/cv_thirdparty
else
    export CV_THIRDPARTY=$CV_THIRDPARTY_DIR
fi

if [ "$CV_BOOST_ROOT" == "" ] ; then
    export CV_BOOST_ROOT="$CV_THIRDPARTY/boost_1_60_0"
fi

if [ "$CV_BOOST_INCLUDE" == "" ] ; then
    export CV_BOOST_INCLUDE="$CV_BOOST_ROOT/include"
fi

if [ "$CV_BOOST_LIBS" == "" ] ; then
    export CV_BOOST_LIBS="$CV_BOOST_ROOT/lib"
fi

if [ ! -d "$CV_BOOST_ROOT" -o ! -d "$CV_BOOST_INCLUDE" -o ! -d "$CV_BOOST_LIBS" ] ; then
    echo "ERROR: The BOOST folders does not exist."
    return 1
fi

# Add doc folder to path, contains class templates
export PATH=$PATH:$CV_ROOT/doc

# aliases
alias mk='make -f makefile.lnx'
alias mkd='make -f makefile.lnx debug'
alias mkr='make -f makefile.lnx release'
alias mka='make -f makefile.lnx all'
alias mkc='make -f makefile.lnx clean'
alias mkca='make -f makefile.lnx clean all'
alias mki='make -f makefile.lnx install'
alias svn='svn --config-dir $CV_ROOT/svn'
