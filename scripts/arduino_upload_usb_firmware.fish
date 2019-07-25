#!/bin/fish
#
# script is based on this manual site:
# https://www.arduino.cc/en/Hacking/DFUProgramming8U2

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))
set DEP_DIR "$BASE_DIR/dependencies"

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

#################################################
# functions
#################################################

function print_help
	echo "usage: "(status -f)" [OPTIONS...] [CMD...]"
	echo "  upload alternative usb firmware turning the device"
	echo "  into a native MIDI device."
	echo "  Procedure is based on"
	echo "    https://www.arduino.cc/en/Hacking/DFUProgramming8U2"
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

begin
	echo "1. delete the old bootloader:"
	echo "next to the USB connector, connect the reset pin to ground"
	echo
	echo " --- direction of the USB ---"
	echo "O--O"
	echo "    "
	echo "x  x"
	echo "    "
	echo "x  x"
	echo
	echo "wait a few seconds, then remove the jumper"
	read -P "done? (press any key to continue)..."
	echo "2. program the chip"
	set cmd "dfu-programmer atmega16u2 erase"
	echo "executing: '$cmd'..."
	eval "$cmd"
	set cmd "dfu-programmer atmega16u2 flash $DEP_DIR/USBMidiKliK/USBMidiKliK_dual_mega.hex"
	echo "executing: '$cmd'..."
	eval "$cmd"
	set cmd "dfu-programmer atmega16u2 reset"
	echo "executing: '$cmd'..."
	eval "$cmd"
	echo "3. unplug the device and plug it back in"
	read -P "done? (press any key to continue)..."
	echo "done."
	echo "The device is a MIDI device now."
	echo "To switch the device back to default USB mode"
	echo "in order to upload programs,"
	echo "set a jumper next to the USB connector:"
	echo
	echo " --- direction of the USB ---"
	echo "O  x"
	echo "|   "
	echo "O  x"
	echo "    "
	echo "x  x"
	echo

end
