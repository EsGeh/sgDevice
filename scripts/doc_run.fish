#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))

set doc_dir $BASE_DIR/doc

# where to install:
set install $doc_dir/install

set baud_rate (cat "$BASE_DIR/src/global/Constants.h" | grep '^#define PC_RATE' | cut -d' ' -f3)

status is-full-job-control
echo "full job control: $status"

./ttymidi/ttymidi --baudrate=$baud_rate --serialdevice=/dev/ttyACM0 --name=ttymidi_test &

set tty_pid (ps -C ttymidi -o pid=)
# set tty_pid (jobs -l | awk '{print $2}')

echo "ttymidi pid: $tty_pid"

function on_exit
	echo "killing tty ($tty_pid)..."
	kill $tty_pid
	echo "done"
end

trap on_exit EXIT

pd -noaudio \
	-alsamidi \
	-midiindev 1 \
	-noprefs \
	-path "$install" \
	-lib "sgDevice" \
	"$doc_dir/sgDevice_doc.pd"
