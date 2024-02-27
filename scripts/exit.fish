#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))
set DEP_DIR $BASE_DIR/dependencies

eval $SCRIPTS_DIR/doc_exit.fish
$SCRIPTS_DIR/build.fish clean
rm -rf -v $DEP_DIR
