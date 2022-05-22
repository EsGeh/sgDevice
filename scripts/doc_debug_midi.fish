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
	echo "not implemented for sgDevice ver. 1"
else
	mkdir -pv "$LOG_DIR"
	echo "turn down all analog controls!"
	read --prompt-str "press any key when ready"

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
		"doc/sgDevice-test.pd" \
		2> "$LOG_DIR/pd_log.log" &
	sleep 1
	and aconnect 'sgDevice 2' 'Pure Data'
	set pd_pid (ps -C pd -o pid=)
	echo "pd_pid: $pd_pid"

	amidi -p 'virtual' --dump > "$LOG_DIR/raw_log.log" &
	sleep 1
	and aconnect 'sgDevice 2' 'Client-130'

	set amidi_pid (ps -C amidi -o pid=)
	echo "amidi_pid: $amidi_pid"
	echo "slowly turn the first analog control and check '$LOG_DIR' for output"

	read --prompt-str "press any key when done"

	kill $pd_pid
	kill $amidi_pid

	echo "done"

end
