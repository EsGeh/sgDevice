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

set usb_dev /dev/ttyACM0
set dev_version 2
set temp_dir "$BASE_DIR/arduino-tempdir"

# (syntax: short/long/description)
set options_descr \
	'h/help/print help' \
	"d/dev=/usb device. default: $usb_dev" \
	"v/dev-ver=/sgDevice version. default: $dev_version"

#################################################
# functions
#################################################

function print_help
	echo "usage: "(status -f)" [OPTIONS...] [CMD...]"
	echo "  compile and upload the software to the sgDevice"
	echo "OPTIONS:"
	print_options_descr $options_descr
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
else
	if set -q _flag_dev
		set usb_dev (readlink -m $_flag_dev)
	end
	if set -q _flag_dev_ver
		set dev_version $_flag_dev_ver
	end
end

if test $dev_version -eq 1
	arduino --upload "$BASE_DIR/src/arduino/sgDevice/sgDevice.ino"
else if test "$dev_version" -eq "2"
	echo "connected boards: "
	mkdir -p "$temp_dir"
	and arduino-cli board list
	and echo "compiling: "
	cd "$temp_dir"
	and arduino-cli compile --fqbn arduino:avr:mega --output "sgDevice_ver2" "$BASE_DIR/src/arduino/sgDevice_ver2"
	and arduino-cli upload --fqbn arduino:avr:mega --input "sgDevice_ver2" --port $usb_dev "$BASE_DIR/src/arduino/sgDevice_ver2"
	cd -
else
	echo "unknown device version"
	exit 1
end
