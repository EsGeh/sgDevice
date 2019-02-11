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

set mode ascii
set usb_dev /dev/ttyACM0
set baud_rate (cat "$BASE_DIR/src/global/Constants.h" | grep '^#define PC_RATE' | cut -d' ' -f3)
# set baud_rate 38400
# set baud_rate 9600

# (syntax: short/long/description)
set options_descr \
	'h/help/print help' \
	"m/mode=/output mode. one of 'ascii' or 'hex'. default: $mode" \
	"d/dev=/usb device. default: $usb_dev" \
	"b/baud=/baud rate. default: $baud_rate"

#################################################
# functions
#################################################

function print_help
	echo "usage: "(status -f)" [OPTIONS...] [CMD...]"
	echo "  this script dumps out what it receives from the usb device"
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
	if set -q _flag_mode
		set mode (readlink -m $_flag_mode)
	end
	if set -q _flag_dev
		set usb_dev (readlink -m $_flag_dev)
	end
	if set -q _flag_baud
		set baud_rate (readlink -m $_flag_baud)
	end
end


# set baud rate:
stty -F $usb_dev $baud_rate
# print usb info:
stty -a -F $usb_dev

## skip arduino internal protocol messages:
sleep 3

if test $mode = 'ascii'
	tail -f $usb_dev
else
	# dump as hex:
	tail -f $usb_dev | hexdump -v -e '1/1 "x%02X\n"'
end
