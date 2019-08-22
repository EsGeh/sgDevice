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
set dev_version 2

# (syntax: short/long/description)
set options_descr \
	'h/help/print help' \
	"v/dev-version=/sgDevice version. default: $dev_version"

#################################################
# functions
#################################################

function print_help
	echo "run example patch"
	echo "USAGE: "
	echo "  "(status -f)
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
	if set -q _flag_dev_version
		set dev_version $_flag_dev_version
	end
end

#################################################
# actual script
#################################################

echo "device version: $dev_version"
if test $dev_version -eq 1

	set baud_rate (cat "$BASE_DIR/src/global/Constants.h" | grep '^#define PC_RATE' | cut -d' ' -f3)

	echo "baud rate (from '$BASE_DIR/src/global/Constants.h'): $baud_rate"

	status is-full-job-control
	echo "full job control: $status"

	eval "$SCRIPTS_DIR/ttymidi --baudrate=$baud_rate --serialdevice=/dev/ttyACM0 --name=ttymidi_test &"

	set tty_pid (ps -C ttymidi -o pid=)
	# set tty_pid (jobs -l | awk '{print $2}')

	if test "$tty_pid" != ""
		echo "ttymidi pid: $tty_pid"
	else
		echo "WARNING: sgDevice propably unplugged"
	end

	function on_exit
		echo "killing tty ($tty_pid)..."
		kill $tty_pid
		echo "done"
	end

	trap on_exit EXIT

	pd -noaudio \
		-alsamidi \
		-stderr \
		-midiindev 1 \
		-midioutdev 1 \
		-noprefs \
		-path "$DEP_DIR/zexy" \
		-lib "zexy" \
		-path "$doc_dir" \
		-lib "sgDevice" \
		-lib "structuredDataC" \
		-send "dev_version $dev_version" \
		"$doc_dir/sgDevice-help.pd" &

	if test "$tty_pid" != ""
		sleep 1
		and aconnect ttymidi_test 'Pure Data'
		and aconnect 'Pure Data':1 'sgDevice 2'
	end

	set pd_pid $last_pid
	# (ps -C pd -o pid=)

	echo "pd pid: $pd_pid"

	wait $pd_pid
else
	pd -noaudio \
		-alsamidi \
		-stderr \
		-midiindev 1 \
		-midioutdev 1 \
		-noprefs \
		-path "$DEP_DIR/zexy" \
		-lib "zexy" \
		-path "$doc_dir" \
		-lib "sgDevice" \
		-lib "structuredDataC" \
		-send "dev_version $dev_version" \
		"$doc_dir/sgDevice-help.pd" &
	sleep 1
	and aconnect 'sgDevice 2' 'Pure Data'
	and aconnect 'Pure Data':1 'sgDevice 2'
	wait
end
