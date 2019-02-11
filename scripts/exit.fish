#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))
set DEP_DIR $BASE_DIR/dependencies

eval $SCRIPTS_DIR/clean_autotools.fish
eval $SCRIPTS_DIR/doc_exit.fish

rm -rf -v $DEP_DIR
