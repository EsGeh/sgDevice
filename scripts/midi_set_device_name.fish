#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))

if test ! -e $SCRIPTS_DIR/utils/cmd_args.fish
	echo "error: fishshell-cmd-opts not installed!"
	exit 1
end

source $SCRIPTS_DIR/utils/cmd_args.fish


#################################################
# variables
#################################################

set name 'sgDevice 2'

# (syntax: short/long/description)
set options_descr \
	'h/help/print help'

#################################################
# functions
#################################################

function print_help
	echo "set device name"
	echo "usage: "(status -f)" [OPTIONS...] [NAME]"
	echo "OPTIONS:"
	print_options_descr $options_descr
	echo "ARGS:"
	echo "  NAME: default: $name"
end

#################################################
# cmd line interface
#################################################

set options (options_descr_to_argparse $options_descr)

# parse command line arguments:
argparse $options -- $argv 2>/dev/null
or begin
	print_help
	exit 1
end
if set -q _flag_h
	print_help
	exit 0
end

if [ "$argv[1]" != "" ]
	set name $argv[1]
end

#################################################
# actual script
#################################################

echo "setting device name '$name'..."

set name_in_hex (echo "$name" | tr -d '\n' | od -An -t x1)
set name_in_hex (string trim $name_in_hex)

# set device name:
amidi \
	-p 'hw:1,0,0' \
	-S "F0 77 77 77 0B $name_in_hex F7"

# restart device:
amidi \
	-p 'hw:1,0,0' \
	-S "F0 77 77 77 0A F7"
