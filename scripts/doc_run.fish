#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))

set doc_dir $BASE_DIR/doc

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
	-noprefs \
	-path "$doc_dir" \
	-lib "sgDevice" \
	-lib "structuredDataC" \
	"$doc_dir/sgDevice-help.pd" &

if test "$tty_pid" != ""
	sleep 1
	aconnect ttymidi_test 'Pure Data'
end

set pd_pid $last_pid
# (ps -C pd -o pid=)

echo "pd pid: $pd_pid"

wait $pd_pid
