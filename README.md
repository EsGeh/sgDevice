# sgDevice

software utilities to use my homemade midi input device with puredata

## Build Dependencies

- fish shell
- autotools
- arduino
- arduino-cli

## Run Dependencies

- [puredata](https://puredata.info/): sgDevice is mainly to be used with puredata.
- [ttymidi](https://github.com/cjbarnes18/ttymidi): This software can be used to connect the usb input/output to alsa on linux

## Preparing the Repository, and cleaning

- Make shure to issue this command before any other:

		$ ./scripts/init.fish
	
	This will download git repositories and dependencies and install them into the file hierarchy.

- Cleaning the file hierarchy:

		$ ./scripts/exit.fish

## Build and Install

- (only sgDevice ver 2)

	- turn the Arduino into a native MIDI device:

		The case has to be open in order to set a jumper.
		This (hopefully) has only to be done once!

		plug the device in the PC via USB, then run:

			$ ./scripts/arduino_upload_usb_firmware.fish

		(the script will guide you)

	- set midi device name:

		plug the device in the PC via USB, then run:

			$ ./scripts/midi_set_device_name.fish

- build and install the software on sgDevice (arduino part):

	plug the device in the PC via USB, then run:

		$ ./scripts/arduino_upload.fish

	(the script will guide you)

- build and install the PC part (puredata library):

		$ ./scripts/build.fish build install
	
	When running pd, you have 2 options:

	- start pd from the command line like this:

			$ pd [-path <install_path>] -lib sgDevice

	- add this to `~/.pdsettings` :

			...
			loadlib1: sgDevice
			...

		you might also have to adjust the library search path:

			...
			path1: <install_path>
			...

	(manuall adjust the numbering!)

## Debug serial communication

	$ ./scripts/arduino_usbdump.fish

## Documentation

### init docu

	$ ./scripts/doc_init.fish

(!): You'll also need some way to redirect usb midi to alsa, e.g. [ttymidi](https://github.com/cjbarnes18/ttymidi).

### run docu

	$ ./scripts/doc_run.fish

### cleanup docu

	$ ./scripts/doc_exit.fish
