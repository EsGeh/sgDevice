#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))
set DEP_DIR $BASE_DIR/dependencies

if test ! -e $SCRIPTS_DIR/utils/cmd_args.fish
	echo "error: fishshell-cmd-opts not installed!"
	exit 1
end

source $SCRIPTS_DIR/utils/cmd_args.fish

#################################################
# variables
#################################################

set doc_dir $BASE_DIR/doc

#################################################
# functions
#################################################

function print_help
	echo "install library and dependencies locally into '$doc_dir'"
end

#################################################
# actual script
#################################################

if [ "$argv[1]" = "-h" ]; or [ "$argv[1]" = "--help" ]
	print_help
	exit
end

and begin
	set cmd $SCRIPTS_DIR/build.fish --prefix "$doc_dir" install
	echo "executing: '$cmd'"
	$cmd
end

# dependencies:

# zexy:
begin
	echo "installing zexy into '$doc_dir'"
	cd $DEP_DIR/zexy
	set cmd make DESTDIR="$doc_dir" install
	echo "executing: '$cmd'"
	$cmd
	cd -
end

# structuredData:
begin
	echo "installing structuredData into '$doc_dir'"
	set cmd $DEP_DIR/structuredData/scripts/build.fish --prefix "$doc_dir" install
	echo "executing: '$cmd'"
	$cmd
end
