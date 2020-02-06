#!/bin/bash

compiletips() {
    if [ -n "$2" ] ; then
        echo -n "compile status: "
        if [ $2 -eq 0 ]; then
            echo "sucess"
        else
            echo "failed"
        fi
    fi

    if [ -n "$1" ] ; then
        echo "--------------------" $1 "----------------------"
    elif [ -z "$!" ] ; then
        echo ""
    fi

}


RPATH=$(pwd)
NPATH=$RPATH/src/main/jni
OUTDIR=$RPATH/output/lib

autocompile() {
    compiletips "run cmake to generate Makefile"
    cd $NPATH
    if [ -d build ] ; then
        rm -rf build
    fi

    mkdir build
    cd build
    cmake ..

    compiletips " begin to make current project"
    make
    compiletips "end" $?

    if [ ! -d $OUTDIR ]; then
        echo "create " $OUTDIR
        mkdir -p $OUTDIR
    fi

    find . -name \*.so | xargs -i cp -rf {} $OUTDIR
    cd $RPATH
}

runtest() {
    compiletips ""
    compiletips "run test example"
    export LD_LIBRARY_PATH=$OUTDIR
    ./gradlew build
    cd build/classes/java/main
    java cpp11.java.App
}

autocompile
runtest