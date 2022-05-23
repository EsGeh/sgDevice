#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))
set DEP_DIR $BASE_DIR/dependencies

set LOG_DIR "$BASE_DIR/logs"

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
set debug_mode 0

# (syntax: short/long/description)
set options_descr \
	'h/help/print help' \
	"v/dev-version=/sgDevice version. default: $dev_version" \
	"d/debug/enable debug mode: log midi input to file"

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
	if set -q _flag_debug
		set debug_mode 1
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

	# 1. start pd:
	set args -noaudio \
		-noprefs \
		-path "$doc_dir" \
		-path "$DEP_DIR/zexy" \
		-lib "zexy" \
		-lib "sgDevice" \
		-lib "structuredDataC" \
		-alsamidi \
		-stderr \
		-midiindev 1 \
		-midioutdev 1 \
		-send "dev_version $dev_version"
	if test "$debug_mode" = 1
		mkdir -pv "$LOG_DIR"
		set --append args -send "print_midi 1"
		echo "args: $args"
		pd $args "$doc_dir/sgDevice-help.pd" 2> "$LOG_DIR/pd_log.log" &
	else
		pd $args "$doc_dir/sgDevice-help.pd" &
	end
	sleep 1
	and aconnect 'sgDevice 2' 'Pure Data'
	and aconnect 'Pure Data':1 'sgDevice 2'
	
	set pd_pid (jobs -l -p)
	echo "pd_pid: $pd_pid"

	# 2. in debug mode: start midi input:
	if test "$debug_mode" = 1
		amidi -p 'virtual' --dump > "$LOG_DIR/raw_log.log" &
		sleep 1
		and aconnect 'sgDevice 2' 'Client-130'

		set amidi_pid (jobs -l -p)
		echo "amidi_pid: $amidi_pid"
	end

	wait $pd_pid
	if test $amidi_pid != ""
		kill $amidi_pid
	end
end
